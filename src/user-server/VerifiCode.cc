#include"UserServer.h"
#include<curl/curl.h>

/* 读取发送数据 */
size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp) {
  std::string *payload = static_cast<std::string*>(userp);
  if (payload->empty()) return 0;
  
  size_t copy_size = std::min(size * nmemb, payload->size());
  memcpy(ptr, payload->data(), copy_size);
  
  *payload = payload->substr(copy_size);
  
  return copy_size;
}

/* 验证码生成 */
std::string UserServer::GenerateVerifiCode() {
  std::string code;
  code.reserve(6);
  srand((size_t)time(NULL));
  for(int i = 0;i<6;i++) {
      code.push_back((rand() % 10) + '0');
  }
  return code;
}

/* 发送验证码 */
void UserServer::SendCodeToEmall(const std::string & code,const std::string & email) {
  CURLcode res = CURLE_OK;
  CURL * curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // 打印通信日志
 
  if(curl) {
      curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.qq.com:465");
      curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
      curl_easy_setopt(curl, CURLOPT_USERNAME, "323602912@qq.com");
      curl_easy_setopt(curl, CURLOPT_PASSWORD, "wfpmagfsbhfqcaie");
      curl_easy_setopt(curl, CURLOPT_LOGIN_OPTIONS, "AUTH=LOGIN");

      const std::string from = "From: <323602912@qq.com>\r\n";
      const std::string to = "To: <"+ email +">\r\n";
      const std::string type = "Content-Type: text/html;\r\n";
      const std::string subject = "Subject: Chatroom Verification Code\r\n";
      const std::string body = "使用以下验证码以完成注册：<p style=\"font-size: 48px; color:rgb(0, 0, 0);\">" + 
      code +"</p>\r\n";

      struct curl_slist* headers = NULL;
      headers = curl_slist_append(headers, from.c_str());
      headers = curl_slist_append(headers, to.c_str());
      std::string payload_text = from + to + type + subject + "\r\n\r\n" + body;

      curl_easy_setopt(curl, CURLOPT_MAIL_FROM, "323602912@qq.com");
      
      struct curl_slist* recipients = NULL;
      recipients = curl_slist_append(recipients, email.c_str());
      curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

      curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
      curl_easy_setopt(curl, CURLOPT_READDATA, &payload_text);
      curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

      res = curl_easy_perform(curl);

      if (res != CURLE_OK) {
          std::cerr << "错误: " << curl_easy_strerror(res) <<std::endl;
      }

      // free
      curl_slist_free_all(recipients);
      curl_slist_free_all(headers);
      curl_easy_cleanup(curl);
  }
}