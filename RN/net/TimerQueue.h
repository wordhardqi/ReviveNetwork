//
// Created by rmqi on 8/7/18.
//

#ifndef RN_TIMERQUEUE_H
#define RN_TIMERQUEUE_H

#include <RN/base/Timestamp.h>
#include <bits/unique_ptr.h>
#include "Timer.h"
#include "EventLoop.h"
#include "Channel.h"
#include "TimerId.h"
#include <set>
#include <vector>

namespace RN {
    class TimerQueue {
    public:
        TimerQueue(EventLoop *loop);

        ~TimerQueue();

        TimerId addTimer(const TimerCallback &cb,
                         Timestamp when, double interval);

    private:
        typedef std::pair<Timestamp, TimerPtr> Entry;
        typedef std::set<Entry> TimerList;

        void handleRead();

        std::vector<Entry> getExpired(Timestamp now);

        void reset(const std::vector<Entry> &expired, Timestamp now);

        bool insert(TimerPtr timer);

        EventLoop *loop_;
        const int timerfd_;
        Channel timerfdChannel_;
        TimerList timers_;
    };
}

#endif //RN_TIMERQUEUE_H
