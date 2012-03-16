#include "jmlib.h"
#include "jmsys.h"

void jm_lib_init(const gchar *software_path)
{
    g_thread_init(NULL);
    jm_log_init();
    jm_sys_set_software_path(software_path);
    jm_auth_init();
    jm_auth_set_dat_path(software_path);
    jm_db_init();
    jm_commbox_port_init();
    jm_commbox_factory_init();
    jm_ld_array_init();
    jm_ui_init();
    jm_sys_init();
	jm_vehicle_init();
	jm_vehicle_set_path(software_path);
}

void jm_lib_destroy(void)
{
	jm_vehicle_destroy();
    jm_sys_destroy();
    jm_ui_destroy();
    jm_ld_array_destroy();
    jm_commbox_factory_destroy();
    jm_commbox_port_destroy();
    jm_db_init();
    jm_auth_destroy();
    jm_log_destroy();
}
