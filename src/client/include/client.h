#pragma once

#include<ftxui/component/component.hpp>
#include<ftxui/dom/elements.hpp>
#include<ftxui/screen/screen.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include"responsecode.h"
#include"user.h"
#include"msg.h"
#include"ftp.h"

class Client
{
public:
  Client();
  ~Client();
  void run();
private:
  /* 登录注册 */
  void Verify();
  /* 登录控制器 */
  void LoginController();
  /* 提交登录信息 */
  bool LoginSubmit(const std::string & email,const std::string & passwd,const std::string & info);
  /* 注册控制器 */
  void RegisterController();

  /* Msg */
  void Msg();
  
 /* ftxui controllers */

  void FriendList();
  
  void MsgController();

  void VerifyFriend();

  void DeleteFriend();

  void GroupList();

  void CreateGroup();

  void GroupVerify();

  void DeleteGroup();

  void Settings();

  bool parseCommand(std::string & cmd);

  void flush_terminal_input();

  ftxui::Element makeSidebar();

  ftxui::Component makeRenderer();

  void storageMessage();

  void readMessage();

  void pullGroupMembers();

  void sendFileTo(const std::string & who,const std::string fileName,const std::string & filePath);

  void downloadFile(const std::string & filename,const std::string & localDir);

  void fileService();

  /* 登录注册客户端 */
  UserClient userClient_;

  /* 聊天客户端 */
  MsgClient msgClient_;

  /* 文件传输客户端 */
  FtpClient ftpClient_;

  /* UI界面 */
  ftxui::ScreenInteractive mainScreen_;
  ftxui::ScreenInteractive loginScreen_;
  ftxui::ScreenInteractive registerScreen_;

  bool isExit = false;

  std::string localUser_;
};