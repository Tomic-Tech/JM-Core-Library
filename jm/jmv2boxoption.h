#ifndef __JM_V2_BOX_OPTION_H__
#define __JM_V2_BOX_OPTION_H__

#include <glib.h>
#include "jmcanbusbaud.h"
#include "jmcanbusidmode.h"
#include "jmcanbusfiltermask.h"

G_BEGIN_DECLS

#define JM_V2_BOX_STATUS 0xFF
#define JM_V2_BOX_CONFIG_SUCCESS 0x00
#define JM_V2_BOX_SET_COMMBOX 0x50
#define JM_V2_BOX_GET_MCU_VER 0x00
#define JM_V2_BOX_SET_ECU_COM 0x01
#define JM_V2_BOX_SET_KWP_MODE 0x01
#define JM_V2_BOX_SET_CAN_MODE 0x03
#define JM_V2_BOX_SET_MAZDA_MODE 0x04
#define JM_V2_BOX_SET_DCHECK_MODE 0x05
#define JM_V2_BOX_SET_ECU_TIMEOUT 0x02
#define JM_V2_BOX_SET_QUICK_KWP 0x03
#define JM_V2_BOX_SET_KEEPLINK 0x04
#define JM_V2_BOX_SET_CANID_AFRM 0x05
#define JM_V2_BOX_VERSION_LENGTH 0x0A

gboolean jm_v2_box_is_config_success(void);
gboolean jm_v2_box_read_version(void);
guint8* jm_v2_box_get_version(void);
gboolean jm_v2_box_set_can_mode(gint32 h_line, gint32 l_line,
    JMCanbusBaud baud);
gboolean jm_v2_box_set_can_afmr(JMCanbusIDMode id_mode, 
    JMCanbusFilterMask mask, size_t id_count,
    const guint32 *id_buffer);
gboolean jm_v2_box_set_kwp_mode(gint32 com_line, guint32 baud, 
    gboolean l_line);
gboolean jM_v2_box_set_kep_signal_time(guint8 low, guint8 high);
G_END_DECLS

#endif