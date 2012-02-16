#include "jmv1commbox.h"
#include "jmv1shared.h"
#include "jmv1boxw80.h"
#include "jmv1boxc168.h"
#include "jmv1comm.h"

#define JM_V1_COMMBOX(obj) ((JMV1Commbox*)(obj))

typedef struct _JMV1Commbox JMV1Commbox;

struct _JMV1Commbox {
    guint32 current_box;
    JMV1Shared shared;
    JMV1Box *w80;
    JMV1Box *c168;
    GHashTable *w80_prc_hash;
    GHashTable *c168_prc_hash;
};

static gint32 _jm_v1_commbox_open(JMCommbox *self) {
    JMV1Commbox *v1 = NULL;
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    v1 = (JMV1Commbox*)self->user_data;

    if (v1->current_box == JM_V1_BOX_C168) {
        if (!v1->c168->open_comm(v1->c168)) {
            v1->current_box = JM_V1_BOX_W80;
            return JM_ERROR_COMMBOX_TRY_AGAIN;
        }
    } else {
        if (!v1->w80->open_comm(v1->w80)) {
            v1->current_box = JM_V1_BOX_C168;
            return JM_ERROR_COMMBOX_OPEN_FAIL;
        }
    }
    return JM_ERROR_SUCCESS;
}

static gint32 _jm_v1_commbox_close(JMCommbox *self) {
    JMV1Commbox *v1 = NULL;
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    v1 = (JMV1Commbox*)self->user_data;

    if (v1->current_box == JM_V1_BOX_C168) {
        if (v1->c168->close_comm(v1->c168)) {
            return JM_ERROR_SUCCESS;
        }
    } else {
        if (v1->w80->close_comm(v1->w80)) {
            return JM_ERROR_SUCCESS;
        }
    }
    return JM_ERROR_GENERIC;
}

static JMComm* _jm_v1_commbox_configure(JMCommbox *self, JMProtocolType type) {
    JMV1Commbox *v1 = NULL;
    self->comm = NULL;

    g_return_val_if_fail(self != NULL, NULL);

    v1 = (JMV1Commbox*)self->user_data;

    if (v1->current_box == JM_V1_BOX_W80) {
        self->comm = (JMComm*)g_hash_table_lookup(v1->w80_prc_hash, GINT_TO_POINTER(type));
        if (self->comm == NULL) {
            self->comm = jm_v1_comm_new(type, v1->w80);
            g_hash_table_insert(v1->w80_prc_hash, GINT_TO_POINTER(type), self->comm);
        }
    } else if (v1->current_box == JM_V1_BOX_C168) {
        self->comm = (JMComm*)g_hash_table_lookup(v1->c168_prc_hash, GINT_TO_POINTER(type));
        if (self->comm == NULL) {
            self->comm = jm_v1_comm_new(type, v1->c168);
            g_hash_table_insert(v1->c168_prc_hash, GINT_TO_POINTER(type), self->comm);
        }
    }
    
    return self->comm;
}

static gint32 _jm_v1_commbox_set_connector(JMCommbox *self, JMConnector type) {
    JMV1Commbox *v1 = NULL;
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    v1 = (JMV1Commbox*)self->user_data;

    v1->shared.connector = type;
    return JM_ERROR_SUCCESS;
}

static gint32 _jm_v1_commbox_serial_port_baud(JMCommbox *self) {
    JMV1Commbox *v1 = NULL;
    g_return_val_if_fail(self != NULL, 0);

    v1 = (JMV1Commbox*)self->user_data;

    if (v1->current_box == JM_V1_BOX_C168) {
        return v1->c168->serial_port_baud(v1->c168);
    } else {
        return v1->w80->serial_port_baud(v1->w80);
    }
}

static guint8 _jm_v1_commbox_serial_port_databits(JMCommbox *self) {
    JMV1Commbox *v1 = NULL;
    g_return_val_if_fail(self != NULL, 0);

    v1 = (JMV1Commbox*)self->user_data;

    if (v1->current_box == JM_V1_BOX_C168) {
        return v1->c168->serial_port_databits(v1->c168);
    } else {
        return v1->w80->serial_port_databits(v1->w80);
    }
}

static gint32 _jm_v1_commbox_serial_port_parity(JMCommbox *self) {
    JMV1Commbox *v1 = NULL;
    g_return_val_if_fail(self != NULL, 0);

    v1 = (JMV1Commbox*)self->user_data;

    if (v1->current_box == JM_V1_BOX_C168) {
        return v1->c168->serial_port_parity(v1->c168);
    } else {
        return v1->w80->serial_port_parity(v1->w80);
    }
}

static gint32 _jm_v1_commbox_serial_port_stopbits(JMCommbox *self) {
    JMV1Commbox *v1 = NULL;
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    v1 = (JMV1Commbox*)self->user_data;

    if (v1->current_box == JM_V1_BOX_C168) {
        return v1->c168->serial_port_stopbits(v1->c168);
    } else {
        return v1->w80->serial_port_stopbits(v1->w80);
    }
}

static gint32 _jm_v1_commbox_serial_port_flow_control(JMCommbox *self) {
    JMV1Commbox *v1 = NULL;
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    v1 = (JMV1Commbox*)self->user_data;

    if (v1->current_box == JM_V1_BOX_C168) {
        return v1->c168->serial_port_flow_control(v1->c168);
    } else {
        return v1->w80->serial_port_flow_control(v1->w80);
    }
}

static gboolean _jm_v1_commbox_serial_port_change_config(JMCommbox *self) {
    JMV1Commbox *v1 = NULL;
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    v1 = (JMV1Commbox*)self->user_data;

    if (v1->current_box == JM_V1_BOX_C168) {
        return v1->c168->serial_port_change_config(v1->c168);
    } else {
        return v1->w80->serial_port_change_config(v1->w80);
    }
}

static gboolean _jm_v1_commbox_check_serial_port_change_config(JMCommbox *self) {
    JMV1Commbox *v1 = NULL;
    g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);

    v1 = (JMV1Commbox*)self->user_data;

    if (v1->current_box == JM_V1_BOX_C168) {
        return v1->c168->check_serial_port_change_config(v1->c168);
    } else {
        return v1->w80->check_serial_port_change_config(v1->w80);
    }
}

static void _jm_comm_destroy(gpointer data) {
    JMComm *comm = (JMComm*)data;
    g_return_if_fail(data != NULL);

    jm_v1_comm_free(comm);
}

JMCommbox* jm_v1_commbox_new(void) {
    JMCommbox *obj = jm_commbox_new();
    JMV1Commbox *v1 = (JMV1Commbox*)g_malloc(sizeof(JMV1Commbox));

    obj->open = _jm_v1_commbox_open;
    obj->close = _jm_v1_commbox_close;
    obj->configure = _jm_v1_commbox_configure;
    obj->set_connector = _jm_v1_commbox_set_connector;
    obj->serial_port_baud = _jm_v1_commbox_serial_port_baud;
    obj->serial_port_databits = _jm_v1_commbox_serial_port_databits;
    obj->serial_port_parity = _jm_v1_commbox_serial_port_parity;
    obj->serial_port_stopbits = _jm_v1_commbox_serial_port_stopbits;
    obj->serial_port_flow_control = _jm_v1_commbox_serial_port_flow_control;
    obj->serial_port_change_config = _jm_v1_commbox_serial_port_change_config;
    obj->check_serial_port_change_config = _jm_v1_commbox_check_serial_port_change_config;
    obj->version = JM_COMMBOX_V1;
    obj->user_data = v1;
    v1->current_box = 0;
    v1->shared.buff_id = 0;
    v1->shared.connector = JM_CN_OBDII_16;
    v1->shared.is_db_20 = FALSE;
    v1->shared.is_do_now = TRUE;
    v1->shared.last_error = 0;
    v1->shared.next_address = 0;
    v1->shared.req_byte_to_byte = 0;
    v1->shared.req_wait_time = 0;
    v1->shared.res_byte_to_byte = 0;
    v1->shared.res_wait_time = 0;
    v1->w80_prc_hash = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, _jm_comm_destroy);
    v1->c168_prc_hash = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, _jm_comm_destroy);

    v1->w80 = jm_v1_box_w80_new(&v1->shared);
    v1->c168 = jm_v1_box_c168_new(&v1->shared);


    return obj;
}

void jm_v1_commbox_free(JMCommbox *self) {
    JMV1Commbox *v1 = NULL;
    g_return_if_fail(self != NULL);

    v1 = (JMV1Commbox *)self->user_data;

    jm_v1_box_c168_free(v1->c168);
    jm_v1_box_w80_free(v1->w80);
    g_hash_table_unref(v1->w80_prc_hash);
    g_hash_table_unref(v1->c168_prc_hash);

    g_free(self->user_data);
    self->user_data = NULL;

    jm_commbox_free(self);
}