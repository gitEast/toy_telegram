#include "message.h"

std::string serialize(const Message& m) {
  std::string result = "{";
  result += "\"type\":\"" + m.type + "\"";
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
  m.type = extract(str, "type");
  m.username = extract(str, "username");
  m.msg = extract(str, "msg");
  return m;
}
