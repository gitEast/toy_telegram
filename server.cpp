// g++ -std=c++17 server.cpp -o server -pthread
#include <arpa/inet.h>  // close
#include <unistd.h>     // socket 相关

#include <iostream>
#include <thread>
#include <vector>

// 用于保存所有的客户端连接
std::vector<int> clients;

/**
 * @brief 广播函数
 * 把一条消息发送给所有客户端（除了发送者）
 */
void broadcast(const std::string& msg, int sender_fd) {
  for (int client : clients) {
    if (client != sender_fd) {
      // send: 向 socket 发送数据
      send(client, msg.c_str(), msg.size(), 0);
    }
  }
}

/**
 * @brief 每个客户端都会运行这个函数（在新线程中）
 */
void handle_client(int client_fd) {
  char buffer[1024];

  while (true) {
    // recv: 从 socket 中接收数据
    int len = recv(client_fd, buffer, sizeof(buffer), 0);

    // 如果客户端断开
    if (len <= 0) {
      std::cout << "客户端断开：" << client_fd << std::endl;
      break;
    }

    // 把接收到的数据转成 string
    std::string msg(buffer, len);
    std::cout << "收到：" << msg << std::endl;

    // 转发给其他客户端
    broadcast(msg, client_fd);
  }

  // 关闭连接
  close(client_fd);
}

/**
 * @brief 主函数：服务器入
 */
int main() {
  // 1. 创建 socket
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  // 2. 定义结构地址
  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;          // IPv4
  server_addr.sin_port = htons(8080);        // port 8080
  server_addr.sin_addr.s_addr = INADDR_ANY;  // 监听所有网卡
  // 3. 绑定地址
  bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr));
  // 4. 开始监听
  listen(server_fd, 10);
  std::cout << "服务器启动，端口 8080..." << std::endl;
  while (true) {
    // 5. 等待客户端连接（阻塞）
    int client_fd = accept(server_fd, nullptr, nullptr);
    std::cout << "新客户端连接：" << client_fd << std::endl;
    // 6. 保存客户端
    clients.push_back(client_fd);
    // 7. 为该客户端创建线程
    std::thread t(handle_client, client_fd);
    // detach: 线程独立运行，不需要 join
    t.detach();
  }
  return 0;
}
