#include"Client.h"
#include<iostream>
#include<regex>

using namespace ftxui;


Client::Client() {

}

Client::~Client() {

}

void Client::run() {
  Verify();

}

void Client::Verify() {
  userClient_.Connect();
  // LoginController();
  RegisterController();
}

bool isValidEmail(const std::string& email) {
  const std::regex pattern(
      R"(^(?!.*[.]{2})[a-zA-Z0-9._%+-]+@(?:[a-zA-Z0-9-]+\.)+[a-zA-Z]{2,}$)"
  );
  return std::regex_match(email, pattern);
}