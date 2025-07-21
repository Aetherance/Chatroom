#include"threadpool.h"

threadpool::threadpool(size_t threads) {
    for(size_t i = 0; i<threads; i++) {
        workers.emplace_back([this](){
            while (true) {
                std::function<void()> task;

                {
                    std::unique_lock lock(this->mtx);
                    this->cv.wait(lock,[&]{return stop || !task_queue.empty();});
                    
                    if(this->stop && this->task_queue.empty()) {
                        return;
                    }

                    task = std::move(this->task_queue.front());
                    this->task_queue.pop();
                }

                task();
            }
            
        });
    }
}

threadpool::~threadpool() {
    {
        std::unique_lock lock(mtx);
        stop = true;
    }

    cv.notify_all();

    for(auto &thread : workers) {
        thread.join();
    }
}