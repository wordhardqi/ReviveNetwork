//
// Created by rmqi on 29/6/18.
//

#include <RN/base/ProcessInfo.h>
#include <pwd.h>
//TOdo:: not finished
namespace RN {
    namespace detail {
        Timestamp g_startTime = Timestamp::now();
        int g_clockTicksPerSecond = static_cast<int>(::sysconf(_SC_CLK_TCK));
        int g_pageSize = static_cast<int>(::sysconf(_SC_PAGESIZE));
    }
}

pid_t RN::ProcessInfo::pid() {
    return ::getpid();
}

RN::string RN::ProcessInfo::pidString() {
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", pid());
    return buf;
}

uid_t RN::ProcessInfo::uid() {
    return ::getuid();
}

uid_t RN::ProcessInfo::euid() {
    return ::geteuid();
}

RN::string RN::ProcessInfo::username() {
    const char *name = "UnknownUser";
    struct passwd pwd;
    struct passwd *result = NULL;
    char buf[8192];
    getpwuid_r(uid(), &pwd, buf, sizeof(buf), &result);
    if (result) {
        name = result->pw_name;
    }
    return name;

}

RN::Timestamp RN::ProcessInfo::startTIme() {
    return detail::g_startTime;
}

int RN::ProcessInfo::clockTicksPerSecond() {
    return detail::g_clockTicksPerSecond;
}

int RN::ProcessInfo::pageSize() {
    return detail::g_pageSize;
}

bool RN::ProcessInfo::isDebugBuild() {
#ifdef NDEBUG
    return false;
#else
    return true;
#endif
}

RN::string RN::ProcessInfo::hostname() {
    char buf[256] = {'\0'};
    if (::gethostname(buf, sizeof(buf)) == 0) {
        buf[sizeof(buf) - 1] = '\0';
        return buf;
    } else {
        return "UnknownHost";
    }
}

RN::string RN::ProcessInfo::procname() {
    return RN::string();
}

RN::StringPiece RN::ProcessInfo::procname(const RN::string &stat) {
    StringPiece name;
    size_t lp = stat.find('(');
    size_t rp = stat.find(')');
    if (lp != string::npos && rp != string::npos && lp < rp) {
        name.set(stat.data() + lp + 1, static_cast<int>(rp - rp - 1));
    }
    return name;

}

RN::string RN::ProcessInfo::procStatus() {
    return RN::string();
}

RN::string RN::ProcessInfo::procStat() {
    return RN::string();
}

RN::string RN::ProcessInfo::threadStat() {
    return RN::string();
}

RN::string RN::ProcessInfo::exePath() {
    return RN::string();
}

int RN::ProcessInfo::openedFiles() {
    return 0;
}

int RN::ProcessInfo::maxOpenFiles() {
    return 0;
}

RN::ProcessInfo::CpuTime RN::ProcessInfo::cpuTime() {
    return RN::ProcessInfo::CpuTime();
}

int RN::ProcessInfo::numThreads() {
    return 0;
}

std::vector<pid_t> RN::ProcessInfo::threads() {
    return std::vector<pid_t>();
}
