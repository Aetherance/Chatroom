#include"UserClient.h"
#include<ftxui/component/component.hpp>
#include<ftxui/dom/elements.hpp>
#include<ftxui/screen/screen.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include"UserClient.h"
#include"ResponseCode.h"

class Client
{
public:
  Client();
  ~Client();
  void run();
private:
  void Verify();
  void LoginController();
  bool LoginSubmit(const std::string & email,const std::string & passwd,const std::string & info);

  void RegisterController();
  // bool RegisterSubmit(const std::string & email,const std::string & passwd,const std::string & info);

  void work() {}

  UserClient userClient_;
};