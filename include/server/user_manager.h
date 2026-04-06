#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include <map>
#include <set>
#include <string>

// TODO: 需要加锁

class UserManager {
 public:
  static constexpr int INVALID_FD = -1;
  /* ------------ ⬇ 用户数据 ⬇ ------------ */
  // 用户上线
  void add_user(int fd, const std::string& username);
  // 用户离线
  void remove_user(int fd);
  // 获取用户名
  std::string get_username(int fd) const;
  // 根据用户名找 fd
  int get_fd(const std::string& username) const;
  // 判断用户是否存在
  bool user_exists(const std::string& username) const;

  void for_each(const std::function<void(int)>& func) const;
  void for_each(const std::function<void(int, std::string)>& func) const;
  /* ------------ ⬆ 用户数据 ⬆ ------------ */

  /* ------------ ⬇ 好友关系 ⬇ ------------ */
  // 是否存在好友关系
  bool are_friends(const std::string& a, const std::string& b) const;
  // 新增好友关系
  void add_friend(const std::string& a, const std::string& b);
  /* ------------ ⬆ 好友关系 ⬆ ------------ */

  /* ------------ ⬇ 好友申请 ⬇ ------------ */
  // 新增好友申请
  void add_friend_request(const std::string& to, const std::string& from);
  // 是否存在好友申请
  bool has_friend_request(const std::string& to, const std::string& from) const;
  // 消除好友申请
  void remove_friend_request(const std::string& to, const std::string& from);
  /* ------------ ⬆ 好友申请 ⬆ ------------ */

 private:
  std::map<int, std::string> fd_to_user;
  std::map<std::string, int> user_to_fd;

  std::map<std::string, std::set<std::string>> friends;          // 好友关系
  std::map<std::string, std::set<std::string>> friend_requests;  // 好友申请
};

#endif
