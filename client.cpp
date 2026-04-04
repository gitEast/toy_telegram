// g++ -std=c++17 client.cpp message.cpp framer.cpp -o client -pthread
#include <arpa/inet.h>  // socket
#include <unistd.h>     // close

#include <iostream>
#include <thread>

#include "framer.h"
#include "message.h"

/**
 * @brief 专门负责“接收消息”的线程
 */
void receive_msg(int sock) {
  char buffer[1024];
  Framer framer;  // 消息分帧器

  while (true) {
    int len = recv(sock, buffer, sizeof(buffer), 0);
    if (len <= 0) {
      std::cout << "服务器断开连接\n";
      break;
    }

    framer.append(buffer, len);
    while (framer.has_message()) {
      std::string one_msg = framer.next_message();
      Message m = deserialize(one_msg);
      // 广播 / 系统 信息
      if (m.type == MessageType::Broadcast || m.type == MessageType::System) {
        std::cout << "[" << m.username << "]: " << m.msg << std::endl;
      } else if (m.type == MessageType::Private) {
        std::cout << "[私聊]" << "[" << m.username << "]: " << m.msg
                  << std::endl;
      }
    }
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
  std::cout << "请输入用户名：";
  std::string username;
  std::getline(std::cin, username);
  Message login_msg;
  login_msg.type = MessageType::Login;
  login_msg.username = username;
  std::string login_serialized = serialize(login_msg);
  send(sock, login_serialized.c_str(), login_serialized.size(), 0);
  // 5. 启动接收线程
  std::thread t(receive_msg, sock);
  t.detach();
  // 6. 主线程负责发送消息
  std::string input;
  while (true) {
    std::getline(std::cin, input);
    Message m;
    // 判断 私聊 / 广播
    if (input.rfind("/msg ", 0) == 0) {
      m.type = MessageType::Private;
      size_t first_space = input.find(' ', 5);
      if (first_space == std::string::npos) {
        std::cout << "格式错误：/msg 用户名 内容\n";
        continue;
      }
      m.username = input.substr(5, first_space - 5);  // 私聊对象
      m.msg = input.substr(first_space + 1);
    } else {
      m.type = MessageType::Broadcast;
      m.msg = input;
    }
    std::string m_serialized = serialize(m);
    send(sock, m_serialized.c_str(), m_serialized.size(), 0);
  }
  close(sock);
  return 0;
}
