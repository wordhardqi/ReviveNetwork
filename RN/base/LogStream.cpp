//
// Created by rmqi on 28/6/18.
//

#include "LogStream.h"
#include <limits>
#include <algorithm>

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wtautological-compare"
#else
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif

namespace RN {
    namespace detail {
        const char digits[] = "9876543210123456789";
        const char *zero = digits + 9;
        static_assert(sizeof(digits) == 20, "Wrong number of digits");

        const char digitsHex[] = "0123456789ABCDEF";
        static_assert(sizeof digitsHex == 17, "Wrong number of Hex digits");

        template<typename T>
        size_t convert(char buf[], T value) {
            T i = value;
            char *p = buf;
            do {
                int lsd = static_cast<int>(i % 10);
                i /= 10;
                *p = zero[lsd];
                p++;
            } while (i != 0);
            if (value < 0) {
                *p = '-';
                p++;
            }
            *p = '\0';
            std::reverse(buf, p);
            return p - buf;
        }

        size_t convertHex(char buf[], uintptr_t value) {
            uintptr_t i = value;
            char *p = buf;
            do {
                int lsd = static_cast<int>(i % 16);
                i /= 16;
                *p = digitsHex[lsd];
                p++;
            } while (i != 0);

            *p = '\0';
            std::reverse(buf, p);
            return p - buf;
        }

        template
        class FixedBuffer<kSmallBuffer>;

        template
        class FixedBuffer<kLargeBuffer>;


    }
}
using namespace RN;
using namespace RN::detail;


template<int SIZE>
const char *FixedBuffer<SIZE>::debugString() {
    *cur_ = '\0';
    return data_;
}

template<int SIZE>
void FixedBuffer<SIZE>::cookieStart() {}

template<int SIZE>
void FixedBuffer<SIZE>::cookieEnd() {}

void LogStream::staticCheck() {
#define kMaxNumbericSizeAssert(kMaxNumbericSize, nT)   \
{static_assert(kMaxNumbericSize - 10 > std::numeric_limits<nT>::digits10, \
        "kMaxNumericSize is Large enough");                 \
    }
    kMaxNumbericSizeAssert(kMaxNumbericSize, double);
    kMaxNumbericSizeAssert(kMaxNumbericSize, long double);
    kMaxNumbericSizeAssert(kMaxNumbericSize, long);
    kMaxNumbericSizeAssert(kMaxNumbericSize, long long);
#undef kMaxNumbericSizeAssert

}

template<typename T>
void LogStream::formatInteger(T v) {
    if (buffer_.avail() >= kMaxNumbericSize) {
        //copy is done by `convert`
        size_t len = convert(buffer_.current(), v);
        buffer_.add(len);
    }
}


LogStream &LogStream::operator<<(short v) {
    *this << static_cast<int>(v);
    return *this;
}

LogStream &LogStream::operator<<(unsigned short v) {
    *this << static_cast<int>(v);
    return *this;
}

LogStream &LogStream::operator<<(int v) {
    formatInteger(v);
    return *this;
}

LogStream &LogStream::operator<<(unsigned int v) {
    formatInteger(v);
    return *this;
}

LogStream &LogStream::operator<<(long v) {
    formatInteger(v);
    return *this;
}

LogStream &LogStream::operator<<(long long v) {
    formatInteger(v);
    return *this;
}

LogStream &LogStream::operator<<(unsigned long v) {
    formatInteger(v);
    return *this;
}

LogStream &LogStream::operator<<(unsigned long long v) {
    formatInteger(v);
    return *this;
}

LogStream &LogStream::operator<<(const void *p) {
    uintptr_t v = reinterpret_cast<uintptr_t >(p);
    if (buffer_.avail() >= kMaxNumbericSize) {
        char *buf = buffer_.current();
        buf[0] = '0';
        buf[1] = 'x';
        size_t len = convertHex(buf + 2, v);
        buffer_.add(len + 2);
    }
    return *this;
}

LogStream &LogStream::operator<<(float v) {
    *this << static_cast<double>(v);
    return *this;
}

LogStream &LogStream::operator<<(double v) {
    // Todo :: Grisu3 by Florian Loitsch.
    if (buffer_.avail() >= kMaxNumbericSize) {
        int len = snprintf(buffer_.current(), kMaxNumbericSize, "%.12g", v);
        buffer_.add(len);
    }
    return *this;
}

template<typename T>
Fmt::Fmt(const char *fmt, T val) {
    static_assert(std::is_arithmetic<T>::value == true,
                  "Must be arithmetic type");
    length_ = snprintf(buf_, sizeof(buf_), fmt, val);
}

template Fmt::Fmt(const char *fmt, char);

template Fmt::Fmt(const char *fmt, short);

template Fmt::Fmt(const char *fmt, int);

template Fmt::Fmt(const char *fmt, unsigned int);

template Fmt::Fmt(const char *fmt, long);

template Fmt::Fmt(const char *fmt, unsigned long);

template Fmt::Fmt(const char *fmt, long long);

template Fmt::Fmt(const char *fmt, unsigned long long);

template Fmt::Fmt(const char *fmt, float);

template Fmt::Fmt(const char *fmt, double);

LogStream &LogStream::operator<<(char v) {
    buffer_.append(&v, 1);
    return *this;
}

LogStream &LogStream::operator<<(const char *str) {
    if (str) {
        buffer_.append(str, strlen(str));
    } else {
        buffer_.append("(null)", 6);
    }
    return *this;
}



