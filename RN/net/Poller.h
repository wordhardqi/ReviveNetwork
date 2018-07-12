//
// Created by rmqi on 8/7/18.
//

#ifndef RN_POLLER_H
#define RN_POLLER_H


#include <vector>
#include <RN/base/Timestamp.h>
#include <map>
#include "EventLoop.h"

namespace RN {
    class Channel;

    class Poller {
    public:
        typedef std::vector<Channel *> ChannelList;

        Poller(EventLoop *loop);

        ~Poller();

        Timestamp poll(int timeoutMs, ChannelList *activeChannels);

        void updateChannel(Channel *channel);

        void removeChannel(Channel *channel);

        void assertInLoopThread() {
            ownerLoop_->assertInLoopThread();
        }


    private:
        void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;

        typedef std::vector<struct pollfd> PollFdList;
        typedef std::map<int, Channel *> ChannelMap;
        EventLoop *ownerLoop_;
        PollFdList pollfds_;
        ChannelMap channels_;
    };
}


#endif //RN_POLLER_H
