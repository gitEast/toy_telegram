// g++ -std=c++17 client.cpp -o client -pthread
#include <arpa/inet.h>  // close
#include <unistd.h>     // socket

#include <iostream>
#include <thread>

/**
 * @brief 专门负责“接收消息”的线程
 */
void receive_msg(int sock) {
  char buffer[1024];

  while (true) {
    int len = recv(sock, buffer, sizeof(buffer), 0);
    if (len <= 0) {
      std::cout << "服务器断开连接\n";
      break;
    }
    std::string msg(buffer, len);
    std::cout << msg << std::endl;
  }
}

int main() {
  // 1. 创建 socket
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  // 2. 设置服务器地址
  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8080);
  inet_pton(AF_INET, "127.0.0.1",
            &server_addr.sin_addr);  // 把字符串 IP 转成二进制
  // 3. 连接服务器
  connect(sock, (sockaddr*)&server_addr, sizeof(server_addr));
  // 4. 输入用户名
  std::string username;
  std::cout << "请输入用户名：";
  std::getline(std::cin, username);
  send(sock, username.c_str(), username.size(), 0);
  // 5. 启动接收线程
  std::thread t(receive_msg, sock);
  t.detach();
  // 6. 主线程负责发送消息
  std::string msg;
  while (true) {
    std::getline(std::cin, msg);
    send(sock, msg.c_str(), msg.size(), 0);
  }
  close(sock);
  return 0;
}
