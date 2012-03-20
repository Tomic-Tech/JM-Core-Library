#ifndef __JM_V1_BOX_H__
#define __JM_V1_BOX_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>
#include "jmv1shared.hpp"
#include "jmv1boxw80c.hpp"
#include "jmv1boxc168c.hpp"

namespace JM
{
    namespace V1
    {
        template<typename CONSTANT>
        class Box
        {
        public:
            typedef CONSTANT C;
        public:
            Box(Shared *shared)
                : _shared(shared)
            {
            }
            virtual ~Box()
            {
            }

            virtual gboolean openComm() = 0;
            virtual gboolean closeComm() = 0;
            virtual size_t readBytes(guint8 *buff, size_t count) = 0;
            virtual gboolean setCommCtrl(guint8 valueOpen, 
                guint8 valueClose) = 0;
            virtual gboolean setCommLine(guint8 sendLine, 
                guint8 recvLine) = 0;
            virtual gboolean setCommLink(guint8 ctrlWord1, guint8 ctrlWord2, 
                guint8 ctrlWord3) = 0;
            virtual gboolean setCommBaud(gdouble baud) = 0;
            virtual gboolean setCommTime(guint8 type, guint32 time) = 0;
            virtual gboolean setLineLevel(guint8 valueLow, 
                guint8 valueHigh) = 0;
            virtual gboolean commboxDelay(guint32 time) = 0;
            virtual gboolean turnOverOneByOne() = 0;
            virtual gboolean stopNow(gboolean isStopExecute) = 0;
            virtual gboolean newBatch(guint8 buffID) = 0;
            virtual gboolean delBatch(guint8 buffID) = 0;
            virtual gboolean checkResult(gint64 microseconds) = 0;
            virtual gboolean keepLink(gboolean isRunLink) = 0;
            virtual gboolean sendOutData(const guint8 *buff, 
                size_t count) = 0;
            virtual gboolean runReceive(guint8 type) = 0;
            virtual gboolean endBatch() = 0;
            virtual gboolean runBatch(guint8 *buffID, size_t count, 
                gboolean isExecuteMany) = 0;
            virtual size_t readData(guint8 *buff, size_t count, 
                gint64 microseconds) = 0;
            virtual gboolean updateBuff(guint8 type, guint8 *buff) = 0;
            virtual guint8 getAbsAdd(guint8 buffID, guint8 add) = 0;
            
        protected:
            Shared *_shared;
        };
    }
}

#endif