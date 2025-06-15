#!/bin/bash
set -e

# 安装 JSONCPP 1.9.6
echo "===== 检查 JSONCPP 1.9.6 ====="
if pkg-config --modversion jsoncpp | grep -q "1.9.6"; then
    echo "[跳过] 已安装 JSONCPP 1.9.6"
else
    for attempt in {1..3}; do
        echo "开始安装 JSONCPP (尝试 $attempt/3)"
        sudo apt install -y build-essential cmake &>/dev/null
        rm -rf jsoncpp-1.9.6  # 清理旧文件
        wget -q https://github.com/open-source-parsers/jsoncpp/archive/1.9.6.tar.gz
        tar -xzf 1.9.6.tar.gz
        cd jsoncpp-1.9.6
        mkdir build && cd build
        cmake -DCMAKE_BUILD_TYPE=Release .. &>/dev/null
        make -j$(nproc) &>/dev/null
        sudo make install &>/dev/null
        
        # 验证安装
        if pkg-config --modversion jsoncpp | grep -q "1.9.6"; then
            echo "[成功] JSONCPP 1.9.6 已安装"
            break
        else
            echo "[失败] 验证未通过，正在重试..."
            cd ../.. && rm -rf jsoncpp-1.9.6*
            sleep $((attempt*2))  # 重试延迟
        fi
    done
fi

# 安装 Protobuf 3.21.12
echo -e "\n===== 检查 Protobuf 3.21.12 ====="
if protoc --version | grep -q "3.21.12"; then
    echo "[跳过] 已安装 Protobuf 3.21.12"
else
    for attempt in {1..3}; do
        echo "开始安装 Protobuf (尝试 $attempt/3)"
        sudo apt install -y autoconf libtool &>/dev/null
        rm -rf protobuf-3.21.12  # 清理旧文件
        wget -q https://github.com/protocolbuffers/protobuf/releases/download/v3.21.12/protobuf-all-3.21.12.tar.gz
        tar -xzf protobuf-all-3.21.12.tar.gz
        cd protobuf-3.21.12
        ./configure --prefix=/usr/local/protobuf-3.21.12 &>/dev/null
        make -j$(nproc) &>/dev/null
        sudo make install &>/dev/null
        
        # 配置环境
        echo 'export PATH=/usr/local/protobuf-3.21.12/bin:$PATH' | sudo tee /etc/profile.d/protobuf.sh &>/dev/null
        source /etc/profile.d/protobuf.sh
        sudo ldconfig
        
        # 验证安装
        if protoc --version | grep -q "3.21.12"; then
            echo "[成功] Protobuf 3.21.12 已安装"
            break
        else
            echo "[失败] 验证未通过，正在重试..."
            cd .. && rm -rf protobuf-3.21.12*
            sudo rm -rf /usr/local/protobuf-3.21.12  # 清理安装目录
            sleep $((attempt*2))
        fi
    done
fi