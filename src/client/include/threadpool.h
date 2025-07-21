#include<iostream>
#include<vector>
#include<thread>
#include<functional>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<queue>

class threadpool {
public:
    threadpool(size_t);
    ~threadpool();

    template<class F,class ... Args>
    void enqueue(F && f,Args && ... args);
private:
    std::vector<std::thread>workers;
    std::mutex mtx;
    std::condition_variable cv;
    std::queue <std::function<void()>>task_queue;

    bool stop = false;
};

template<class F,class ... Args>
void threadpool::enqueue(F && f,Args && ... args) {
    auto task = std::bind(std::forward<F>(f),std::forward<Args>(args)...); // bind无法实现完美转发！！

    {
        std::unique_lock lock(mtx);

        task_queue.emplace([task](){ task(); });
    }

    cv.notify_one();
}