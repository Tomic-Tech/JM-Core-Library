/* 
 * File:   jm_commbox_error.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月25日, 下午1:59
 */

#ifndef JM_COMMBOX_ERROR_HPP
#define	JM_COMMBOX_ERROR_HPP

#ifdef _MSC_VER
#pragma once
#endif

namespace jm {

enum commbox_error {
    commbox_success = 0,
    commbox_open_fail = -1,
    commbox_try_again = -2
};

}


#endif	/* JM_COMMBOX_ERROR_HPP */

