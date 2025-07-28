#!/bin/bash

if ! docker image inspect "chatroom-cli:v1.0" >/dev/null 2>&1; then

#  获取客户端镜像
  wget http://$1:9090/download/chatroom-cli.tar
  docker load < chatroom-cli.tar
  rm chatroom-cli.tar                                  
fi


#  运行客户端镜像
if [ ! -d "/tmp/history" ]; then
  mkdir /tmp/history
fi

if [ ! -d "/tmp/download" ]; then
  mkdir /tmp/download
fi

if [ ! -d "/tmp/upload" ]; then
  mkdir /tmp/upload
fi

clear

docker run -it --rm -v "/tmp/history:/Chatroom/history" -v "/tmp/download:/Chatroom/download" -v "/tmp/upload:/Chatroom/upload" chatroom-cli:v1.0 ./client $1
