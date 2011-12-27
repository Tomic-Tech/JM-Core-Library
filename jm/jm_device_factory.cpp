#include "jm_device_factory.hpp"
#include "jm_v1_commbox.hpp"

namespace jm {
device_factory::device_factory(const boost::shared_ptr<port> &port) {
    _port = port;
}

boost::shared_ptr<commbox> device_factory::create(commbox_version ver) {
    switch (ver) {
    case V1:
        if (_obj_map.find(V1) != _obj_map.end()) {
            return _obj_map.find(V1)->second;
        } else {
            boost::shared_ptr<commbox> box = boost::make_shared<v1_commbox>(_port);
            _obj_map.insert(std::pair<commbox_version, boost::shared_ptr<commbox> >(V1, box));
            return box;
        }
        break;
    }
    return boost::shared_ptr<commbox>();
}

}
