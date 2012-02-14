#include "ljmkwpmode.h"

LuaJMKWPMode lua_jm_kwp_mode[6] = {
    {"8X", JM_KWP8X},
    {"80", JM_KWP80},
    {"XX", JM_KWPXX},
    {"00", JM_KWP00},
    {"CX", JM_KWPCX},
    {NULL, (JMKWPMode)(0)}
};