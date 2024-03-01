package main

import (
	"crypto/sha256"
	"encoding/hex"
	"encoding/json"
	"errors"
	"flag"
	"fmt"
	"log"
	"net"
	"os"
	"path/filepath"

	MQTT "github.com/eclipse/paho.mqtt.golang"
)

var (
	host        string
	port        string
	clientId    string
	fileId      string
	fileName    string
	segmentSize int
	qos         int
)

func init() {
	flag.StringVar(&host, "host", "127.0.0.1", "MQTT server hostname or IP")
	flag.StringVar(&port, "port", "1883", "MQTT server port")
	flag.StringVar(&clientId, "client-id", "go-simple", "MQTT client id")
	flag.StringVar(&fileId, "file-id", "", "file id for uploading")
	flag.StringVar(&fileName, "file-name", "", "file name for uploading")
	flag.IntVar(&segmentSize, "segment-size", 10*1024, "custom segment size for chunked transfer")
	flag.IntVar(&qos, "qos", 1, "QoS of MQTT client options")
	flag.Parse()
}

func main() {
	data, err := os.ReadFile(fileName)
	if err != nil {
		log.Fatal(err)
	}

	// Calculate file checksum.
	h := sha256.New()
	h.Write(data)
	sum := hex.EncodeToString(h.Sum(nil))
	log.Printf("%s checksum: %s", fileName, sum)

	// Setup client options.
	MQTT.DEBUG = log.New(os.Stdout, "[DEBUG] ", 0)
	opts := MQTT.NewClientOptions().AddBroker("tcp://" + net.JoinHostPort(host, port))
	opts.SetClientID(clientId)
	opts.SetDefaultPublishHandler(func(client MQTT.Client, msg MQTT.Message) {
		log.Printf("TOPIC: %s", msg.Topic())
		log.Printf("MSG: %s", msg.Payload())
	})

	// Create and start a client using the above options.
	c := MQTT.NewClient(opts)
	if token := c.Connect(); token.Wait() && token.Error() != nil {
		log.Fatal(token.Error())
	}

	type Meta struct {
		Name     string `json:"name"`
		Size     int    `json:"size"`
		Checksum string `json:"checksum"`
	}

	meta, err := json.Marshal(&Meta{
		Name:     filepath.Base(fileName),
		Size:     len(data),
		Checksum: sum,
	})
	if err != nil {
		log.Fatal(err)
	}
	log.Printf("Meta info: %s", meta)

	// Publish metadata.
	if token := c.Publish(fmt.Sprintf("$file/%s/init", fileId), byte(qos), false, meta); token.Error() != nil {
		log.Fatal(token.Error())
	}

	chunks, err := chunk(data, segmentSize)
	if err != nil {
		log.Fatal(err)
	}

	for i := 0; i < len(chunks); i++ {
		token := c.Publish(fmt.Sprintf("$file/%s/%d", fileId, chunks[i].Offset), byte(qos), false, chunks[i].Data)
		if token.Error() != nil {
			log.Fatal(token.Error())
		}
		token.Wait()
	}

	token := c.Publish(fmt.Sprintf("$file/%s/fin/%d", fileId, len(data)), byte(qos), false, "")
	if token.Error() != nil {
		log.Fatal(token.Error())
	}
	token.Wait()
}

type Chunk struct {
	Offset int
	Data   []byte
}

func chunk(data []byte, segmentSize int) ([]Chunk, error) {
	if len(data) <= 0 || segmentSize <= 0 {
		return nil, errors.New("invalid arguments")
	}

	size := len(data) / segmentSize
	if len(data)%segmentSize != 0 {
		size++
	}

	chunks := make([]Chunk, 0, size)
	for i := 0; i < size; i++ {
		offset := i * segmentSize
		end := offset + segmentSize
		if end > len(data) {
			end = len(data)
		}
		c := Chunk{
			Offset: offset,
			Data:   data[offset:end],
		}
		chunks = append(chunks, c)
	}
	return chunks, nil
}
