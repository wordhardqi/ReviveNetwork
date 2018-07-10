//
// Created by rmqi on 8/7/18.
//

#ifndef RN_EVENTLOOP_H
#define RN_EVENTLOOP_H

#include <RN/base/noncopyable.h>
#include <RN/base/CurrentThread.h>
#include <sys/types.h>
#include <boost/scoped_ptr.hpp>
#include <vector>
#include "TimerQueue.h"

namespace RN {
    class Channel;

    class Poller;

    class TimerId;

    class TimerQueue;

    class EventLoop : RN::noncopyable {
    public:
        EventLoop();

        ~EventLoop();

        void loop();

        void quit();

        //Todo:: Simplify the owning structure.
        void updateChannel(Channel *channel);

        void assertInLoopThread() {
            if (threadId_ == RN::CurrentThread::tid()) {
                return;
            } else {
                abortNotInLoopThread();
            }
        }

        bool isInLoopThread() const {
            return threadId_ == RN::CurrentThread::tid();
        }

        Timestamp pollReturnTime() {
            return pollReturnTime_;
        }

        TimerId runAt(const Timestamp &time, const TimerCallback);

        TimerId runAfter(double delay, const TimerCallback);

        TimerId runEvery(double interval, const TimerCallback);

    private:
        typedef std::vector<Channel *> ChannelList;

        void abortNotInLoopThread();

        bool looping_;
        bool quit_;
        const pid_t threadId_;
        boost::scoped_ptr<Poller> poller_;
        Timestamp pollReturnTime_;
        std::unique_ptr<TimerQueue> timerQueue_;
        ChannelList activeChannels_;
    };

}


#endif //RN_EVENTLOOP_H
