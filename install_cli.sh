#!/bin/bash

wget http://10.30.0.131:9090/download/chatroom-cli.tar

docker load < chatroom-cli.tar

rm chatroom-cli.tar