/* 
 * File:   jm_log.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月15日, 下午10:56
 */

#ifndef JM_LOG_HPP
#define	JM_LOG_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <jm_types.hpp>
#include <string>

namespace jm {

class log {
public:

    enum level {
#ifdef JM_OS_ANDROID
        unknow = ANDROID_LOG_UNKNOWN,
        defaultt = ANDROID_LOG_DEFAULT,
        verbose = ANDROID_LOG_VERBOSE,
        debug = ANDROID_LOG_DEBUG,
        info = ANDROID_LOG_INFO,
        warn = ANDROID_LOG_WARN,
        error = ANDROID_LOG_ERROR,
        fatal = ANDROID_LOG_FATAL,
        silent = ANDROID_LOG_SILENT
#else
        unknow,
        defaultt,
        verbose,
        debug,
        info,
        warn,
        error,
        fatal,
        silent
#endif
    };

    static log& instance();
    void write(level l, const std::string &tag, const std::string &message);
private:
    log();
};

inline log& log_inst() {
    return log::instance();
}
}

#endif	/* JM_LOG_HPP */

