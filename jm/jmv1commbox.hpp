#ifndef __JM_V1_COMMBOX_HPP__
#define __JM_V1_COMMBOX_HPP__

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
            Commbox()
                : _shared(new Shared())
                , _w80(new JM::V1::W80::Box(_shared))
                , _c168(new JM::V1::C168::Box(_shared))
                , _currentBox(_c168.get())
                , _w80PrcHash()
                , _c168PrcHash()
            {

            }

            ~Commbox()
            {
				for (std::hash_map<JMProtocolType, JM::Link*>::iterator it = _w80PrcHash.begin();
					it != _w80PrcHash.end(); ++it)
				{
					delete it->second;
				}

				for (std::hash_map<JMProtocolType, JM::Link*>::iterator it = _c168PrcHash.begin();
					it != _c168PrcHash.end(); ++it)
				{
					delete it->second;
				}
            }

            boost::int32_t open()
            {
				while (true)
				{
					if (_currentBox == _c168.get())
					{
						if (!_c168->openComm())
						{
							_currentBox = _w80.get();
							_c168->closeComm();
							continue;
						}
						break;
					}
					else
					{
						if (!_w80->openComm())
						{
							_currentBox = _c168.get();
							_w80->closeComm();
							return JM_ERROR_COMMBOX_OPEN_FAIL;
						}
						break;
					}
				}
				return JM_ERROR_SUCCESS;
            }

            boost::int32_t close()
            {
                if (_currentBox == _c168.get())
                {
                    if (_c168->closeComm())
                    {
                        return JM_ERROR_SUCCESS;
                    }
                }
                else
                {
                    if (_w80->closeComm())
                    {
                        return JM_ERROR_SUCCESS;
                    }
                }
                return JM_ERROR_GENERIC;
            }

            JM::Link* configure(JMProtocolType type)
            {
                JM::Link *comm = NULL;
                if (_currentBox == _c168.get())
                {
					if (_c168PrcHash.find(type) != _c168PrcHash.end())
					{
						comm = _c168PrcHash[type];
					}
                    if (comm == NULL)
                    {
                        comm = makeProtocol(_c168, type);
						_c168PrcHash[type] = comm;
                    }
                }
                else if (_currentBox == _w80.get())
                {
					if (_w80PrcHash.find(type) != _w80PrcHash.end())
					{
						comm = _w80PrcHash[type];
					}
					if (comm == NULL)
					{
						comm = makeProtocol(_w80, type);
						_w80PrcHash[type] = comm;
					}
                }
                return comm;
            }

            boost::int32_t setConnector(JMConnector type)
            {
                _shared->connector = type;
                return JM_ERROR_SUCCESS;
            }

        private:
            template<typename BoxType>
            JM::Link* makeProtocol(const boost::shared_ptr<BoxType> &box, JMProtocolType type)
            {
                JM::Link *ret = NULL;
                switch(type)
                {
                case JM_PRC_ISO14230:
                    ret = new JM::V1::Link<BoxType, JM::V1::ISO14230<BoxType> >(box, _shared);
                    jm_kwp2000_set_handler(ret->protocol());
                    break;
                case JM_PRC_ISO15765:
                    ret = new JM::V1::Link<BoxType, JM::V1::ISO15765<BoxType> >(box, _shared);
                    jm_canbus_set_handler(ret->protocol());
                    break;
                case JM_PRC_KWP1281:
                    ret = new JM::V1::KWP1281Link<BoxType>(box, _shared);
                    jm_kwp1281_set_handler(ret->protocol());
                    break;
                case JM_PRC_MIKUNI:
                    ret = new JM::V1::Link<BoxType, JM::V1::Mikuni<BoxType> >(box, _shared);
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
            void* _currentBox;
			std::hash_map<JMProtocolType, JM::Link*> _w80PrcHash;
			std::hash_map<JMProtocolType, JM::Link*> _c168PrcHash;
        };
    }
}

#endif

