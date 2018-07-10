//
// Created by rmqi on 3/7/18.
//

#ifndef RN_LOGFILE_H
#define RN_LOGFILE_H


#include <bits/unique_ptr.h>
#include <RN/base/Types.h>
#include <RN/base/Mutex.h>

namespace RN {
    namespace FileUtil {
        class AppendFile;
    }
    class LogFile {
    public:
        LogFile(const string &basename,
                off_t rollSize,
                bool threadSafe = true,
                int flushInterval = 3,
                int checkEveryN = 1024);

        ~LogFile();

        void append(const char *logLine, int len);

        void flush();

        bool rollFile();

    private:
        static string getLogFileName(const string &basename, time_t *now);

        void append_unlocked(const char *logLine, int len);

        const string basename_;
        const off_t rollSize_;
        const int flushInterval_;
        const int checkEveryN_;
        int count_;
        std::unique_ptr<MutexLock> mutex_;
        time_t startOfPeriod_;
        time_t lastRoll_;
        time_t lastFlush_;
        std::unique_ptr<FileUtil::AppendFile> file_;
        const static int kSecondsPerRoll = 24 * 60 * 60;
    };
}

#endif //RN_LOGFILE_H
