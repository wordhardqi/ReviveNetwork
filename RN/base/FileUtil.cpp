//
// Created by rmqi on 2/7/18.
//

#include <RN/base/FileUtil.h>
#include <fcntl.h>

using namespace RN;

RN::FileUtil::AppendFile::AppendFile(RN::StringArg fileName)
        : fp_(::fopen(fileName.c_str(), "ae")), writeenBytes_(0) {
    assert(fp_);
    ::setbuffer(fp_, buffer_, sizeof(buffer_));

}


RN::FileUtil::AppendFile::~AppendFile() {
    ::fclose(fp_);
}

void RN::FileUtil::AppendFile::flush() {
    ::fflush(fp_);
}

size_t RN::FileUtil::AppendFile::write(const char *logLine, size_t len) {
    return ::fwrite_unlocked(logLine, 1, len, fp_);
}

__thread char t_errnobuf[512];

//const char *strerror_tl(int err) {
//    return strerror_r(err, t_errnobuf, sizeof t_errnobuf);
//
//}
//Though it's quite stupid to pass len;
void RN::FileUtil::AppendFile::append(const char *logLine, const size_t len) {
    size_t n = write(logLine, len);
    size_t remain = (len - n);
    while (remain > 0) {
        size_t x = write(logLine + n, remain);
        if (x == 0) {
            int err = ferror(fp_);
            if (err) {
                fprintf(stderr, "AppendFile::append() failed %s\n", strerror_tl(err));
            }
        }
        n += x;
        remain -= x;
    }
    writeenBytes_ += len;

}

RN::FileUtil::ReadSmallFile::ReadSmallFile(RN::StringArg fileName)
        : fd_(::open(fileName.c_str(), O_RDONLY | O_CLOEXEC)),
          err_(0) {
    buf_[0] = '\0';
    if (fd_ < 0) {
        err_ = errno;
    }

}

RN::FileUtil::ReadSmallFile::~ReadSmallFile() {
    if (fd_ > 0) {
        again:
        int r = ::close(fd_);
        if (!r) {
            if (errno == EINTR) {
                goto again;
            } else {
                fprintf(stderr, "ReadSmallFile::~ReadSmallFile() failed %s\n", strerror_tl(errno));
            }
        }
    }

}

const char *RN::FileUtil::ReadSmallFile::buffer() {
    return buf_;
}

int RN::FileUtil::ReadSmallFile::readToBuffer(int *size) {
    int err = err_;
    if (fd_ >= 0) {
        ssize_t n = ::pread(fd_, buf_, sizeof(buf_) - 1, 0);
        if (n >= 0) {
            if (size) {
                *size = static_cast<int>(n);
            }
            buf_[n] = '\0';
        } else {
            err = errno;
        }

    }
    return err;
}

template<typename String>
int RN::FileUtil::ReadSmallFile::readToString(int maxSize,
                                              String *content,
                                              int64_t *fileSize,
                                              int64_t *modifyTime,
                                              int64_t *createTime) {
    static_assert(sizeof(off_t) == 8, "_FILE_OFFSET_BITS = 64");
    assert(content != NULL);
    int err = err_;
    if (fd_ >= 0) {
        content->clear();
        if (fileSize) {
            struct stat statbuf;
            if (::fstat(fd_, &statbuf) == 0) {
                if (S_ISREG(statbuf.st_mode)) {
                    *fileSize = statbuf.st_size;
                    content->reserve(static_cast<int>(std::min(implicit_cast<int64_t>(maxSize), *fileSize)));
                } else if (S_ISDIR(statbuf.st_mode)) {
                    err = EISDIR;
                }
                if (modifyTime) {
                    *modifyTime = statbuf.st_mtime;
                }
                if (createTime) {
                    *createTime = statbuf.st_ctime;
                }
            }
        } else {
            err = errno;
        }
        while (content->size() < implicit_cast<size_t>(maxSize)) {
            size_t toRead = std::min(implicit_cast<size_t>(maxSize) - content->size(), sizeof(buf_));;
            ssize_t n = ::read(fd_, buf_, toRead);
            if (n > 0) {
                content->append(buf_, n);
            } else {
                if (n < 0) {
                    err = errno;
                }
                break;
            }
        }

    }
    return err;
}

template int FileUtil::readFile(StringArg fileName,
                                int maxSize,
                                string *content,
                                int64_t *, int64_t *, int64_t *);

template int FileUtil::ReadSmallFile::readToString(int maxSize, string *content, int64_t *fileSize, int64_t *modifyTime,
                                                   int64_t *createTime);

#ifndef  RN_STD_STRING

template int FileUtil::readFile(StringArg fileName,
                                int maxSize,
                                std::string *content,
                                int64_t *, int64_t *, int64_t *);

template int
FileUtil::ReadSmallFile::readToString(int maxSize, std::string *content, int64_t *fileSize, int64_t *modifyTime,
                                      int64_t *createTime);

#endif
