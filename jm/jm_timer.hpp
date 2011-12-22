/* 
 * File:   jm_timer.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月22日, 下午10:55
 */

#ifndef JM_TIMER_HPP
#define	JM_TIMER_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <jm_types.hpp>
#include <string>

namespace jm {
class time_val {
public:
    static const int32 USEC_PER_SEC = 1000000;
    time_val();
    time_val(const time_val &other);
    time_val& operator=(const time_val &other);
    
    // to_iso_8601:
    // Converts time_val into an ISO 8601 encoded string, relative to the
    // Coordinated Universal Time(UTC).
    //
    // Return value: a newly allocated string containing an ISO 8601 date
    std::string to_iso_8601();
    static time_val get_current_time();
    void set_sec(int64 value);
    void set_usec(int64 value);
    int64 sec();
    int64 usec();
    
    // add
    // @microseonds: number of microseoncs to add to time_val.
    //
    // Adds the given number of microseoncs to time_val. @microseoncs can
    // also be negative to decrease the value of time_val;
    void add(int64 microseconds);
private:
    int64 _tv_sec;
    int64 _tv_usec;
};

class timer {
public:
    timer();
    void set_timeout(int64 microseconds);
    bool is_timeout();
    static void sleep(int64 microseconds);
    static inline int64 to_sec(int64 value) {
        return to_ms(value) * 1000;
    }
    static inline int64 to_ms(int64 value) {
        return value * 1000;
    }
    static inline int64 to_us(int64 value) {
        return value;
    }
private:
    time_val _expire;
};

}

#endif	/* JM_TIMER_HPP */

