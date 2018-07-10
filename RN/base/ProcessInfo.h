//
// Created by rmqi on 29/6/18.
//

#ifndef RN_PROCESSINFO_H
#define RN_PROCESSINFO_H

#include <sys/types.h>
#include <RN/base/Types.h>
#include <RN/base/Timestamp.h>
#include <vector>
#include "Timestamp.h"
#include "StringPiece.h"

namespace RN {

    namespace ProcessInfo {
        pid_t pid();

        string pidString();

        uid_t uid();

        string username();

        uid_t euid();

        Timestamp startTIme();

        int clockTicksPerSecond();

        int pageSize();

        bool isDebugBuild();

        string hostname();

        string procname();

        RN::StringPiece procname(const string &);

        string procStatus();

        string procStat();

        string threadStat();

        string exePath();

        int openedFiles();

        int maxOpenFiles();

        struct CpuTime {
            double userSeconds;
            double kernelSeconds;

            CpuTime()
                    : userSeconds(0), kernelSeconds(0) {
            }
        };

        CpuTime cpuTime();

        int numThreads();

        std::vector<pid_t> threads();


    }

}


#endif //RN_PROCESSINFO_H
