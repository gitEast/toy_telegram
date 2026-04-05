# Toy Telegram

> 用 C++ 实现一个玩具版的 Telegram（并不保证功能的完整性和可靠性）

## 版本追踪

| Version | Feature                             |
| ------- | ----------------------------------- |
| 01      | 广播                                |
| 02      | 绑定用户名                          |
| 03      | 规范信息结构 Message & 处理信息边界 |
| 04      | 向指定客户端发送一条信息            |
| 05      | 好友功能 & 仅允许好友私聊           |

## command

| type             | command                                                   |
| ---------------- | --------------------------------------------------------- |
| broadcast        | `[content]`                                               |
| private          | `/msg [username] [content]`                               |
| add_friend       | `/fri_add [username]`                                     |
| add_friend_reply | accept: `/fri_ok [username]`; refuse: `fri_no [username]` |
