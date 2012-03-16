#ifndef __JM_V2_BOX_LINK_H__
#define __JM_V2_BOX_LINK_H__

#include <glib.h>

G_BEGIN_DECLS

#define JM_V2_BOX_HEADER_1 0x55
#define JM_V2_BOX_HEADER_2 0xAA

gboolean jm_v2_box_link_open(void);
void jm_v2_box_link_close(void);
gboolean jm_v2_box_link_is_connected(void);
size_t jm_v2_box_link_send(size_t length);
size_t jm_v2_box_link_read(gint64 microseconds);
void jm_v2_box_link_reset(void);

G_END_DECLS

#endif