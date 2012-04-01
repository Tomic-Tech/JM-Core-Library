#include "jmv1commbox.hpp"

namespace JM
{
	namespace V1
	{
		Commbox::Commbox()
			: _shared(new Shared())
			, _w80(new JM::V1::W80::Box(_shared))
			, _c168(new JM::V1::C168::Box(_shared))
			, _currentBox(_c168.get())
			, _w80PrcHash()
			, _c168PrcHash()
		{
			_shared->buffID = 0;
			_shared->connector = JM_CN_UNKNOW;
			_shared->isDB20 = false;
			_shared->isDoNow = true;
			_shared->lastError = 0;
			_shared->nextAddress = 0;
			_shared->reqByteToByte = 500000;
			_shared->reqWaitTime = 500000;
			_shared->resByteToByte = 500000;
			_shared->resWaitTime = 500000;
		}

		Commbox::~Commbox()
		{
		}

		boost::int32_t Commbox::open()
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

		boost::int32_t Commbox::close()
		{
			if (_currentBox == _c168.get())
			{
				if (jm_link_protocol_type() == JM_PRC_ISO15765)
				{
					((JM::V1::ISO15765<JM::V1::C168::Box>*)jm_link_get_protocol())->reset();
				}

				if (_c168->closeComm())
				{
					return JM_ERROR_SUCCESS;
				}
			}
			else
			{
				if (jm_link_protocol_type() == JM_PRC_ISO15765)
				{
					((JM::V1::ISO15765<JM::V1::C168::Box>*)jm_link_get_protocol())->reset();
				}
				if (_w80->closeComm())
				{
					return JM_ERROR_SUCCESS;
				}
			}
			return JM_ERROR_GENERIC;
		}

		JM::Link* Commbox::configure(JMProtocolType type)
		{
			JM::Link *comm = NULL;
			if (_currentBox == _c168.get())
			{
				if (_c168PrcHash.find(type) != _c168PrcHash.end())
				{
					comm = _c168PrcHash[type].get();
				}
				if (comm == NULL)
				{
					_c168PrcHash[type] = makeProtocol(_c168, type);
					comm = _c168PrcHash[type].get();
				}
			}
			else if (_currentBox == _w80.get())
			{
				if (_w80PrcHash.find(type) != _w80PrcHash.end())
				{
					comm = _w80PrcHash[type].get();
				}
				if (comm == NULL)
				{
					_w80PrcHash[type] = makeProtocol(_w80, type);
					comm = _w80PrcHash[type].get();
				}
			}
			return comm;
		}

		boost::int32_t Commbox::setConnector(JMConnector type)
		{
			_shared->connector = type;
			return JM_ERROR_SUCCESS;
		}


	}
}