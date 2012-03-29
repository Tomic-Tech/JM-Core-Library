#include "jmcommboxfactory.hpp"
#include "jmlib.h"
#include "jmv1commbox.hpp"

namespace JM
{
	CommboxFactory::CommboxFactory()
		: _objHash()
	{

	}
	CommboxFactory::~CommboxFactory()
	{
		for (std::hash_map<JMCommboxVersion, JM::Commbox*>::iterator it = _objHash.begin(); it != _objHash.end(); it++)
		{
			delete it->second;
		}
	}

	CommboxFactory& CommboxFactory::inst()
	{
		static CommboxFactory instance;
		return instance;
	}

	void CommboxFactory::setVersion(JMCommboxVersion ver)
	{
		JM::Commbox *box = NULL;
		switch(ver)
		{
		case JM_COMMBOX_V1:
			if (_objHash.find(ver) != _objHash.end())
			{
				box = _objHash[ver];
			}
			if (box != NULL)
				break;
			box = new JM::V1::Commbox();
			_objHash[ver] = box;
		default:
			break;
		}
		jm_commbox_set_handler((pointer_t)box);
	}
}