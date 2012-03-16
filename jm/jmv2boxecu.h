#ifndef __JM_V2_BOX_ECU_H__
#define __JM_V2_BOX_ECU_H__

#include <glib.h>

G_BEGIN_DECLS

#define JM_V2_BOX_SEND_TO_ECU 0x60
#define JM_V2_BOX_ENTER_5BPS 0x01
#define JM_V2_BOX_QUICK_KWP 0x02
#define JM_V2_BOX_K_SEND 0x03
#define JM_V2_BOX_CAN_SEND 0x04
#define JM_V2_BOX_CAN_SEND_MULTI 0x05
#define JM_V2_BOX_K_REQ_NEXT 0x06
#define JM_V2_BOX_ECU_DATA 0x00

size_t jm_v2_box_ecu_send(guint8 type, size_t count);
size_t jm_v2_box_ecu_read(gint64 microseconds);
size_t jm_v2_box_ecu_kwp_req_next_msg(gint64 microseconds);

G_END_DECLS

#endif