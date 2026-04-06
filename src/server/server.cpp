#include <arpa/inet.h>  // socket 相关
#include <unistd.h>     // close

#include <iostream>
#include <map>
#include <set>
#include <thread>

#include "framer.h"
#include "message.h"
#include "user_manager.h"

UserManager userManager;

/**
 * @brief 广播函数
 * 把一条消息发送给所有客户端（除了发送者）
 * @param sender_fd 发送者（不需要被广播的 id）
 */
void broadcast(const Message& m, int sender_fd) {
  std::string m_serialized = serialize(m);
  userManager.for_each([sender_fd, &m_serialized](int fd) {
    if (fd != sender_fd) {
      // send: 向 socket 发送数据
      send(fd, m_serialized.c_str(), m_serialized.size(), 0);
    }
  });
}
/**
 * @brief 单播
 */
void unicast(const Message& m, int sender_fd) {
  std::string m_serialized = serialize(m);
  send(sender_fd, m_serialized.c_str(), m_serialized.size(), 0);
}

/**
 * @brief 每个客户端都会运行这个函数（在新线程中）
 */
void handle_client(int client_fd) {
  char buffer[1024];
  Framer framer;

  // 1. 登录阶段
  while (true) {
    int len = recv(client_fd, buffer, sizeof(buffer), 0);
    if (len <= 0) {
      close(client_fd);  // 关闭连接
      return;
    }

    framer.append(buffer, len);

    size_t pos;
    if (framer.has_message()) {
      std::string one_msg = framer.next_message();

      Message m = deserialize(one_msg);
      if (m.type == MessageType::Unknown) continue;
      if (m.type != MessageType::Login) {
        close(client_fd);
        return;
      }
      userManager.add_user(client_fd, m.username);
      std::cout << "用户上线：" << m.username << std::endl;
      // 通知其他人
      Message join_msg;
      join_msg.type = MessageType::System;
      join_msg.username = "system";
      join_msg.msg = m.username + " 加入聊天室";
      broadcast(join_msg, client_fd);
      break;
    }
  }

  // 2. 💬 聊天阶段
  while (true) {
    int len = recv(client_fd, buffer, sizeof(buffer), 0);
    if (len <= 0) break;

    framer.append(buffer, len);

    if (framer.has_message()) {
      std::string one_msg = framer.next_message();
      Message m = deserialize(one_msg);
      if (m.type == MessageType::Broadcast) {  // 广播
        // 覆盖 username：server 不信任客户端 username
        m.username = userManager.get_username(client_fd);
        // 转发给其他客户端
        broadcast(m, client_fd);
      } else if (m.type == MessageType::Private) {  // 私聊
        std::string target_name = m.username;
        std::string sender_name = userManager.get_username(client_fd);
        if (userManager.user_exists(m.username)) {  // 用户存在
          if (userManager.are_friends(sender_name,
                                      target_name)) {  // 存在好友关系
            // 更改 username 为发送者的 name
            m.username = userManager.get_username(client_fd);
            unicast(m, userManager.get_fd(target_name));
          } else {  // 不存在好友关系
            unicast(Message{MessageType::System, "system",
                            target_name + " 不是你的好友"},
                    client_fd);
          }
        } else {  // 用户不存在，向发送者报错
          unicast(Message{MessageType::System, "system", "用户不存在"},
                  client_fd);
        }
      } else if (m.type == MessageType::AddFriend) {  // 好友申请
        // { type: "add_friend", "username": receiver_name }
        std::string sender_name = userManager.get_username(client_fd);
        std::string target_name = m.username;
        if (userManager.user_exists(target_name)) {  // 用户存在
          /*
           * 1. 加入好友申请
           * 2. 通知接收者
           */
          userManager.add_friend_request(target_name, sender_name);
          Message notify{MessageType::System, "system",
                         sender_name + " 请求添加你为好友"};
          unicast(notify, userManager.get_fd(target_name));
        } else {  // 用户不存在
          unicast(Message{MessageType::System, "system",
                          "用户 " + target_name + " 不存在"},
                  client_fd);
        }
      } else if (m.type == MessageType::AddFriendReply) {  // 回复好友申请
        // { type: "add_friend_reply", username: requester_name, msg: "ok/no" }
        std::string replier_name =
            userManager.get_username(client_fd);  // 好友申请回复方 name
        std::string requester_name = m.username;  // 好友申请方 name
        int requester_fd = userManager.get_fd(requester_name);  // 好友申请方 fd

        if (userManager.user_exists(requester_name)) {  // 申请方存在
          // 检查是否存在申请
          if (userManager.has_friend_request(replier_name, requester_name)) {
            if (m.msg == "ok") {  // 同意好友申请
              // 建立双向好友关系
              userManager.add_friend(requester_name, replier_name);
              // 通知双方
              unicast(Message{MessageType::System, "system",
                              "你已和 " + replier_name + " 成为好友"},
                      requester_fd);
              unicast(Message{MessageType::System, "system",
                              "你已和 " + requester_name + " 成为好友"},
                      client_fd);
            } else {  // 拒绝好友申请
              // 通知申请方被拒
              unicast(Message{MessageType::System, "system",
                              replier_name + " 拒绝了你的好友申请"},
                      requester_fd);
            }
            // 消除好友申请记录
            userManager.remove_friend_request(replier_name, requester_name);
          } else {
            unicast(Message{MessageType::System, "system",
                            requester_name + " 并未向您发送好友申请"},
                    client_fd);
          }
        } else {  // 申请方不存在
          unicast(Message{MessageType::System, "system",
                          "用户 " + requester_name + " 不存在"},
                  client_fd);
        }
      }
    }
  }

  // 关闭连接
  close(client_fd);
  // 通知用户离线
  Message leave_msg;
  leave_msg.type = MessageType::System;
  leave_msg.username = "system";
  std::string client_name = userManager.get_username(client_fd);
  leave_msg.msg =
      client_name.empty() ? "Jane Doe" : client_name + " 离开聊天室";
  broadcast(leave_msg, client_fd);
  std::cout << "用户离线：" << userManager.get_username(client_fd) << std::endl;
  // 用户表清除记录
  userManager.remove_user(client_fd);
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
    // 6. 为该客户端创建线程
    std::thread t(handle_client, client_fd);
    // detach: 线程独立运行，不需要 join
    t.detach();
  }
  return 0;
}
