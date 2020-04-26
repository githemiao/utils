#ifndef NOW_H
#define NOW_H

#include <chrono>
#include <cstddef>
#include <ctime>

enum CLOCK_TYPE {
    SYSTEM_CLOCK = 0,
    STEADY_CLOCK
};

static inline int64_t now_in_usec(int clock_type = 0) {
    if (clock_type == SYSTEM_CLOCK) {
        auto now = std::chrono::system_clock::now();
        auto now_tp = std::chrono::time_point_cast<std::chrono::microseconds>(now);
        auto us = now_tp.time_since_epoch().count();
        return us;
    } else if (clock_type == STEADY_CLOCK) {
        auto now = std::chrono::steady_clock::now();
        auto now_tp = std::chrono::time_point_cast<std::chrono::microseconds>(now);
        auto us = now_tp.time_since_epoch().count();
        return us;
    }

    return -1;
}

static inline int64_t now_in_msec(int clock_type = 0) {
    if (clock_type == SYSTEM_CLOCK) {
        auto now = std::chrono::system_clock::now();
        auto now_tp = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        auto ms = now_tp.time_since_epoch().count();
        return ms;
    } else if (clock_type == STEADY_CLOCK) {
        auto now = std::chrono::steady_clock::now();
        auto now_tp = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        auto ms = now_tp.time_since_epoch().count();
        return ms;
    }

    return -1;
}

static inline int64_t now_in_sec(int clock_type = 0) {
    if (clock_type == SYSTEM_CLOCK) {
        auto now = std::chrono::system_clock::now();
        auto now_tp = std::chrono::time_point_cast<std::chrono::seconds>(now);
        auto sec = now_tp.time_since_epoch().count();
        return sec;
    } else if (clock_type == STEADY_CLOCK) {
        auto now = std::chrono::steady_clock::now();
        auto now_tp = std::chrono::time_point_cast<std::chrono::seconds>(now);
        auto sec = now_tp.time_since_epoch().count();
        return sec;
    }

    return -1;
}

static inline std::tm * sec_to_date(int64_t sec) {
    std::tm *tm = std::localtime((time_t *)&sec);

    if (tm) {
        tm->tm_year += 1900;
        tm->tm_mon += 1;

        return tm;
    }

    return nullptr;
}

static inline std::tm * msec_to_date(int64_t ms) {
    int64_t sec = ms / 1000;  // to seconds
    return sec_to_date(sec);
}

#endif  /* NOW_H */