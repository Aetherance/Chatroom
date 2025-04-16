#ifndef POLLER_H
#define POLLER_H

#include"../base/noncopyable.h"
#include"../base/Timestamp.h"
#include<vector>
#include<map>

struct pollfd;

namespace ilib {
namespace net {
class Channel;
class EventLoop;
class Poller : noncopyable
{
public:
    using ChannelList = std::vector<Channel *>;
    Poller(EventLoop* loop);
    ~Poller();
    base::Timestamp poll(int timeoutMs,ChannelList& activeChannels);

    void updateChannel(Channel* channel);
    void assertInLoopThread() { }
    void removeChannel(Channel * channel);
    
private:
    void fillActiveChannels(int numEvents,ChannelList& activeChannels) const;

    using PollFdList = std::vector<struct pollfd>;
    using ChannelMap = std::map<int,Channel*>;

    EventLoop* ownerLoop_;
    PollFdList pollfds_;
    ChannelMap channels_;
};

}
}

#endif