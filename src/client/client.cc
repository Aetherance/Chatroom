#include"client.h"
#include<iostream>
#include<regex>
#include<termios.h>
#include<chrono>
#include<iomanip>
#include<fstream>

#define HISTORY_MESSAGE_FILE ".data"

using namespace ftxui;

extern std::unordered_map<std::string,std::vector<messageinfo>> messageMap;

extern std::vector<Group> groups;

extern ScreenInteractive MsgScreen;
 
extern std::unordered_map<std::string,bool> NewMessageMap;

std::unordered_set<std::string> commandSet = { 
    "/break" ,
    "/op" ,
    "/deop" ,
    "/rm" ,
    "/block" ,
    "/unblock" ,
    "/upload",
    "/download",
    "/t"
  };

/* 构造函数 : 初始化UI界面 */
Client::Client() : loginScreen_(ScreenInteractive::Fullscreen()),
                   registerScreen_(ScreenInteractive::Fullscreen()),
                   mainScreen_(ScreenInteractive::Fullscreen()),
                   msgClient_(ftpClient_)
{}

Client::~Client() 
{}

/* 运行客户端程序 */
void Client::run() {
  /* 身份验证 */
  Verify();

  if(isExit) {
    return;
  }

  /* 聊天室聊天功能 */
  readMessage();

  Msg();

  storageMessage();
}

void Client::Verify() {
  userClient_.Connect();
  
  /* 登录回调 */
  Component login_button = Button("登录",[&]{ LoginController(); mainScreen_.Exit(); });
  /* 注册回调 */
  Component register_button = Button("注册",[&]{ RegisterController(); mainScreen_.Exit(); });
  /* 退出 */
  Component exit_button = Button("退出",[&]{ mainScreen_.Exit(); isExit = true;});

  Component container;

  container = Container::Vertical({
    login_button,
    register_button,
    exit_button
  });

  auto renderer = Renderer(container, [&] {
    return vbox({
      text("▄▖▌   ▗          "),
      text("▌ ▛▌▀▌▜▘▛▘▛▌▛▌▛▛▌"),
      text("▙▖▌▌█▌▐▖▌ ▙▌▙▌▌▌▌"),
      text(""),
      login_button->Render(),
      register_button->Render(),
      exit_button->Render() | bold
    }) | (size(WIDTH, EQUAL, 30) | size(HEIGHT, EQUAL, 20)) | center;
  }) | CatchEvent([&](Event event){
    if(event == Event::CtrlC) {
      return true;
    } else {
      return false;
    }
  });

  while(!userClient_.hasLogin() && !isExit) {
    mainScreen_.Loop(renderer);
  }
}

bool isValidEmail(const std::string& email) {
  const std::regex pattern(
      R"(^(?!.*[.]{2})[a-zA-Z0-9._%+-]+@(?:[a-zA-Z0-9-]+\.)+[a-zA-Z]{2,}$)"
  );
  return std::regex_match(email, pattern);
}

void Client::Msg() { 
  std::thread recvThread([this]{ msgClient_.recvMsgLoop(); });
  
  msgClient_.setEmail(localUserEmail_);
  
  msgClient_.connect();

  msgClient_.pullFriendList();

  msgClient_.pullGroupList();

  FriendList();

  msgClient_.ExitLoop();

  recvThread.join();

  flush_terminal_input();
}

std::vector<std::string> split(const std::string s,char ch)
{
  std::vector<std::string>result;
  int pos = 0;
  while (s[pos]==ch) {
    pos++;
  }
    
  while (pos< s.size()) {
    int n = 0;
    while (s[pos+n]!=ch&&pos+n<s.size()) {
      n++;
    }
    result.push_back(s.substr(pos,n));
    pos += n;
    while (s[pos] ==ch&&pos<s.size()) {
      pos++;
    }
  }
  return result;
}

bool Client::parseCommand(std::string & input) {
  if(input[input.size() - 1] == '\n' && input.size() != 1) {
    input.resize(input.size() - 1);
  }
  
  const std::vector<std::string> cmds = split(input,' ');
  
  if(commandSet.find(cmds[0]) == commandSet.end()) {
    return false;
  }

  input.clear();
  
  if(cmds[0] == "/break" && msgClient_.isPeerGroup()) {
    messageMap[msgClient_.peerEmail()].clear();
    msgClient_.breakGroup(msgClient_.LocalEmail(),msgClient_.peerEmail());
    MsgScreen.Exit();
    msgClient_.pullGroupList();
    return true;
  } else if(cmds[0] == "/op" && msgClient_.isPeerGroup() && cmds.size() > 1) {
    msgClient_.setOP(cmds[1],msgClient_.peerEmail());
    pullGroupMembers();
  } else if(cmds[0] == "/deop" && msgClient_.isPeerGroup() && cmds.size() > 1) {
    msgClient_.deOP(cmds[1],msgClient_.peerEmail());
    pullGroupMembers();
  } else if(cmds[0] == "/rm" && msgClient_.isPeerGroup() && cmds.size() > 1) {
    msgClient_.rmGroupMember(cmds[1],msgClient_.peerEmail());
  } else if(cmds[0] == "/block") {
    msgClient_.blockFriend(msgClient_.LocalEmail(),msgClient_.peerEmail());
    messageMap[msgClient_.peerEmail()].push_back({"系统","来自 " + msgClient_.peerUsername() + " 的消息已屏蔽!",ilib::base::Timestamp::now().microSecondsSinceEpoch()});
    MsgScreen.PostEvent(Event::Custom);
  } else if(cmds[0] == "/unblock") {
    msgClient_.unBlock(msgClient_.LocalEmail(),msgClient_.peerEmail());
    messageMap[msgClient_.peerEmail()].push_back({"系统","来自 " + msgClient_.peerUsername() + " 的消息已取消屏蔽!",ilib::base::Timestamp::now().microSecondsSinceEpoch()});
    MsgScreen.PostEvent(Event::Custom);
  } else if(cmds[0] == "/upload" && cmds.size() > 2) {
    sendFileTo(msgClient_.peerEmail(), cmds[2], cmds[1]);
    messageMap[msgClient_.peerEmail()].push_back({"系统","文件已上传",ilib::base::Timestamp::now().microSecondsSinceEpoch()});
    MsgScreen.PostEvent(Event::Custom);
  } else if(cmds[0] == "/download" && cmds.size() > 2) {
    downloadFile(cmds[1],cmds[2]);
  } else if(cmds[0] == "/t") {
    MsgScreen.Exit();
    mainScreen_.Exit();
    tMode();
  }
  
  return true;
}

void Client::flush_terminal_input() {
  struct TermiosGuard { // RAII自动恢复终端设置
    termios original;
    TermiosGuard() { tcgetattr(STDIN_FILENO, &original); }
    ~TermiosGuard() { tcsetattr(STDIN_FILENO, TCSANOW, &original); }
} guard;

termios settings = guard.original;
settings.c_lflag &= ~(ICANON | ECHO);
settings.c_cc[VMIN] = 0;
settings.c_cc[VTIME] = 1;

tcsetattr(STDIN_FILENO, TCSANOW, &settings);

std::string buf;
  for (char c; read(STDIN_FILENO, &c, 1) == 1;) {
    if (c == '\n') break;
  }
}

void Client::readMessage() {
  std::ifstream file("." + localUserEmail_ + HISTORY_MESSAGE_FILE,std::ios::binary);
  int message_len;
  while(file.peek() != EOF) {
    std::string buff;
    file.read(reinterpret_cast<char*>(&message_len),sizeof(message_len));
    if(file.eof()) {
      break;
    }
    buff.resize(message_len);
    file.read(buff.data(),message_len);
    Message history_msg;
    history_msg.ParseFromString(buff);
    if(history_msg.from() == "You") {
      history_msg.set_from(history_msg.to());
      msgClient_.enMapYouMessage(history_msg);
    } else {
      msgClient_.parseMsg(buff);
    }
  }
  for(auto & entry : messageMap) {
    entry.second.push_back({"系统","以上为历史消息",ilib::base::Timestamp::now().microSecondsSinceEpoch()});
    NewMessageMap[entry.first] = false;
  }
}

void Client::storageMessage() {
  std::ofstream file("." + localUserEmail_ + HISTORY_MESSAGE_FILE,std::ios::binary);
  Message histroy_message;
  if (file.is_open()) {
    for(auto & pair : messageMap) {
      for(auto & entry : pair.second) {
        histroy_message.set_from(entry.from);
        histroy_message.set_text(entry.text);
        histroy_message.set_timestamp(entry.timestamp);
        if(histroy_message.from() == "You") {
          histroy_message.set_to(pair.first);
        }
        std::string perMessage = histroy_message.SerializeAsString();
        int messageLen = perMessage.size();
        file.write((char*)&messageLen,sizeof(messageLen));
        file.write(perMessage.data(),perMessage.size());
      }
    }
  }
}

void Client::pullGroupMembers() {
  for(auto & entry : groups) {
    msgClient_.pullGroupMembers(false,entry.groupname);
  }
}

void Client::sendFileTo(const std::string & who, const std::string fileName, const std::string & filePath) {
  if(!std::filesystem::exists(filePath)) {
    return;
  }
  
  ftpClient_.connect();
  ftpClient_.uploadFile(filePath,who,fileName);

  std::string toUser = std::filesystem::path(who).parent_path();

  Message msg;
  msg.set_text(UPLOAD_FILE);
  msg.set_isservice(true);
  msg.set_from(msgClient_.LocalEmail());
  msg.set_to(fileName);
  msg.set_timestamp(ilib::base::Timestamp::now().microSecondsSinceEpoch());
  msg.add_args(toUser);

  msgClient_.safeSend(msg.SerializeAsString());
}

void Client::downloadFile(const std::string & filename,const std::string & localDir) {
  ftpClient_.connect();
  ftpClient_.downloadFile(localDir, msgClient_.LocalEmail(), filename);

  messageMap[msgClient_.peerEmail()].push_back({"系统","已将文件下载至" + localDir + "/" + filename,ilib::Timestamp::now().microSecondsSinceEpoch()});
}

void Client::tMode() {
  std::string inputBuff;
  while (inputBuff != "q") {
    printf("输入q退出");
    printf("%s: ",msgClient_.LocalUsername().data());
    std::cin>>inputBuff;

    msgClient_.sendMsgPeer(inputBuff);  
  }
}