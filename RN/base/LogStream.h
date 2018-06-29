//
// Created by rmqi on 28/6/18.
//

#ifndef RN_LOGSTREAM_H
#define RN_LOGSTREAM_H

#include <RN/base/StringPiece.h>
#include <RN/base/Types.h>
#include <assert.h>
#include <string.h>

#ifndef RN_STD_STRING

#include <string>

#endif

namespace RN {
    namespace detail {


        const int kSmallBuffer = 4000;
        const int kLargeBuffer = 4000 * 1000;


        template<int SIZE>
        class FixedBuffer : noncopyable {

        public:
            FixedBuffer()
                    : cur_(data_) {
                setCookie(cookieStart);
            }

            ~FixedBuffer() {
                setCookie(cookieEnd);
            }

            void setCookie(void (*cookie)()) {
                cookie_ = cookie;
            }

            const char *data() const { return data_; }

            int length() const {
                return static_cast<int>(cur_ - data_);
            }

            char *current() {
                return cur_;
            }

            int avail() const {
                return static_cast<int>(end() - cur_);
            }

            void add(size_t len) {
                cur_ += len;
            }

            void reset() {
                cur_ = data_;
            }

            void bzero() {
                ::bzero(data_, sizeof(data_));
            }

            const char *debugString();

            void append(const char *buf, size_t len) {
                if (implicit_cast<size_t>(avail()) > len) {
                    memcpy(cur_, buf, len);
                    cur_ += len;
                }
            }

            string toString() const {
                return string(data_, length());
            }

            StringPiece toStringPiece() const {
                return StringPiece(data_, length());
            }

        private:
            const char *end() const {
                return data_ + sizeof(data_);
            }

            static void cookieStart();

            static void cookieEnd();

            void (*cookie_)();

            char data_[SIZE];
            char *cur_;

        };

    }
    class LogStream {
        typedef LogStream self;
    public:
        typedef detail::FixedBuffer<detail::kSmallBuffer> Buffer;
    private:
        void staticCheck();

        template<typename T>
        void formatInteger(T);

        Buffer buffer_;
        static const int kMaxNumbericSize = 32;
    public:
        self &operator<<(bool v) {
            buffer_.append(v ? "1" : "0", 1);
            return *this;
        }

        self &operator<<(short);

        self &operator<<(unsigned short);

        self &operator<<(int);

        self &operator<<(unsigned int);

        self &operator<<(long);

        self &operator<<(unsigned long);

        self &operator<<(long long);

        self &operator<<(unsigned long long);

        self &operator<<(const void *);

        self &operator<<(float v);

        self &operator<<(double);

        self &operator<<(char v);

        self &operator<<(const char *str);

        self &operator<<(const unsigned char *str) {
            return operator<<(reinterpret_cast<const char *>(str));
        }

        self &operator<<(const string &v) {
            buffer_.append(v.c_str(), v.size());
            return *this;
        }

#ifndef RN_STD_STRING

        self &operator<<(const std::string &v) {
            buffer_.append(v.c_str(), v.size());
            return *this;
        }

#endif

        self &operator<<(const StringPiece &v) {
            buffer_.append(v.data(), v.size());
            return *this;
        }

        self &operator<<(const Buffer &v) {
            *this << v.toStringPiece();
            return *this;
        }

        void append(const char *data, int len) {
            buffer_.append(data, len);
        }

        const Buffer &buffer() const {
            return buffer_;
        }

        void resetBuffer() {
            buffer_.reset();
        }
    };

    class Fmt : noncopyable {
    private:
        char buf_[32];
        int length_;
    public:
        template<typename T>
        Fmt(const char *fmt, T val);

        const char *data() const { return buf_; }

        int length() const { return length_; }
    };

    inline LogStream &operator<<(LogStream &s, const Fmt &fmt) {
        s.append(fmt.data(), fmt.length());
        return s;
    }


}

#endif //RN_LOGSTREAM_H



