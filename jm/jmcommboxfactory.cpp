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
				box = _objHash[ver].get();
			}
			if (box != NULL)
				break;
			box = new JM::V1::Commbox();
			_objHash[ver].reset(box);
		default:
			break;
		}
		jm_commbox_set_handler((const pointer_t)box);
	}
}