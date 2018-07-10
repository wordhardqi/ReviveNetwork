//
// Created by rmqi on 4/7/18.
//

#ifndef RN_LOGGING_H
#define RN_LOGGING_H

#include <RN/base/Timestamp.h>
#include <RN/base/LogStream.h>

namespace RN {
    namespace detail {
        class SourceFile {
        public:
            const char *data_;
            int size_;

            // const char __FILE__[N]
            template<int N>
            SourceFile(const char (&filename)[N]):
                    data_(filename), size_(N - 1) {
                const char *slash = strrchr(data_, '/');
                if (slash) {
                    data_ = slash + 1;
                    size_ -= static_cast<int>(data_ - filename);
                }
            }

            SourceFile(const char *filename)
                    : data_(filename), size_(static_cast<int>(strlen(filename))) {
                const char *slash = strrchr(filename, '/');
                if (slash) {
                    data_ = slash + 1;

                }
                size_ = static_cast<int>(strlen(data_));
            }
        };

        class LengthChecker {
        public:
            const char *str_;
            const unsigned len_;

            LengthChecker(const char *str, unsigned int len)
                    : str_(str), len_(len) {
//                fprintf(stderr ,"%s", str);
//                fprintf(stderr,"  %d, %d", static_cast<int>((strlen(str))), static_cast<int>(len));
                assert(strlen(str) == len_);
            }
        };

    }
}
namespace RN {
    using namespace RN::detail;
    enum LogLevel {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS,
    };

    class Logger {
    public:
        typedef void (*OutputFunc)(const char *, int);

        typedef void (*FlushFunc)();

        static void setOutputFunc(OutputFunc);

        static void setFlushFunc(FlushFunc);

        static LogLevel logLevel();

//        static void timeZone();

        Logger(SourceFile file, int line);

        Logger(SourceFile file, int line, LogLevel level);

        //func is usually __FUNC__
        Logger(SourceFile file, int line, LogLevel level, const char *func);

        Logger(SourceFile file, int line, bool toAbort);

        ~Logger();

        LogStream &stream() {
            return impl_.stream_;
        }

    private:
        //Simple wrapper for initialization.
        class Impl {
        public:
            Impl(LogLevel level, int savedError, const SourceFile &file, int line);

            void finish();

            void formatTime();

            Timestamp time_;
            LogStream stream_;
            LogLevel level_;
            int line_;
            SourceFile basename_;

        };

        Impl impl_;
    };

    extern Logger::OutputFunc g_outputFunc;
    extern Logger::FlushFunc g_flushFunc;

    const char *strerror_tl(int savedErrno);

#define LOG_TRACE if (RN::Logger::logLevel()<= RN::LogLevel::TRACE) \
        RN::Logger(__FILE__,__LINE__, RN::LogLevel::TRACE,__func__).stream()
#define LOG_DEBUG if (RN::Logger::logLevel()<= RN::LogLevel::DEBUG) \
        RN::Logger(__FILE__,__LINE__, RN::LogLevel::DEBUG,__func__).stream()
#define LOG_INFO if (RN::Logger::logLevel()<= RN::LogLevel::INFO) \
        RN::Logger(__FILE__,__LINE__, RN::LogLevel::INFO,__func__).stream()
#define LOG_WARN RN::Logger(__FILE__,__LINE__, RN::LogLevel::WARN).stream()
#define LOG_ERROR RN::Logger(__FILE__,__LINE__, RN::LogLevel::ERROR).stream()
#define LOG_FATAL RN::Logger(__FILE__,__LINE__, RN::LogLevel::FATAL).stream()
#define LOG_SYSERR RN::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL RN::Logger(__FILE__, __LINE__, true).stream()


}


#endif //RN_LOGGING_H
