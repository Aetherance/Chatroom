#ifndef TIMER_H
#define TIMER_H

#include"../base/noncopyable.h"
#include"../base/Timestamp.h"
#include<functional>
#include<atomic>

namespace ilib{
using base::Timestamp;
namespace net{

class Timer : noncopyable 
{
public:
    using TimerCallback = std::function<void()>;
    Timer(TimerCallback cb,Timestamp when,double interval);
    void run() const;
    Timestamp expiration() const;
    bool repeat() const;
    int64_t sequence() const;

    void restart(Timestamp now);

private:
    const TimerCallback callback_;
    Timestamp expiration_;
    const double interval_;
    const bool repeat_;

    const int64_t sequeuece_;
    static std::atomic<int64_t> s_numCreated;
};

inline std::atomic<int64_t> Timer::s_numCreated = 0; 

inline Timer::Timer(TimerCallback cb,Timestamp when,double interval) :
     callback_(cb) , 
     expiration_(when), 
     interval_(interval),
     repeat_(interval > 0),
     sequeuece_(++ s_numCreated) {}

inline void Timer::run() const {
    callback_();
}

inline Timestamp Timer::expiration() const {
    return expiration_;
}

inline bool Timer::repeat() const {
    return repeat_;
}

inline void Timer::restart(Timestamp now) {
    if(repeat_) {
        expiration_ = Timestamp::addTime(now,interval_);
    } else {
        expiration_ = Timestamp(-1);
    }
}

inline int64_t Timer::sequence() const {
    return sequeuece_;
}

}
}

#endif