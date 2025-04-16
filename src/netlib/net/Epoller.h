#ifndef EPOLLPOLLER_H
#define EPOLLPOLLER_H

#include"../base/Timestamp.h"
#include"../base/noncopyable.h"
#include<vector>
#include<map>

struct epoll_event;
namespace ilib {
namespace net {

class Channel;
class EventLoop;
class Epoller : noncopyable
{
public:
    using ChannelList = std::vector<Channel *>;
    Epoller(EventLoop * loop);
    ~Epoller();
    base::Timestamp poll(int timeoutMs,ChannelList &activeChannels);

    void updateChannel(Channel * channel);
    void assertInLoopThread() {}
    void removeChannel(Channel * channel);
private:
    void fillActiveChannels(int numEvents,ChannelList& activeChannels) const;
    void update(int opt,Channel * channel);
    using EventList = std::vector<struct epoll_event>;
    using ChannelMap = std::map<int,Channel*>;

    EventLoop * ownerLoop_;
    int epollfd_;

    const int EventSize = 32;
    EventList events_;
    ChannelMap channels_;
};

}
}

#endif