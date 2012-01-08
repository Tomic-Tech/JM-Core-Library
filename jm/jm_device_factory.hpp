/* 
 * File:   jm_device_factory.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月25日, 下午1:33
 */

#ifndef JM_DEVICE_FACTORY_HPP
#define	JM_DEVICE_FACTORY_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <map>
#include <boost/smart_ptr.hpp>
#include <jm/jm_commbox_version.hpp>
#include <jm/jm_commbox.hpp>

namespace jm {
class JMCORE_API device_factory {
public:
    device_factory(const commbox_port_ptr &port);
    boost::shared_ptr<commbox> create(commbox_version ver);
    boost::shared_ptr<commbox> create();
private:
    std::multimap<commbox_version, commbox_ptr > _obj_map;
    commbox_port_ptr _port;
};
}


#endif	/* JM_DEVICE_FACTORY_HPP */

