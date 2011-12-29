#include "jm_device_factory.hpp"
#include "jm_v1_commbox.hpp"

namespace jm {
device_factory::device_factory(const commbox_port_ptr &port) {
    _port = port;
}

commbox_ptr device_factory::create(commbox_version ver) {
    switch (ver) {
    case V1:
        if (_obj_map.find(V1) != _obj_map.end()) {
            return _obj_map.find(V1)->second;
        } else {
            commbox_ptr box = boost::make_shared<v1::v1_commbox>(_port);
            _obj_map.insert(std::pair<commbox_version, commbox_ptr >(V1, box));
            return box;
        }
        break;
    }
    return commbox_ptr();
}

}
