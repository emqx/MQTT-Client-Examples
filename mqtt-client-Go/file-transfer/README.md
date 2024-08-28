# Golang MQTT File Transfer Example

## Install

```shell
go get -u github.com/eclipse/paho.mqtt.golang
```

### Run

```bash
go run main.go -host 127.0.0.1 -port 1883 -file-id file-id-1 -file-name uploaded-test-file.txt
```

## Params

```text
usage: go run main.go -h
  -client-id string
        MQTT client id (default "go-simple")
  -file-id string
        file id for uploading
  -file-name string
        file name for uploading
  -host string
        MQTT server hostname or IP (default "127.0.0.1")
  -port string
        MQTT server port (default "1883")
  -qos int
        QoS of MQTT client options (default 1)
  -segment-size int
        custom segment size for chunked transfer (default 10240)
```
