set -e  # 出错即停止
g++ -std=c++17 \
    client.cpp \
    message.cpp \
    framer.cpp \
    user_manager.cpp \
    -o client \
    -pthread
echo "编译成功！已生成 client"
