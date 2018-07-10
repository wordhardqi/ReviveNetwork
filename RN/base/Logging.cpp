//
// Created by rmqi on 4/7/18.
//

#include <RN/base/Logging.h>
#include <RN/base/CurrentThread.h>

namespace RN {
    __thread char t_errnobuf[512];
    __thread char t_time[64];
    __thread time_t t_lastSecond;

    const char *strerror_tl(int savedErrno) {
        return strerror_r(savedErrno, t_errnobuf, sizeof(t_errnobuf));
    }

    const char *LogLevelName[LogLevel::NUM_LOG_LEVELS] = {
            "TRACE ",
            "DEBUG ",
            "INFO  ",
            "WARN  ",
            "ERROR ",
            "FATAL ",
    };

    void defaultOutputFunc(const char *msg, int len) {
        ::fwrite(msg, 1, len, stdout);
    }

    void defaultFlushFunc() {
        ::fflush(stdout);
    }

    RN::LogLevel initLogLevel() {
        if (::getenv("RN_LOG_TRACE")) {
            return LogLevel::TRACE;
        } else if (::getenv("RN_LOG_DEBUG")) {
            return LogLevel::DEBUG;
        } else {
            return LogLevel::INFO;
        }
    }

    Logger::OutputFunc g_outputFunc = defaultOutputFunc;
    Logger::FlushFunc g_flushFunc = defaultFlushFunc;
    RN::LogLevel g_logLevel = initLogLevel();

    LogStream &operator<<(LogStream &out, LengthChecker v) {
        out.append(v.str_, v.len_);
        return out;
    }

    LogStream &operator<<(LogStream &out, const SourceFile &v) {
        out.append(v.data_, v.size_);
        return out;
    }


    Logger::Impl::Impl(LogLevel level, int savedError, const SourceFile &file, int line)
            : time_(Timestamp::now()),
              stream_(),
              level_(level),
              line_(line),
              basename_(file) {
        formatTime();
        CurrentThread::tid();
        stream_ << LengthChecker(CurrentThread::tidString(), CurrentThread::tidStringLength());
        stream_ << LengthChecker(LogLevelName[level], 6);
        if (savedError != 0) {
            stream_ << strerror_tl(savedError) << "(errno=" << savedError << ") ";

        }

    }

    void Logger::Impl::formatTime() {
        int64_t microSecondsSinceEpoch = time_.microSecondsSinceEpoch();
        time_t seconds = static_cast<time_t >(microSecondsSinceEpoch / Timestamp::kMicroSecondsPerSecond);
        int microseconds = static_cast<int>(microSecondsSinceEpoch % Timestamp::kMicroSecondsPerSecond);
        if (seconds != t_lastSecond) {
            t_lastSecond = seconds;
            struct tm tm_time;
            ::gmtime_r(&seconds, &tm_time);
            int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
                               tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                               tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
            (void) len;

        }
        Fmt us(".%06dZ ", microseconds);

        stream_ << LengthChecker(t_time, 17) << LengthChecker(us.data(), 9);


    }

    void Logger::Impl::finish() {
        stream_ << " - " << basename_ << ":" << line_ << '\n';
    }
}

void RN::Logger::setOutputFunc(OutputFunc func) {
    g_outputFunc = func;
}

void RN::Logger::setFlushFunc(FlushFunc func) {
    g_flushFunc = func;
}

RN::Logger::Logger(RN::SourceFile file, int line)
        : impl_(INFO, 0, file, line) {

}

RN::Logger::Logger(RN::SourceFile file, int line, RN::LogLevel level)
        : impl_(level, 0, file, line) {

}

RN::Logger::Logger(RN::SourceFile file, int line, RN::LogLevel level, const char *func)
        : impl_(level, 0, file, line) {
    impl_.stream_ << func << ' ';
}

RN::Logger::Logger(RN::SourceFile file, int line, bool toAbort)
        : impl_(toAbort ? FATAL : ERROR, errno, file, line) {

}

RN::Logger::~Logger() {
    impl_.finish();
    const LogStream::Buffer &buffer(impl_.stream_.buffer());
    g_outputFunc(buffer.data(), buffer.length());
    if (impl_.level_ == FATAL) {
        g_flushFunc();
        abort();
    }

}

RN::LogLevel RN::Logger::logLevel() {
    return g_logLevel;
}

