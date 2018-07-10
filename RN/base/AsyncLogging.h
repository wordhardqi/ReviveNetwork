//
// Created by rmqi on 3/7/18.
//

#ifndef RN_ASYNCLOGGING_H
#define RN_ASYNCLOGGING_H

#include <bits/unique_ptr.h>
#include <vector>
#include <atomic>
#include "LogStream.h"
#include "Thread.h"

namespace RN {
    class AsyncLogging {
    public:
        AsyncLogging(const string &basename,
                     off_t rollSize,
                     int flushInterval = 3);

        ~AsyncLogging();

        void start();

        void stop();

        void append(const char *logLine, int len);

    private:

        typedef RN::detail::FixedBuffer<RN::detail::kLargeBuffer> Buffer;
        typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
        typedef BufferVector::value_type BufferPtr;

        void threadFunc();

        const int flushInterval_;
        std::atomic<bool> running_;
        string basename_;
        off_t rollSize_;

        RN::Thread thread_;
        RN::CountDownLatch latch_;

        RN::MutexLock mutex_;
        RN::Condition cond_;
        BufferPtr currentBuffer_;
        BufferPtr nextBuffer_;
        BufferVector buffers_;


    };

}


#endif //RN_ASYNCLOGGING_H
