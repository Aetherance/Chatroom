#!/bin/bash

if [ ! -d "/tmp/history" ]; then
  mkdir /tmp/history
fi

if [ ! -d "/tmp/download" ]; then
  mkdir /tmp/download
fi

sudo docker run -it --rm -v "/tmp/history:/Chatroom/history" -v "/tmp/download:/Chatroom/download" chatroom-cli:latest ./client $1