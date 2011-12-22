#include "jm_timer.hpp"
#include <ctime>
#include <sstream>
#include <boost/thread.hpp>
#ifdef JM_OS_WIN
#include <windows.h>
#endif

namespace jm {

time_val::time_val() : _tv_sec(0), _tv_usec(0) {
}

time_val::time_val(const time_val &other) {
    _tv_sec = other._tv_sec;
    _tv_usec = other._tv_usec;
}

time_val& time_val::operator=(const time_val &other) {
    _tv_sec = other._tv_sec;
    _tv_usec = other._tv_usec;
    return *this;
}

std::string time_val::to_iso_8601() {
    struct tm *tm;
    time_t secs;

    if (_tv_sec < 0 || _tv_usec > USEC_PER_SEC) {
        return std::string();
    }
    secs = _tv_sec;
#ifdef JM_OS_WIN
    tm = gmtime(&secs);
#else
    tm = gmtime(&secs);
#endif
    std::stringstream ss(std::stringstream::in | std::stringstream::out);
    if (_tv_usec != 0) {
        // ISO8601 date and time format, with fractionary seconds:
        // YYYY-MM-DDTHH:MM:SS.MMMMMMZ
        ss << tm->tm_year + 1900 << "-"
                << tm->tm_mon + 1 << "-"
                << tm->tm_mday << "T"
                << tm->tm_hour << ":"
                << tm->tm_min << ":"
                << tm->tm_sec << "."
                << _tv_usec;
    } else {
        // ISO 8601 date and time format:
        // YYYY-MM-DDTHH:MM:SSZ
        ss << tm->tm_year + 1900 << "-"
                << tm->tm_mon + 1 << "-"
                << tm->tm_mday << "T"
                << tm->tm_hour << ":"
                << tm->tm_min << ":"
                << tm->tm_sec;
    }
    return ss.str();
}

time_val time_val::get_current_time() {
    time_val result;

#ifdef JM_OS_WIN
    FILETIME ft;
    uint64 time64;

    GetSystemTimeAsFileTime(&ft);
    memmove(&time64, &ft, sizeof (FILETIME));

    // Convert from 100s of nanoseconds since 1601-01-01
    // to Unix epoch. Yes, this is Y2038 unsafe.
    time64 -= JM_INT64_CONSTANT(116444736000000000);
    time64 /= 10;

    result.set_sec(time64 / 1000000);
    result.set_usec(time64 % 1000000);

#else
    struct timeval r;

    // this is required on alpha, there the timeval structs are int's
    // not longs and a cast only would fail horribly
    gettimeofday(&r, NULL);
    result.set_sec(r.tv_sec);
    result.set_usec(r.tv_usec);
#endif
    return result;
}

void time_val::set_sec(int64 value) {
    _tv_sec = value;
}

void time_val::set_usec(jm::int64 value) {
    _tv_usec = value;
}

int64 time_val::sec() {
    return _tv_sec;
}

int64 time_val::usec() {
    return _tv_usec;
}

void time_val::add(int64 microseconds) {
    if (_tv_sec < 0 || _tv_usec > USEC_PER_SEC)
        return;

    if (microseconds >= 0) {
        _tv_usec += microseconds % USEC_PER_SEC;
        _tv_sec += microseconds / USEC_PER_SEC;
        if (_tv_usec >= USEC_PER_SEC) {
            _tv_usec -= USEC_PER_SEC;
            _tv_sec++;
        }
    } else {
        microseconds *= -1;
        _tv_usec -= microseconds % USEC_PER_SEC;
        _tv_sec -= microseconds / USEC_PER_SEC;
        if (_tv_usec < 0) {
            _tv_usec += USEC_PER_SEC;
            _tv_sec--;
        }
    }
}

timer::timer() {

}

void timer::set_timeout(int64 microseconds) {
    _expire = time_val::get_current_time();
    _expire.add(microseconds);
}

bool timer::is_timeout() {
    time_val now = time_val::get_current_time();
    if (now.sec() < _expire.sec()) {
        return false;
    } else if (now.sec() == _expire.sec()) {
        if (now.usec() < _expire.usec()) {
            return false;
        }
    }
    return true;
}

void timer::sleep(int64 microseconds) {
    boost::this_thread::sleep(boost::posix_time::microseconds(microseconds));
}

}
