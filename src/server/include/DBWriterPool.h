#ifndef MESSAGE_THREAD_POOL
#define MESSAGE_THREAD_POOL

#include<thread>
#include<vector>
#include<mutex>
#include<condition_variable>
#include<functional>
#include<string>
#include<queue>
#include<memory>
#include<cpp_redis/cpp_redis>

class DBWriterPool
{
public:
  DBWriterPool(int size = 12);
  ~DBWriterPool();

  void enqueue(const std::string & DBname, const std::string & message);

private:
  std::vector<std::thread>workers_;
  std::mutex mtx_;
  std::condition_variable cv;
  std::queue<std::pair<std::string,std::string>> messages_;
  bool isStop_ = false;
};

inline void DBWriterPool::enqueue(const std::string & DBname,const std::string & message) {
  {
    std::lock_guard lock(mtx_);
    messages_.push(std::pair<std::string,std::string>(DBname,message));
  }
  cv.notify_one();
}

inline DBWriterPool::DBWriterPool(int size) {
  for(int i = 0;i<size;i++) {
    workers_.emplace_back([this] {
        cpp_redis::client redis;
        redis.connect("127.0.0.1",6379);

        std::pair<std::string,std::string> entry;
        while (true) {
        {
          std::unique_lock<std::mutex> lock(mtx_);
          cv.wait(lock,[this]() { return isStop_ || !messages_.empty(); });
        
          if(isStop_) {
            redis.disconnect();
            return ;
          }
        
          entry = messages_.front();
          messages_.pop();
        }

        if(!redis.is_connected()) {
          redis.connect("127.0.0.1",6379);
        }

        redis.sadd(entry.first,{entry.second});
        redis.sync_commit();
      }
    });
  }
}

inline DBWriterPool::~DBWriterPool() {
  isStop_ = true;
  cv.notify_all();
  for(auto & thread : workers_) {
    thread.join();
  }
}

#endif