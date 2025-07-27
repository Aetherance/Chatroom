#include "client.h"

using namespace ftxui;

ScreenInteractive FileScreen = ScreenInteractive::Fullscreen();

std::string status = {};

std::vector<std::string> downloadable_files;
std::vector<std::string> uploadable_files;

std::vector<Component> download_buttons;
std::vector<Component> upload_buttons;

std::string Client::generateUniqueFilename(const std::string& originalName) {
    std::string filename = originalName;
    int counter = 1;
    
    // 检查文件名是否已存在
    while (ftpClient_.transList.find(filename) != ftpClient_.transList.end()) {
        // 尝试移除可能已有的数字后缀
        size_t lastDot = filename.find_last_of('.');
        if (lastDot != std::string::npos && 
            std::all_of(filename.begin() + lastDot + 1, filename.end(), ::isdigit)) {
            filename = filename.substr(0, lastDot);
        }
        
        // 添加新的数字后缀
        filename = originalName + "." + std::to_string(counter);
        counter++;
    }
    
    return filename;
}

void Client::fileService() {
  // 扫描上传目录
  auto scanUploadDir = [&] {
    uploadable_files.clear();
    upload_buttons.clear();
    std::string uploadDir = "./upload/";
    
    if (std::filesystem::exists(uploadDir)) {
      for (const auto & entry : std::filesystem::directory_iterator(uploadDir)) {
        if (!std::filesystem::is_directory(entry)) {
          uploadable_files.push_back(entry.path().filename().string());
        }
      }
    }
  };
  
  // 初始化扫描
  scanUploadDir();
  msgClient_.pullDownloadList(msgClient_.LocalEmail(), msgClient_.peerEmail());

  // 创建上传按钮
  for (const auto& filename : uploadable_files) {
    upload_buttons.push_back(
      Button(filename, [&, filename] {
        std::string upload_path = "./upload/" + filename;
        
        if(!std::filesystem::exists(upload_path)) {
          status = "文件不存在!";
          return ;
        }

        if(ftpClient_.transList.find(filename) != ftpClient_.transList.end()) {
          fileTrans();
          return;
        }
        
        std::thread([&, upload_path, filename]{ 
          ftpClient_.transList.insert({filename, "正在传输"});
          ftpClient_.transProgressMap[filename] = 0.0f;

          if(msgClient_.isPeerGroup()) {
            sendFileTo(msgClient_.peerEmail(), filename, upload_path);       
          } else {
            sendFileTo(msgClient_.peerEmail() + "/" + msgClient_.LocalEmail(), filename, upload_path);
          }
        }).detach();
        
        fileTrans();
      })
    );
  }

  // 创建下载按钮
  for (size_t i = 0; i < downloadable_files.size(); ++i) {
    download_buttons.push_back(
      Button(downloadable_files[i], [&,i] {
        std::thread([&]{
          std::string filename_download = downloadable_files[i];

          if(ftpClient_.transList.find(filename_download) != ftpClient_.transList.end()) {
            return;
          }

          ftpClient_.transList.insert({filename_download, "正在传输"});
          ftpClient_.transProgressMap[filename_download] = 0.0f;

          std::filesystem::path savePath = "./download";
          std::filesystem::create_directory(savePath);
          if(msgClient_.isPeerGroup()) {
            ftpClient_.downloadFile(savePath, msgClient_.peerEmail(), downloadable_files[i]);
          } else {
            ftpClient_.downloadFile(savePath, msgClient_.LocalEmail() + "/" + msgClient_.peerEmail(), downloadable_files[i]);
          }
        }).detach();

        fileTrans();
      })
    );
  }

  // 刷新按钮
  auto reload_button = Button("刷新", [&] {
    // 刷新时重新扫描上传目录
    scanUploadDir();
    msgClient_.pullDownloadList(msgClient_.LocalEmail(), msgClient_.peerEmail());
  });

  // 传输列表按钮
  auto transfer_list_button = Button(" 传输列表 ", [&]{ fileTrans(); });

  // 创建按钮容器
  auto upload_container = Container::Vertical(upload_buttons);
  auto download_container = Container::Vertical(download_buttons);

  // 组合所有组件 - 使用水平布局
  auto file_columns = Container::Horizontal({
    // 左边上传列
    upload_container,
    // 右边下载列
    download_container
  });

  // 底部按钮容器
  auto bottom_buttons = Container::Horizontal({
    reload_button,
    transfer_list_button
  });

  // 整体垂直布局
  auto main_container = Container::Vertical({
    file_columns,
    bottom_buttons
  });

  // 渲染界面
  auto renderer = Renderer(main_container, [&] {
    return vbox({
      // 标题区域
      hbox({
        filler(),
        text("文件传输系统") | bold | center,
        filler(),
      }) | size(HEIGHT, EQUAL, 3),
      
      separator(),

      text(" " + status) | size(HEIGHT, EQUAL, status.empty() ? 0 : 1) | (status.empty() ? bold : border),
      
      // 左右两列文件区域
      hbox({
        // 左边上传列
        vbox({
          text("上传文件") | bold | center,
          separator(),
          upload_container->Render() | flex | frame | border | size(WIDTH, GREATER_THAN, 30)
        }) | flex,
        
        // 右边下载列
        vbox({
          text("下载文件") | bold | center,
          separator(),
          download_container->Render() | flex | frame | border | size(WIDTH, GREATER_THAN, 30)
        }) | flex,
      }) | flex,
      
      vbox({
        text("请将需要上传的文件放入 /tmp/upload/ 目录") | color(Color::GrayDark) | center,
        text("刷新后即可看到上传文件按钮") | color(Color::GrayDark) | center,
      }),
      
      separator(),

      // 底部按钮区域
      hbox({
        filler(),
        bottom_buttons->Render(),
        filler()
      }) | size(HEIGHT, EQUAL, 3)
    }) | flex | border | bold;
  }) | color(Color::White) | bgcolor(Color::RGB(22, 22, 30));

  FileScreen.Loop(renderer);
}