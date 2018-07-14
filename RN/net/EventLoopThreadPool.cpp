//
// Created by rmqi on 13/7/18.
//

#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include "EventLoopThread.h"

RN::EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop, int numOfThreads)
        : baseLoop_(baseLoop), started_(false), numOfThreads_(numOfThreads), next_(0) {

}

RN::EventLoopThreadPool::~EventLoopThreadPool() {

}

void RN::EventLoopThreadPool::start() {
    baseLoop_->assertInLoopThread();
    assert(!started_);
    started_ = true;
    for (int i = 0; i < numOfThreads_; i++) {
        std::unique_ptr<EventLoopThread> ioThread(new EventLoopThread);
        std::unique_ptr<EventLoop> ioLoop(ioThread->startLoop());
        loops_.push_back(std::move(ioLoop));
        loopThreads_.push_back(std::move(ioThread));
    }


}

RN::EventLoop *RN::EventLoopThreadPool::getNextLoop() {
    baseLoop_->assertInLoopThread();
    EventLoop *loop = baseLoop_;
    if (!loops_.empty()) {
        loop = loops_[next_].get();
        ++next_;
        if (static_cast<size_t >(next_) >= loops_.size()) {
            next_ = 0;
        }

    }
    return loop;
}
