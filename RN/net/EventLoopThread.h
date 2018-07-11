//
// Created by rmqi on 10/7/18.
//

#ifndef RN_EVENTLOOPTHREAD_H
#define RN_EVENTLOOPTHREAD_H

#include <RN/base/Thread.h>
#include "EventLoop.h"

namespace RN {
    class EventLoopThread {
    public:
        EventLoopThread();

        ~EventLoopThread();

        EventLoop *startLoop();

    private:
        void threadFunc();

        EventLoop *loop_;
        bool exiting_;
        RN::Thread thread_;
        MutexLock mutex_;
        Condition cond_;
    };
}

#endif //RN_EVENTLOOPTHREAD_H
