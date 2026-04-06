#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

/** 消息类型  */
enum class MessageType {
  Login,
  Broadcast,
  Private,
  System,
  AddFriend,       // 发送好友申请
  AddFriendReply,  // 回复好友申请（同意/拒绝）
  Unknown
};

/* 消息结构 */
struct Message {
  MessageType type;
  std::string username;
  std::string msg;
};

/**
 * @brief MessageType 转换成对应字符串
 */
std::string message_type_to_string(MessageType type);
/**
 * @brief 字符串转换成对应 MessageType 枚举
 */
MessageType string_to_message_type(const std::string& str);

/**
 * @brief 序列化（字符串拼接
 */
std::string serialize(const Message& m);

/**
 * @brief 反序列化
 */
Message deserialize(const std::string& str);

#endif
