//
// Created by rmqi on 3/7/18.
//

#include <RN/base/AsyncLogging.h>
#include <RN/base/LogFile.h>
#include <RN/base/Timestamp.h>

RN::AsyncLogging::AsyncLogging(const RN::string &basename,
                               off_t rollSize,
                               int flushInterval)
        : flushInterval_(flushInterval),
          running_(false),
          basename_(basename),
          rollSize_(rollSize),
          thread_(std::bind(&AsyncLogging::threadFunc, this), "Logging"),
          latch_(1),
          mutex_(),
          cond_(mutex_),
          currentBuffer_(new Buffer),
          nextBuffer_(new Buffer),
          buffers_() {
    currentBuffer_->bzero();
    nextBuffer_->bzero();
    buffers_.reserve(16);

}

RN::AsyncLogging::~AsyncLogging() {
    if (running_) {
        stop();
    }
}

void RN::AsyncLogging::start() {
    running_ = true;
    thread_.start();
    latch_.wait();
}

void RN::AsyncLogging::stop() {
    running_ = false;
    cond_.notify();
    thread_.join();
}

void RN::AsyncLogging::append(const char *logLine, int len) {
    RN::MutexLockGuard lockGuard(mutex_);
    if (currentBuffer_->avail() > len) {
        currentBuffer_->append(logLine, len);
    } else {
        buffers_.push_back(std::move(currentBuffer_));
        if (nextBuffer_) {
            currentBuffer_ = std::move(nextBuffer_);
        } else {
            currentBuffer_.reset(new Buffer);
        }
        currentBuffer_->append(logLine, len);
        cond_.notify();
    }

}

void RN::AsyncLogging::threadFunc() {
    assert(running_ == true);
    latch_.countDown();
    LogFile logFile(basename_, rollSize_, false);
    BufferPtr firstNewBuffer(new Buffer);
    BufferPtr secondNewBuffer(new Buffer);
    firstNewBuffer->bzero();
    secondNewBuffer->bzero();
    BufferVector bufferToWrite;
    bufferToWrite.reserve(16);

    while (running_) {
        assert(firstNewBuffer && firstNewBuffer->length() == 0);
        assert(secondNewBuffer && secondNewBuffer->length() == 0);
        assert(bufferToWrite.empty());

        RN::MutexLockGuard lockGuard(mutex_);
        if (bufferToWrite.empty()) {
            cond_.waitForSeconds(flushInterval_);
        }
        buffers_.push_back(std::move(currentBuffer_));
        currentBuffer_ = std::move(firstNewBuffer);
        bufferToWrite.swap(buffers_);
        if (!nextBuffer_) {
            nextBuffer_ = std::move(secondNewBuffer);
        }

        assert(!bufferToWrite.empty());
        if (bufferToWrite.size() > 25) {
            char buf[256];
            snprintf(buf, sizeof(buf), "Dropped log message at %s, %zd large buffer",
                     Timestamp::now().toFormattedString().c_str(), bufferToWrite.size() - 2);
            fputs(buf, stderr);
            logFile.append(buf, static_cast<int>(strlen(buf)));
            bufferToWrite.erase(bufferToWrite.begin() + 2, bufferToWrite.end());
        }
        for (size_t i = 0; i < bufferToWrite.size(); i++) {
            logFile.append(bufferToWrite[i]->data(), bufferToWrite[i]->length());
        }
        if (bufferToWrite.size() > 2) {
            bufferToWrite.resize(2);
        }
        if (!firstNewBuffer) {
            assert(!bufferToWrite.empty());
            firstNewBuffer = std::move(bufferToWrite.back());
            bufferToWrite.pop_back();
            //reset the buffer, not the containing unique_ptr.
            firstNewBuffer->reset();
        }
        if (!secondNewBuffer) {
            assert(!bufferToWrite.empty());
            secondNewBuffer = std::move(bufferToWrite.back());
            bufferToWrite.pop_back();
            secondNewBuffer->reset();
        }
        bufferToWrite.clear();
        logFile.flush();

    }
    logFile.flush();

}
