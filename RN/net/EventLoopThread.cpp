//
// Created by rmqi on 10/7/18.
//

#include "EventLoopThread.h"

RN::EventLoopThread::EventLoopThread()
        : loop_(NULL),
          exiting_(false),
          thread_(std::bind(&EventLoopThread::threadFunc, this)),
          mutex_(), cond_(mutex_) {

}

RN::EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    loop_->quit();
    thread_.join();
}

RN::EventLoop *RN::EventLoopThread::startLoop() {
    assert(!thread_.started());
    thread_.start();
    {
        MutexLock lock(mutex_);
        while (loop_ == NULL) {
            cond_.wait();
        }
    }
    return loop_;
}

void RN::EventLoopThread::threadFunc() {
    EventLoop loop;
    {
        MutexLock lock(mutex_);
        loop_ = &loop;
        cond_.notify();
    }
    loop.loop();

}
