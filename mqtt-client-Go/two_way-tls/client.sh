#!/usr/bin/env bash

set -e

rm -rf ./client && mkdir -p ./client && cd ./client || exit

openssl req \
  -new \
  -newkey rsa:2048 \
  -days 365 \
  -nodes \
  -x509 \
  -subj "/C=CN/O=EMQ Technologies Co., Ltd/CN=EMQ Client CA" \
  -keyout client-ca.key \
  -out client-ca.crt

openssl genrsa -out client.key 2048

openssl req -new -key client.key -out client.csr -subj "/CN=Client-$(date +'%Y%m%d%H%M%S')"

openssl x509 -req -days 365 -sha256 -in client.csr -CA client-ca.crt -CAkey client-ca.key -CAcreateserial -out client.crt
