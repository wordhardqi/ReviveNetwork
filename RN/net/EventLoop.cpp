//
// Created by rmqi on 8/7/18.
//

#include <RN/base/Logging.h>
#include "EventLoop.h"
#include "Poller.h"

using namespace RN;
__thread EventLoop *t_loopInThisThread = 0;
const int kPollTimeMs = 10000;

RN::EventLoop::EventLoop()
        : looping_(false), quit_(false),
          threadId_(RN::CurrentThread::tid()),
          poller_(new Poller(this)), pollReturnTime_(0),
          timerQueue_(new TimerQueue(this)) {
    LOG_TRACE << "EventLoop created " << this << "in thread " <<
              threadId_;
    if (t_loopInThisThread) {
        LOG_FATAL << "Another Eventloop" << t_loopInThisThread
                  << " exists in this thread " << threadId_;
    } else {
        t_loopInThisThread = this;
    }
}

RN::EventLoop::~EventLoop() {
    assert(!looping_);
    t_loopInThisThread = NULL;
}

void RN::EventLoop::loop() {
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;
    quit_ = false;
    while (!quit_) {
        activeChannels_.clear();
        poller_->poll(kPollTimeMs, &activeChannels_);
        for (ChannelList::iterator it = activeChannels_.begin();
             it != activeChannels_.end(); ++it) {
            (*it)->handleEvent();

        }

    }
    LOG_TRACE << "EventLoop " << this << " stop looping";
    looping_ = false;

}

void RN::EventLoop::abortNotInLoopThread() {
    LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
              << " was created in threadId_ = " << threadId_
              << ", current thread id = " << CurrentThread::tid();
}

void EventLoop::quit() {
    quit_ = true;
}

void EventLoop::updateChannel(Channel *channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}

TimerId EventLoop::runAt(const Timestamp &time, const TimerCallback cb) {
    return timerQueue_->addTimer(cb, time, 0.0);
}

TimerId EventLoop::runAfter(double delay, const TimerCallback cb) {
    Timestamp time(addTime(Timestamp::now(), delay));
    return runAt(time, cb);

}

TimerId EventLoop::runEvery(double interval, const TimerCallback cb) {
    return timerQueue_->addTimer(cb, Timestamp::now(), interval);
}
