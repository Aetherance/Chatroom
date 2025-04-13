#ifndef TIMERID_H
#define TIMERID_H

#include<cstdint>

namespace ilib {
namespace net {

class Timer;
class TimerId
{
public:
    TimerId(Timer * timer);
    friend class TimerQueue;
private:
    Timer *timer_;
    int64_t seq_;
};

inline TimerId::TimerId(Timer * timer) :
timer_(timer)
{}

}    
}

#endif