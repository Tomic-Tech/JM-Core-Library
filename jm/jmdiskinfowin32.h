#ifndef __JM_DISK_INFO_WIN32_H__
#define __JM_DISK_INFO_WIN32_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>

#ifdef G_OS_WIN32

G_BEGIN_DECLS

void jm_disk_info_init_win32(void);
void jm_disk_info_destroy_win32(void);
guint64 jm_disk_info_drive_size_win32(size_t drive);
size_t jm_disk_info_get_count_win32(void);
size_t jm_disk_info_buffer_size_win32(size_t drive);
gchar* jm_disk_info_model_number_win32(size_t drive);
gchar* jm_disk_info_serial_number_win32(size_t drive);
gchar* jm_disk_info_revision_number_win32(size_t drive);
const gchar* jm_disk_info_drive_type(size_t drive);
size_t jm_disk_info_load_win32(void);

G_END_DECLS

#endif

#endif