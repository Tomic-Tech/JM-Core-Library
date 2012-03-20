#ifndef __JM_COMMBOX_FACTORY_HPP__
#define __JM_COMMBOX_FACTORY_HPP__
#include "jmv1commbox.hpp"

namespace JM
{
    static void _commboxFree(gpointer data)
    {
        if (data != NULL)
            delete data;
    }

    class CommboxFactory
    {
    public:
        static CommboxFactory& inst()
        {
            static CommboxFactory instance;
            return instance;
        }

        void setVersion(JMCommboxVersion ver)
        {
            JM::Commbox *box = NULL;
            switch(ver)
            {
            case JM_COMMBOX_V1:
                box = (JM::Commbox*)g_hash_table_lookup(_objHash, GINT_TO_POINTER(ver));
                if (box != NULL)
                    break;
                box = new JM::V1::Commbox();
                g_hash_table_insert(_objHash, GINT_TO_POINTER(ver), box);
            default:
                break;
            }
            jm_commbox_set_handler((gpointer)box);
        }

    private:
        CommboxFactory()
        {
            _objHash = g_hash_table_new_full(g_direct_hash, 
                g_direct_equal, NULL, _commboxFree);
        }
        ~CommboxFactory()
        {
            g_hash_table_unref(_objHash);
        }

    private:
        GHashTable *_objHash;
    };
}

#endif



