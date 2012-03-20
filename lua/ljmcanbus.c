#include "ljmcanbus.h"

LuaJMCanbusBaud lua_jm_canbus_baud[16] = {
    {"1000K", JM_CANBUS_B1000K},
    {"800K", JM_CANBUS_B800K},
    {"666K", JM_CANBUS_B666K},
    {"500K", JM_CANBUS_B500K},
    {"400K", JM_CANBUS_B400K},
    {"250K", JM_CANBUS_B250K},
    {"200K", JM_CANBUS_B200K},
    {"125K", JM_CANBUS_B125K},
    {"100K", JM_CANBUS_B100K},
    {"80K", JM_CANBUS_B80K},
    {"50K", JM_CANBUS_B50K},
    {"40K", JM_CANBUS_B40K},
    {"20K", JM_CANBUS_B20K},
    {"10K", JM_CANBUS_B10K},
    {"5K", JM_CANBUS_B5K},
    {NULL, (JMCanbusBaud)0xFF}
};

LuaJMCanbusIDMode lua_jm_canbus_id_mode[3] = {
    {"Standard", JM_CANBUS_ID_MODE_STD},
    {"Extend", JM_CANBUS_ID_MODE_EXT},
    {NULL, (JMCanbusIDMode)(0xFF)}
};

LuaJMCanbusFrameType lua_jm_canbus_frame_type[3] = {
    {"Data", JM_CANBUS_FRAME_TYPE_DATA},
    {"Remote", JM_CANBUS_FRAME_TYPE_REMOTE},
    {NULL, (JMCanbusFrameType)(0xFF)}
};

LuaJMCanbusFilterMask lua_jm_canbus_filter_mask[3] = {
    {"Enable", JM_CANBUS_FILTER_MASK_ENABLE},
    {"Disable", JM_CANBUS_FILTER_MASK_DISABLE},
    {NULL, (JMCanbusFilterMask)(0xFF)}
};