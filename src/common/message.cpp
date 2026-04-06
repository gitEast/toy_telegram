#include "message.h"

std::string message_type_to_string(MessageType type) {
  switch (type) {
    case MessageType::Login:
      return "login";
    case MessageType::Broadcast:
      return "broadcast";
    case MessageType::Private:
      return "private";
    case MessageType::System:
      return "system";
    case MessageType::AddFriend:
      return "add_friend";
    case MessageType::AddFriendReply:
      return "add_friend_reply";
    default:
      return "unknown";
  }
}
MessageType string_to_message_type(const std::string& str) {
  if (str == "login") return MessageType::Login;
  if (str == "broadcast") return MessageType::Broadcast;
  if (str == "private") return MessageType::Private;
  if (str == "system") return MessageType::System;
  if (str == "add_friend") return MessageType::AddFriend;
  if (str == "add_friend_reply") return MessageType::AddFriendReply;
  return MessageType::Unknown;
}

std::string serialize(const Message& m) {
  std::string result = "{";
  result += "\"type\":\"" + message_type_to_string(m.type) + "\"";
  if (!m.username.empty()) {
    result += ",\"username\":\"" + m.username + "\"";
  }
  if (!m.msg.empty()) {
    result += ",\"msg\":\"" + m.msg + "\"";
  }
  result += "}\n";  // \n 作为分隔符
  return result;
}

/**
 * @brief 提取序列化后的 key 字段
 * @return std::string value
 */
std::string extract(const std::string& str, const std::string& key) {
  // 找 key 的位置
  std::string pattern = "\"" + key + "\":\"";
  size_t start = str.find(pattern);
  if (start == std::string::npos) return "";
  start += pattern.length();
  size_t end = str.find("\"", start);
  if (end == std::string::npos) return "";
  return str.substr(start, end - start);
}
Message deserialize(const std::string& str) {
  Message m;
  m.type = string_to_message_type(extract(str, "type"));
  m.username = extract(str, "username");
  m.msg = extract(str, "msg");
  return m;
}
