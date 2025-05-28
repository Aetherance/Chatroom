#!/bin/bash

# 检查命令是否成功执行
check_installed() {
    if [ $? -ne 0 ]; then
        echo "[错误] 安装失败: $1"
        exit 1
    else
        echo "[成功] $1 安装完成"
    fi
}

# 检查库是否已安装
is_lib_installed() {
    case $1 in
        "curl")     dpkg -l libcurl4-openssl-dev &>/dev/null ;;
        "jsoncpp")  dpkg -l libjsoncpp-dev &>/dev/null ;;
        "protobuf") dpkg -l libprotobuf-dev &>/dev/null ;;
        "tacopie")  [ -d "/usr/local/include/tacopie" ] ;;
        "cpp_redis")[ -d "/usr/local/include/cpp_redis" ] ;;
        "ftxui")    [ -d "/usr/local/include/ftxui" ] ;;
        *)          return 1 ;;
    esac
}

# 更新系统包
echo "[步骤] 更新系统包..."
sudo apt-get update && sudo apt-get upgrade -y
check_installed "系统更新"

# 安装基础编译工具
echo "[步骤] 安装编译工具..."
sudo apt-get install -y build-essential cmake git pkg-config
check_installed "编译工具"

# 安装 curl
echo "[步骤] 检查 curl..."
if ! is_lib_installed "curl"; then
    sudo apt-get install -y curl libcurl4-openssl-dev
    check_installed "curl"
else
    echo "[跳过] curl 已安装"
fi

# 安装 jsoncpp
echo "[步骤] 检查 jsoncpp..."
if ! is_lib_installed "jsoncpp"; then
    sudo apt-get install -y libjsoncpp-dev
    check_installed "jsoncpp"
else
    echo "[跳过] jsoncpp 已安装"
fi

# 安装 protobuf
echo "[步骤] 检查 protobuf..."
if ! is_lib_installed "protobuf"; then
    sudo apt-get install -y libprotobuf-dev protobuf-compiler
    check_installed "protobuf"
else
    echo "[跳过] protobuf 已安装"
fi

# 安装 tacopie (cpp_redis 的依赖)
echo "[步骤] 检查 tacopie..."
if ! is_lib_installed "tacopie"; then
    git clone https://github.com/Cylix/tacopie.git
    cd tacopie
    mkdir build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make && sudo make install
    check_installed "tacopie"
    cd ../..
else
    echo "[跳过] tacopie 已安装"
fi

# 安装 cpp_redis
echo "[步骤] 检查 cpp_redis..."
if ! is_lib_installed "cpp_redis"; then
    git clone https://github.com/cpp-redis/cpp_redis.git
    cd cpp_redis
    git submodule init && git submodule update
    mkdir build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-I/usr/local/include"
    make && sudo make install
    check_installed "cpp_redis"
    cd ../..
else
    echo "[跳过] cpp_redis 已安装"
fi

# 安装 ftxui
echo "[步骤] 检查 FTXUI..."
if ! is_lib_installed "ftxui"; then
    git clone https://github.com/ArthurSonzogni/FTXUI.git
    cd FTXUI
    mkdir build && cd build
    cmake .. && make && sudo make install
    check_installed "FTXUI"
    cd ../..
else
    echo "[跳过] FTXUI 已安装"
fi

# 更新动态链接库缓存
sudo ldconfig

# 最终验证
echo "[状态] 安装结果汇总："
echo "----------------------------------------"
echo "curl:      $(is_lib_installed "curl"      && echo "[已安装]" || echo "[未安装]")"
echo "jsoncpp:   $(is_lib_installed "jsoncpp"   && echo "[已安装]" || echo "[未安装]")"
echo "protobuf:  $(is_lib_installed "protobuf"  && echo "[已安装]" || echo "[未安装]")"
echo "tacopie:   $(is_lib_installed "tacopie"   && echo "[已安装]" || echo "[未安装]")"
echo "cpp_redis: $(is_lib_installed "cpp_redis" && echo "[已安装]" || echo "[未安装]")"
echo "FTXUI:     $(is_lib_installed "ftxui"     && echo "[已安装]" || echo "[未安装]")"
echo "----------------------------------------"

echo "[完成] 所有环境配置结束！"