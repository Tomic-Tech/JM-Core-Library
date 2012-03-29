#ifndef __JM_V1_COMMBOX_HPP__
#define __JM_V1_COMMBOX_HPP__

#include "jmlib.h"
#include <hash_map>
#include "jmcommbox.hpp"
#include "jmv1shared.hpp"
#include "jmv1box.hpp"
#include "jmv1boxw80.hpp"
#include "jmv1boxc168.hpp"
#include "jmv1link.hpp"
#include "jmv1iso14230.hpp"
#include "jmv1iso15765.hpp"
#include "jmv1kwp1281.hpp"
#include "jmv1kwp1281link.hpp"
#include "jmv1mikuni.hpp"

namespace JM
{
    namespace V1
    {
        class Commbox : public JM::Commbox
        {
        public:
            Commbox();
            ~Commbox();
            boost::int32_t open();
            boost::int32_t close();
            JM::Link* configure(JMProtocolType type);
            boost::int32_t setConnector(JMConnector type);

        private:
            template<typename BoxType>
            boost::shared_ptr<JM::Link> makeProtocol(const boost::shared_ptr<BoxType> &box, JMProtocolType type)
            {
                boost::shared_ptr<JM::Link> ret;
                switch(type)
                {
                case JM_PRC_ISO14230:
                    ret.reset(new JM::V1::Link<BoxType, JM::V1::ISO14230<BoxType> >(box, _shared));
                    jm_kwp2000_set_handler(ret->protocol());
                    break;
                case JM_PRC_ISO15765:
                    ret.reset(new JM::V1::Link<BoxType, JM::V1::ISO15765<BoxType> >(box, _shared));
                    jm_canbus_set_handler(ret->protocol());
                    break;
                case JM_PRC_KWP1281:
                    ret.reset(new JM::V1::KWP1281Link<BoxType>(box, _shared));
                    jm_kwp1281_set_handler(ret->protocol());
                    break;
                case JM_PRC_MIKUNI:
                    ret.reset(new JM::V1::Link<BoxType, JM::V1::Mikuni<BoxType> >(box, _shared));
                    jm_mikuni_set_handler(ret->protocol());
                    break;
                default:
                    break;
                }
                return ret;
            }
        private:
            boost::shared_ptr<Shared> _shared;
            boost::shared_ptr<JM::V1::W80::Box> _w80;
            boost::shared_ptr<JM::V1::C168::Box> _c168;
            pointer_t _currentBox;
			std::hash_map<JMProtocolType, boost::shared_ptr<JM::Link> > _w80PrcHash;
			std::hash_map<JMProtocolType, boost::shared_ptr<JM::Link> > _c168PrcHash;
        };
    }
}

#endif

