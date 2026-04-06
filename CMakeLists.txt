# 1. 声明 CMake 最低版本要求
cmake_minimum_required(VERSION 3.10)

# 2. 项目名称
project(ToyTelegram)

# 3. 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 4. 指定头文件搜索路径
include_directories(include/common include/server)

# 5. 定义公共源码变量
set(COMMON_SOURCES 
    src/common/framer.cpp
    src/common/message.cpp
)

# 6. 生成服务端可执行文件
add_executable(server
    src/server/server.cpp
    src/server/user_manager.cpp
    ${COMMON_SOURCES}
)

# 7. 生成客户端可执行文件
add_executable(client
    src/client/client.cpp
    ${COMMON_SOURCES}
)
