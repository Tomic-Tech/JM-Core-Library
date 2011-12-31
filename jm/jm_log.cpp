/* 
 * File:   jm_log.cpp
 * Author: Ogilvy
 * 
 * Created on 2011年12月15日, 下午10:56
 */

#include "jm_log.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

namespace jm {

log::log() {

}

log& log::instance() {
    static log instance;
    return instance;
}

void log::write_level(level l) {
    switch (l) {
    case unknow:
        std::cout << "Unknow: ";
        break;
    case defaultt:
        std::cout << "default: ";
        break;
    case verbose:
        std::cout << "verbose: ";
        break;
    case debug:
        std::cout << "debug: ";
        break;
    case info:
        std::cout << "info: ";
        break;
    case warn:
        std::cout << "warn: ";
        break;
    case error:
        std::cout << "error: ";
        break;
    case fatal:
        std::cout << "fatal: ";
    case silent:
        std::cout << "silent: ";
        break;
    }

}

void log::write(level l, const std::string &tag, const std::string &message) {
#ifdef JM_OS_ANDROID
    int priod = l;
    __android_log_write(priod, tag.c_str(), message.c_str());
#else
    write_level(l);
    std::cout << tag << "::" << message << std::endl;
#endif
}

void log::write_hex(level l, const std::string &tag, const uint8 *data, size_t offset, size_t count) {
    std::stringstream ss;
    for (size_t pos = offset; pos < (offset + count); pos++) {
        ss << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<uint32>(data[pos]);
        ss << " ";
    }
    write(l, tag, ss.str());
}
}

