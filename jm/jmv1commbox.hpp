#ifndef __JM_V1_COMMBOX_HPP__
#define __JM_V1_COMMBOX_HPP__

#include "jmlib.h"
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
        inline void commboxFree(gpointer data)
        {            
            JM::Link *comm = (JM::Link*)data;
            g_return_if_fail(data != NULL);

            delete comm;
        }

        class Commbox : public JM::Commbox
        {
        public:
            Commbox()
                : _shared()
                , _w80(&_shared)
                , _c168(&_shared)
                , _currentBox(&_c168)
                , _w80PrcHash(g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, commboxFree))
                , _c168PrcHash(g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, commboxFree))
            {

            }

            ~Commbox()
            {
                g_hash_table_unref(_w80PrcHash);
                g_hash_table_unref(_c168PrcHash);
            }

            gint32 open()
            {
                if (_currentBox == &_c168)
                {
                    if (!_c168.openComm())
                    {
                        _currentBox = &_w80;
                        return JM_ERROR_COMMBOX_TRY_AGAIN;
                    }
                }
                else
                {
                    if (!_w80.openComm())
                    {
                        _currentBox = &_c168;
                        return JM_ERROR_COMMBOX_OPEN_FAIL;
                    }
                }
                return JM_ERROR_SUCCESS;
            }

            gint32 close()
            {
                if (_currentBox == &_c168)
                {
                    if (_c168.closeComm())
                    {
                        return JM_ERROR_SUCCESS;
                    }
                }
                else
                {
                    if (_w80.closeComm())
                    {
                        return JM_ERROR_SUCCESS;
                    }
                }
                return JM_ERROR_GENERIC;
            }

            JM::Link* configure(JMProtocolType type)
            {
                JM::Link *comm = NULL;
                if (_currentBox == &_c168)
                {
                    comm = (JM::Link*)g_hash_table_lookup(_c168PrcHash, GINT_TO_POINTER(type));
                    if (comm == NULL)
                    {
                        comm = makeProtocol(&_c168, type);
                        g_hash_table_insert(_c168PrcHash, GINT_TO_POINTER(type), comm);
                    }
                }
                else if (_currentBox == &_w80)
                {
                    comm = (JM::Link*)g_hash_table_lookup(_w80PrcHash, GINT_TO_POINTER(type));
                    if (comm == NULL)
                    {
                        comm = makeProtocol(&_c168, type);
                        g_hash_table_insert(_w80PrcHash, GINT_TO_POINTER(type), comm);
                    }
                }
                return comm;
            }

            gint32 setConnector(JMConnector type)
            {
                _shared.connector = type;
                return JM_ERROR_SUCCESS;
            }

        private:
            template<typename BOX>
            JM::Link* makeProtocol(BOX *box, JMProtocolType type)
            {
                JM::Link *ret = NULL;
                switch(type)
                {
                case JM_PRC_ISO14230:
                    ret = new JM::V1::Link<BOX, JM::V1::ISO14230<BOX> >(box, &_shared);
                    jm_kwp2000_set_handler(ret->protocol());
                    break;
                case JM_PRC_ISO15765:
                    ret = new JM::V1::Link<BOX, JM::V1::ISO15765<BOX> >(box, &_shared);
                    jm_canbus_set_handler(ret->protocol());
                    break;
                case JM_PRC_KWP1281:
                    ret = new JM::V1::KWP1281Link<BOX>(box, &_shared);
                    jm_kwp1281_set_handler(ret->protocol());
                    break;
                case JM_PRC_MIKUNI:
                    ret = new JM::V1::Link<BOX, JM::V1::Mikuni<BOX> >(box, &_shared);
                    jm_mikuni_set_handler(ret->protocol());
                    break;
                default:
                    break;
                }
                return ret;
            }
        private:
            Shared _shared;
            JM::V1::W80::Box _w80;
            JM::V1::C168::Box _c168;
            gpointer _currentBox;
            GHashTable *_w80PrcHash;
            GHashTable *_c168PrcHash;
        };
    }
}

#endif

