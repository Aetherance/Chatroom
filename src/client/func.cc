#include"user.h"
#include<jsoncpp/json/json.h>

void UserClient::SerializeSend(const std::string & action,const std::string & Requestor,const std::string & obj,const std::vector<std::string>& args) {
  Json::Value root;
  root["action"] = action;
  root["requestor"] = Requestor;
  root["object"] = obj;
  if(!args.empty()) {
    for(auto & element : args) {
      root["args"].append(element);
    }
  }

  Json::StreamWriterBuilder writer;
  std::string requestion = Json::writeString(writer,root);

  Send(requestion);
}

void UserClient::addFriend() {
  
}