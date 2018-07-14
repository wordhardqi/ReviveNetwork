//
// Created by rmqi on 8/7/18.
//

#include <RN/base/Logging.h>
#include <signal.h>
#include "EventLoop.h"
#include "Poller.h"

using namespace RN;
__thread EventLoop *t_loopInThisThread = 0;
const int kPollTimeMs = 10000;

class IngnoreSigPipe {
public:
    IngnoreSigPipe() {
        ::signal(SIGPIPE, SIG_IGN);
    }

private:
};

IngnoreSigPipe ingnoreSigPipeInitObj;
RN::EventLoop::EventLoop()
        : looping_(false), quit_(false),
          threadId_(RN::CurrentThread::tid()),
          poller_(new Poller(this)), pollReturnTime_(0),
          timerQueue_(new TimerQueue(this)),
          wakeupTrigger_(new IOWakeupTrigger(this)),
          pendingFunctors_(), callingPendingFunctors_(false),
          mutex_() {
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
        Timestamp receivedTime = Timestamp::now();
        for (ChannelList::iterator it = activeChannels_.begin();
             it != activeChannels_.end(); ++it) {
            (*it)->handleEvent(receivedTime);

        }
        doPendingFunctors();

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
    if (!isInLoopThread()) {
        wakeup();
    }
}

void EventLoop::updateChannel(Channel *channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}

void EventLoop::removeChannle(Channel *channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->removeChannel(channel);
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

void EventLoop::cancle(TimerId timerId) {


}

void EventLoop::queueInLoop(const EventLoop::Functor &cb) {
    {
        MutexLockGuard lockGuard(mutex_);
        pendingFunctors_.push_back(cb);
    }
    if (!isInLoopThread() || callingPendingFunctors_) {
        wakeup();
    }
}

void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    callingPendingFunctors_ = true; /*atomic*/
    {
        MutexLockGuard lockGuard(mutex_);
        functors.swap(pendingFunctors_);
    }
    for (auto &cb : functors) {
        cb();
    }
    callingPendingFunctors_ = false;

}

void EventLoop::runInLoop(const EventLoop::Functor &cb) {
    if (isInLoopThread()) {
        cb();
    } else {
        queueInLoop(cb);
    }
}
