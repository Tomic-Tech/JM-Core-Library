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

		}

		Commbox::~Commbox()
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

		JM::Link* Commbox::configure(JMProtocolType type)
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

		boost::int32_t Commbox::setConnector(JMConnector type)
		{
			_shared->connector = type;
			return JM_ERROR_SUCCESS;
		}


	}
}