#ifndef FRAMER_H
#define FRAMER_H

#include <string>

class Framer {
 public:
  /**
   * @brief 往缓冲区追加数据
   */
  void append(const char* data, size_t len);

  /**
   * @brief 是否存在完整信息
   */
  bool has_message() const;

  /**
   * @brief 取出一条完整消息（不含 \n，并在取出后去除）
   */
  std::string next_message();

 private:
  // 内部缓存
  std::string buffer;
};

#endif
