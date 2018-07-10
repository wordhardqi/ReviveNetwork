//
// Created by rmqi on 2/7/18.
//

#ifndef RN_FILEUTIL_H
#define RN_FILEUTIL_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <RN/base/StringPiece.h>
#include <RN/base/Logging.h>

namespace RN {
    namespace FileUtil {

        class AppendFile : noncopyable {
        public:
            AppendFile(StringArg fileName);

            ~AppendFile();

            void flush();

            void append(const char *logLine, const size_t len);

            off_t writtenBytes() {
                return writeenBytes_;
            }

        private:
            size_t write(const char *logLine, size_t len);

            FILE *fp_;
            char buffer_[64 * 1024];
            off_t writeenBytes_;
        };

        class ReadSmallFile {
        public:
            static const int kBufferSize = 64 * 1024;

            ReadSmallFile(StringArg fileName);

            ~ReadSmallFile();


            const char *buffer();

            template<typename String>
            int readToString(int maxSize,
                             String *content,
                             int64_t *fileSize,
                             int64_t *modifyTime,
                             int64_t *createTime);

            int readToBuffer(int *size);


        private:
            int fd_;
            int err_;
            char buf_[kBufferSize];
        };

        template<typename String>
        int readFile(StringArg filename, int maxSize,
                     String *content,
                     int64_t *fileSize = NULL,
                     int64_t *modifyTime = NULL,
                     int64_t *createTime = NULL) {
            ReadSmallFile file(filename);
            //the *fileSize parameter is weird.
            return file.readToString(maxSize, content, fileSize, modifyTime, createTime);
        }
    }

}


#endif //RN_FILEUTIL_H
