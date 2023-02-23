#!/usr/bin/env bash

set -e

rm -rf ./server && mkdir -p ./server && cd ./server || exit

openssl req \
  -new \
  -newkey rsa:2048 \
  -days 365 \
  -nodes \
  -x509 \
  -subj "/C=CN/O=EMQ Technologies Co., Ltd/CN=EMQ Server CA" \
  -keyout server-ca.key \
  -out server-ca.crt

openssl genrsa -out server.key 2048

cat <<EOF >./openssl.cnf
[policy_match]
countryName             = match
stateOrProvinceName     = optional
organizationName        = optional
organizationalUnitName  = optional
commonName              = supplied
emailAddress            = optional

[req]
default_bits       = 2048
distinguished_name = req_distinguished_name
req_extensions     = req_ext
x509_extensions    = v3_req
prompt             = no

[req_distinguished_name]
commonName          = Server

[req_ext]
subjectAltName = @alt_names

[v3_req]
subjectAltName = @alt_names

[alt_names]
IP.1 = 1.1.1.1
# DNS
# DNS.1 = tls.emqx.io
EOF

openssl req -new -key server.key -config openssl.cnf -out server.csr

openssl x509 -req \
  -days 365 \
  -sha256 \
  -in server.csr \
  -CA server-ca.crt \
  -CAkey server-ca.key \
  -CAcreateserial -out server.crt \
  -extensions v3_req -extfile openssl.cnf
