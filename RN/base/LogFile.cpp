//
// Created by rmqi on 3/7/18.
//

#include <RN/base/LogFile.h>
#include <RN/base/ProcessInfo.h>
#include <RN/base/FileUtil.h>

RN::LogFile::LogFile(const RN::string &basename,
                     off_t rollSize,
                     bool threadSafe,
                     int flushInterval,
                     int checkEveryN)
        : basename_(basename),
          rollSize_(rollSize),
          flushInterval_(flushInterval),
          checkEveryN_(checkEveryN),
          count_(0),
          mutex_(threadSafe ? new MutexLock : NULL),
          startOfPeriod_(0),
          lastRoll_(0),
          lastFlush_(0) {
    assert(basename.find('/') == string::npos);
    rollFile();
}

RN::LogFile::~LogFile() {

}

void RN::LogFile::append(const char *logLine, int len) {
    if (mutex_) {
        MutexLockGuard lockGuard(*mutex_);
        append_unlocked(logLine, len);
    } else {
        append_unlocked(logLine, len);
    }
}

void RN::LogFile::flush() {
    if (mutex_) {
        MutexLockGuard lockGuard(*mutex_);
        file_->flush();
    } else {
        file_->flush();
    }

}

bool RN::LogFile::rollFile() {
    time_t now = 0;
    string filename = getLogFileName(basename_, &now);
    time_t start = now / kSecondsPerRoll * kSecondsPerRoll;
    if (now > lastRoll_) {
        lastRoll_ = now;
        lastFlush_ = now;
        startOfPeriod_ = start;
        file_.reset(new FileUtil::AppendFile(filename));
        return true;
    }
    return false;
}

RN::string RN::LogFile::getLogFileName(const RN::string &basename, time_t *now) {
    string filename;
    filename.reserve(basename.size() + 64);
    filename = basename;

    char timebuf[32];
    struct tm tm;
    *now = time(NULL);
    gmtime_r(now, &tm);
    strftime(timebuf, sizeof(timebuf), ".%Y%m%d-%H%M%S.", &tm);
    filename += timebuf;

    filename += ProcessInfo::hostname();
    char pidbuf[32];
    snprintf(pidbuf, sizeof(pidbuf), ".%d", ProcessInfo::pid());
    filename += pidbuf;

    filename += ".log";
    return filename;


}

void RN::LogFile::append_unlocked(const char *logLine, int len) {
    file_->append(logLine, len);
    if (file_->writtenBytes() > rollSize_) {
        rollFile();
    } else {
        ++count_;
        if (count_ >= checkEveryN_) {
            count_ = 0;
            time_t now = ::time(NULL);
            time_t thisPeriodStart = now / kSecondsPerRoll * kSecondsPerRoll;
            if (thisPeriodStart != lastRoll_) {
                rollFile();
            } else if (now - lastFlush_ > flushInterval_) {
                //File is not rolled.
                lastFlush_ = now;
                file_->flush();
            }

        }

    }
}
