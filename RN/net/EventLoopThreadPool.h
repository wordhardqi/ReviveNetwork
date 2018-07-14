//
// Created by rmqi on 13/7/18.
//

#ifndef RN_EVENTLOOPTHREADPOOL_H
#define RN_EVENTLOOPTHREADPOOL_H

#include <vector>
#include <RN/base/Thread.h>
#include <memory>

namespace RN {
    class EventLoop;

    class EventLoopThread;

    class EventLoopThreadPool {
    public:
        EventLoopThreadPool(EventLoop *baseLoop, int numOfThreads = 0);

        ~EventLoopThreadPool();

        void start();

        bool isStarted() {
            return started_;
        }

        void setNumOfThreads(int n) {
            numOfThreads_ = n;
        }

        EventLoop *getNextLoop();

    private:
        EventLoop *baseLoop_;
        bool started_;
        int numOfThreads_;
        int next_;
        std::vector<std::unique_ptr<EventLoopThread>> loopThreads_;
        std::vector<std::unique_ptr<EventLoop>> loops_;
    };
}

#endif //RN_EVENTLOOPTHREADPOOL_H
