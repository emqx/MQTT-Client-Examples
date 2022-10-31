# Golang Two-Way TLS Connection Example

## Install

```shell script
go get github.com/eclipse/paho.mqtt.golang
go get github.com/gorilla/websocket
go get golang.org/x/net/proxy
```

## Run

### Certificate Generation

```bash
# generate service certificate
./server.sh
# generate client certificate
./client.sh
```

### Run Pub and Sub

```bash
go run main.go -host 127.0.0.1 -port 8883 --cafile ./server/server-ca.crt --cert ./client/client.crt --key ./client/client.key
```