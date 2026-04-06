#include "user_manager.h"

void UserManager::add_user(int fd, const std::string& username) {
  fd_to_user[fd] = username;
  user_to_fd[username] = fd;
}
void UserManager::remove_user(int fd) {
  if (fd_to_user.count(fd)) {
    std::string username = fd_to_user[fd];
    fd_to_user.erase(fd);
    user_to_fd.erase(username);
  }
}
std::string UserManager::get_username(int fd) const {
  auto it = fd_to_user.find(fd);
  if (it != fd_to_user.end()) return it->second;
  return "";
}
int UserManager::get_fd(const std::string& username) const {
  auto it = user_to_fd.find(username);
  if (it != user_to_fd.end()) return it->second;
  return INVALID_FD;
}
bool UserManager::user_exists(const std::string& username) const {
  return user_to_fd.count(username) > 0;
}
void UserManager::for_each(const std::function<void(int)>& func) const {
  for (const auto& [fd, username] : fd_to_user) {
    func(fd);
  }
}
void UserManager::for_each(
    const std::function<void(int, std::string)>& func) const {
  for (const auto& [fd, username] : fd_to_user) {
    func(fd, username);
  }
}

bool UserManager::are_friends(const std::string& a,
                              const std::string& b) const {
  auto it = friends.find(a);
  if (it != friends.end()) {
    return it->second.count(b) > 0;
  }
  return false;
}
void UserManager::add_friend(const std::string& a, const std::string& b) {
  friends[a].insert(b);
  friends[b].insert(a);
}

void UserManager::add_friend_request(const std::string& to,
                                     const std::string& from) {
  friend_requests[to].insert(from);
}
bool UserManager::has_friend_request(const std::string& to,
                                     const std::string& from) const {
  auto it = friend_requests.find(to);
  if (it != friend_requests.end()) {
    return it->second.count(from) > 0;
  }
  return false;
}
void UserManager::remove_friend_request(const std::string& to,
                                        const std::string& from) {
  auto it = friend_requests.find(to);
  if (it != friend_requests.end()) {
    it->second.erase(from);
  }
}
