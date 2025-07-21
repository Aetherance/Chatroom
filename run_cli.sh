#!/bin/bash

if [ ! -d "/tmp/history" ]; then
  mkdir /tmp/history
fi

if [ ! -d "/tmp/download" ]; then
  mkdir /tmp/download
fi

if [ ! -d "/tmp/upload" ]; then
  mkdir /tmp/upload
fi

docker run -it --rm -v "/tmp/history:/Chatroom/history" -v "/tmp/download:/Chatroom/download" -v "/tmp/upload:/Chatroom/upload" chatroom-cli:latest ./client $1