#include "framer.h"

void Framer::append(const char* data, size_t len) { buffer.append(data, len); }

bool Framer::has_message() const {
  return buffer.find('\n') != std::string::npos;
}

std::string Framer::next_message() {
  size_t pos = buffer.find('\n');
  // 没有完整信息
  if (pos == std::string::npos) return "";
  // 提取一条信息（不含 \n）
  std::string msg = buffer.substr(0, pos);
  // 从 buffer 删除 msg 和 \n
  buffer.erase(0, pos + 1);
  return msg;
}
