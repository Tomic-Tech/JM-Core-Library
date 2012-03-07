#ifndef __JM_KWP_MODE_H__
#define __JM_KWP_MODE_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>

G_BEGIN_DECLS

typedef enum _JMKWPMode JMKWPMode;

enum _JMKWPMode
{
    JM_KWP8X = 0,
    JM_KWP80 = 1,
    JM_KWPXX = 2,
    JM_KWP00 = 3,
    JM_KWPCX = 4
};

G_END_DECLS

#endif