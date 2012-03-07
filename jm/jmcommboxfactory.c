#include "jmcommboxfactory.h"
#include "jmv1commbox.h"

static GHashTable *_jm_commbox_factory_obj_hash;
static JMCommboxVersion _jm_commbox_factory_ver;

static void _jm_commbox_destroy(gpointer data)
{
    JMCommbox *box = NULL;
    g_return_if_fail(data != NULL);
    box = (JMCommbox*)data;
    if (box->version == JM_COMMBOX_V1)
    {
        jm_v1_commbox_free(box);
    }
}

void jm_commbox_factory_init(void)
{
    _jm_commbox_factory_obj_hash = g_hash_table_new_full(g_direct_hash, 
        g_direct_equal, NULL, _jm_commbox_destroy);
}

void jm_commbox_factory_destroy(void)
{
    g_hash_table_unref(_jm_commbox_factory_obj_hash);
}

void jm_commbox_factory_set_commbox_version(JMCommboxVersion ver)
{
    _jm_commbox_factory_ver = ver;
}

JMCommbox* jm_commbox_factory_create_commbox()
{
    JMCommbox *box = NULL;
    switch(_jm_commbox_factory_ver)
    {
    case JM_COMMBOX_V1:
        box = (JMCommbox*)g_hash_table_lookup(_jm_commbox_factory_obj_hash, 
            GINT_TO_POINTER(_jm_commbox_factory_ver));
        if (box != NULL)
            break;

        box = jm_v1_commbox_new();
        g_hash_table_insert(_jm_commbox_factory_obj_hash, 
            GINT_TO_POINTER(_jm_commbox_factory_ver), box);
        break;
    }
    return box;
}