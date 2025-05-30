#pragma once

#include<arpa/inet.h>

/* 登录注册服务器响应码 */

/* 登录/注册 请求成功 */
#define USER_OK                       2000

/* 注册 */
#define USERNAME_ALREADY_EXISTS       4001 // 用户名已存在
#define EMAIL_ALREADY_REGISTERED      4002 // 邮箱已存在
#define VERIFICATION_CODE_INVALID     4003 // 验证码错误

/* 登录 */
#define USER_NOT_FOUND                4011 // 用户不存在
#define PASSWORD_INCORRECT            4012 // 密码错误

/* 客户端请求 action 字段定义 */
#define REGISTER1    "REGISTER1"
#define REGISTER2    "REGISTER2"
#define LOGIN        "LOGIN"
#define CANCEL       "CANCEL"

inline void SendResponseCode(int code,int fd) {
  ::send(fd,&code,sizeof(int),0);
}

/* chat 服务类别 */

#define ADD_FRIEND   "addFriend"
#define DEL_FRIEND   "deleteFriend"
#define VER_FRIEND   "verifyFriend"
#define BLACK_OUT    "blackoutFriend"
#define BLOCK        "blockFriend"
#define UNBLOCK      "unblockFriend"
#define ADD_GROUP    "addGroup"
#define VERI_GROUP   "verifyGroup"
#define QUIT_GROUP   "quitGroup"
#define CREATE_GROUP "createGroup"
#define BREAK_GROUP  "breakGroup"
#define RM_GROUP_MEM "rmGroupMem"

/* Service 返回 */

#define ADDFRIEND_BACK              "ADDFRIEND_BACK"
#define ADD_FRIEND_SEND_SUCCESS     "ADD_FRIEND_SEND_SUCCESS"
#define ADD_FRIEND_SEND_FAILED      "USER_NOT_FOUND"
#define VERI_FRIEND_BACK            "VERI_FRIEND_BACK"
#define VERI_FRIEND_SUCCESS         "VERI_FRIEND_SUCCESS"
#define PULL_FRIEND_LIST            "PULL_FRIEND_LIST"
#define EMAIL_HASH_USERNAME         "emailHashUserName"
#define FRIEND_BE_ONLINE            "FriendBeOnline"
#define FRIEND_BE_OFFLINE           "FriendBeOffline"
#define DEL_FRIEND_SUCCESS          "DEL_FRIEND_SUCCESS"
#define PULL_GROUP_LIST             "PULL_GROUP_LIST"
#define ADD_GROUP_SEND_SUCCESS      "ADD_GROUP_SEND_SUCCESS"
#define ADD_GROUP_SEND_FAILED       "ADD_GROUP_SEND_FAILED"
#define ADDGROUP_BACK               "ADDGROUP_BACK"
#define VERI_GROUP_SUCCESS          "VERI_FRIEND_SUCCESS"
#define ENTERING_NEW_GROUP          "ENTERING_NEW_GROUP"
#define MEMBER_QUIT_GROUP           "MEMBER_QUIT_GROUP"
#define QUIT_GROUP_BACK             "QUIT_GROUP_BACK"
#define QUIT_GROUP_SUCCESS          "QUIT_GROUP_SUCCESS"
#define QUIT_GROUP_FAILED           "QUIT_GROUP_FAILED"
#define GROUP_BROKEN                "GROUP_BROKEN"  
#define PULL_GROUP_MEMBERS          "PULL_GROUP_MEMBERS"
#define SET_OP                      "SET_OP"
#define DE_OP                       "DE_OP"   
#define BLOCKED                     "BLOCKED"

/* 群成员标识 */

#define GROUP_OWNER    "A"
#define GROUP_OP       "B"
#define GROUP_MEMBER   "C"

#define UPLOAD_FILE    "UPLOAD_FILE"
#define DOWNLOAD_FILE  "DOWNLOAD_FILE"
#define FILE_NOT_FOUND "FILE_NOT_FOUND"

/* TCP心跳检测 */
#define HEARTBEAT_INTERVAL 30 // 心跳间隔时间，单位为秒
#define HEARTBEAT_TIMEOUT  60 // 心跳超时时间，单位为秒
#define HEARTBEAT_MSG "HEARTBEAT"
#define HEARTBEAT_MSG_LEN 9 // 心跳消息长度
#define HEARTBEAT_BACK_MSG "ACK_HEARTBEAT" // 心跳回复消息
#define HEARTBEAT_BACK_MSG_LEN 13 // 心跳回复消息长度