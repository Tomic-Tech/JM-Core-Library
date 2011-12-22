/* 
 * File:   jm_log.cpp
 * Author: Ogilvy
 * 
 * Created on 2011年12月15日, 下午10:56
 */

#include "jm_log.hpp"

namespace jm {

log::log() {
    
}

log& log::instance() {
    static log instance;
    return instance;
}

void log::write(level l, const std::string &tag, const std::string &message) {
#ifdef JM_OS_ANDROID
    int priod = l;
    __android_log_write(priod, tag.c_str(), message.c_str());
#endif
}

}

