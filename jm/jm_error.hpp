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

#include <jm_types.hpp>

namespace jm {

namespace error {

const error_code success = 0;
const error_code generic_error = -1;
const error_code commbox_open_fail = -2;
const error_code commbox_try_again = -3;

}

}


#endif	/* JM_COMMBOX_ERROR_HPP */

