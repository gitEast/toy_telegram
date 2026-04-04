#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

/* 消息结构 */
struct Message {
  std::string type;
  std::string username;
  std::string msg;
};

/**
 * @brief 序列化（字符串拼接
 */
std::string serialize(const Message& m);

/**
 * @brief 反序列化
 */
Message deserialize(const std::string& str);

#endif
