# 登陆注册

1. 注册倒数第二个输入框对Enter响应问题

2. 登陆之后 FriendList 无好友显示问题

<!-- 3. 注销账号之后再次尝试登陆会使服务器崩溃

LOG INFO
```
[2025-07-08 10:32:46] [INFO] Server: receive a message from 10.30.0.131
[2025-07-08 10:32:46] [INFO] Client 10.30.0.131:48642 sent
{
        "action" : "LOGIN",
        "email" : "323602912@qq.com",
        "passwd" : "1"
}
terminate called after throwing an instance of 'cpp_redis::redis_error'
  what():  Reply is not a string
fish: Job 1, './server' terminated by signal SIGABRT (Abort)
```

(似乎只要登陆一个不存在的账号就会使服务器崩溃) -->

4. 验证码发送无cd 可以一直发送

5. userServer.cc 168行 缺少 redis 的 `sync_commit()` 方法调用

6. 第一次注销永远不成功

# 好友管理

# 文件系统

1. 从远程传文件时 无法上传到服务器 但是从本地可以 (好像是因为FtpServer 的 服务器ip没有修改)