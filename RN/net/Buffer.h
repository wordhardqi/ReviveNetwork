//
// Created by rmqi on 12/7/18.
//

#ifndef RN_BUFFER_H
#define RN_BUFFER_H

#include <vector>
#include <assert.h>
#include <RN/base/Types.h>
#include <sys/uio.h>

namespace RN {
    class Buffer {
    public:
        static const size_t kCheapPrepend = 8;
        static const size_t kInitialSize = 1024;

        Buffer()
                : buffer_(kCheapPrepend + kInitialSize),
                  writerIndex_(kCheapPrepend),
                  readerIndex_(kCheapPrepend) {
            assert(readableBytes() == 0);
            assert(prependableBytes() == kCheapPrepend);
            assert(writableBytes() == kInitialSize);

        }

        size_t readableBytes() {
            return writerIndex_ - readerIndex_;
        }

        size_t writableBytes() {
            return buffer_.size() - writerIndex_;
        }

        size_t prependableBytes() {
            return readerIndex_;
        }

        const char *peek() const {
            return beginOfReader();
        }

        void retrive(size_t len) {
            assert(len <= readableBytes());
            readerIndex_ += len;
        }

        void retirveUtil(const char *util) {
            assert(util <= beginOfWriter());
            assert(beginOfReader() < util);
            retrive(util - beginOfReader());
        }

        void retrieveAll() {
            writerIndex_ = kCheapPrepend;
            readerIndex_ = kCheapPrepend;
        }

        string retirveAllAsString() {
            string str(beginOfReader(), readableBytes());
            retrieveAll();
            return str;
        }

        void ensureEnoughSpace(size_t len) {
            if (writableBytes() >= len) {
                return;
            } else {
                makeSpace(len);
            }
        }


        void append(const char *str, size_t len) {
            ensureEnoughSpace(len);
            assert(writableBytes() >= len);
            std::copy(str, str + len, beginOfReader());
            hasWritten(len);
        }

        void append(const void *str, size_t len) {
            append(static_cast<const char *>(str), len);
        }

        void append(const string &str, size_t len) {
            append(str.data(), len);
        }

        void prepend(const void *data, size_t len) {
            assert(len <= prependableBytes());
            readerIndex_ -= len;
            const char *d = static_cast<const char *>(data);
            std::copy(d, d + len, beginOfReader());
        }

        void hasWritten(size_t len) {
            writerIndex_ += len;
        }

        ssize_t readFd(int fd, int *savedErrno) {
            char buf[65536];
            struct iovec vec[2];
            size_t writeable = writableBytes();
            vec[0].iov_base = beginOfWriter();
            vec[0].iov_len = writeable;
            vec[1].iov_base = buf;
            vec[1].iov_len = sizeof(buf);
            ssize_t n = readv(fd, vec, 2);
            if (n < 0) {
                *savedErrno = errno;
            } else {
                if (n <= static_cast<ssize_t >(writeable)) {
                    writerIndex_ += n;
                } else {
                    writerIndex_ = buffer_.size();
                    append(buf, n - writeable);

                }
            }
            return n;
        }


    private:
        const char *begin() const {
            return &*buffer_.begin();
        }

        char *begin() {
            return &*buffer_.begin();
        }

        char *beginOfWriter() {
            return begin() + writerIndex_;
        }

        const char *beginOfWriter() const {
            return begin() + writerIndex_;
        }

        char *beginOfReader() {
            return begin() + readerIndex_;
        }

        const char *beginOfReader() const {
            return begin() + readerIndex_;
        }

        void makeSpace(size_t len) {
            if (writableBytes() + prependableBytes() < len + kCheapPrepend) {
                buffer_.resize(writerIndex_ + len);
            } else {
                assert(kCheapPrepend < readerIndex_);
                std::copy(beginOfReader(), beginOfWriter(), begin() + kCheapPrepend);
                writerIndex_ = kCheapPrepend + readableBytes();
                readerIndex_ = kCheapPrepend;


            }
        }

        std::vector<char> buffer_;
        size_t writerIndex_;
        size_t readerIndex_;

    };
}

#endif //RN_BUFFER_H
