FROM ubuntu:25.04

RUN apt-get update && \
     apt-get install -y \
     cmake \
     g++ \
     wget  \
     protobuf-compiler \
     && rm -rf /var/lib/apt/lists/*

WORKDIR /Chatroom

COPY ./build/client .

 RUN wget -q https://github.com/open-source-parsers/jsoncpp/archive/1.9.6.tar.gz \
	tar -xzf 1.9.6.tar.gz \
    cd jsoncpp-1.9.6 \ 
    mkdir build && cd build \
    cmake -DCMAKE_BUILD_TYPE=Release .. &>/dev/null \
    make -j$(nproc) &>/dev/null

ENV TZ=Asia/Shanghai
RUN apt-get update && apt-get install -y tzdata && \
    ln -fs /usr/share/zoneinfo/$TZ /etc/localtime && \
    echo $TZ > /etc/timezone

RUN apt remove -y \
    wget \
    cmake \
    g++

RUN mkdir download

RUN mkdir history

RUN mkdir /user-home

CMD ["./client","10.30.0.131"]