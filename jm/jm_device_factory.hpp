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

#include <jm_commbox_version.hpp>
#include <jm_commbox.hpp>
#include <map>
#include <boost/smart_ptr.hpp>

namespace jm {
class device_factory {
public:
    device_factory(const boost::shared_ptr<port> &port);
    boost::shared_ptr<commbox> create(commbox_version ver);
    boost::shared_ptr<commbox> create();
private:
    std::multimap<commbox_version, boost::shared_ptr<commbox> > _obj_map;
    boost::shared_ptr<port> _port;
};
}


#endif	/* JM_DEVICE_FACTORY_HPP */
