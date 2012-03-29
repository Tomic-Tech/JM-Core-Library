#ifndef __JM_COMMBOX_FACTORY_HPP__
#define __JM_COMMBOX_FACTORY_HPP__

#include <jm/jmcommboxversion.h>
#include <hash_map>
#include <boost/shared_ptr.hpp>
#include <jm/jmcommbox.hpp>

namespace JM
{
    class CommboxFactory
    {
    public:
        static CommboxFactory& inst();
        void setVersion(JMCommboxVersion ver);
    private:
        CommboxFactory();
        ~CommboxFactory();
    private:
		std::hash_map<JMCommboxVersion, boost::shared_ptr<JM::Commbox> > _objHash;
    };
}

#endif



