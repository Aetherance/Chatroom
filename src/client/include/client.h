#include<ftxui/component/component.hpp>
#include<ftxui/dom/elements.hpp>
#include<ftxui/screen/screen.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include"user.h"
#include"responsecode.h"

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

  /* ... */
  void Msg();

  /* 登录注册客户端 */
  UserClient userClient_;

  /* UI界面 */
  ftxui::ScreenInteractive mainScreen_;
  ftxui::ScreenInteractive loginScreen_;
  ftxui::ScreenInteractive registerScreen_;
};