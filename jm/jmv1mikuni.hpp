#ifndef __JM_V1_MIKUNI_HPP__
#define __JM_V1_MIKUNI_HPP__

#include "jmtimer.h"
#include "jmmikuni.hpp"
#include "jmv1default.hpp"

namespace JM
{
    namespace V1
    {
        template<typename BOX>
        class Mikuni : public JM::Mikuni
        {
        public:
            Mikuni(BOX *box, Shared *shared)
                : _box(box)
                , _shared(shared)
                , _default(NULL)
            {
                _default = new JM::V1::Default<BOX, JM::V1::Mikuni<BOX> >(_box, _shared, this);
            }

            ~Mikuni()
            {
                delete _default;
            }

            gint32 init()
            {
                guint8 valueOpen;
                guint8 valueClose;
                guint8 sendLine;
                guint8 recvLine;
                guint8 ctrlWord1;
                guint8 ctrlWord2;
                guint8 ctrlWord3;

                valueOpen = BOX::C::PWC | BOX::C::RZFC | BOX::C::CK | BOX::C::REFC;
                valueClose = BOX::C::SET_NULL;
                sendLine = BOX::C::SK_NO;
                recvLine = BOX::C::RK1;
                ctrlWord1 = BOX::C::RS_232 | BOX::C::BIT9_MARK | BOX::C::SEL_SL | BOX::C::UN_DB20;
                ctrlWord2 = 0xFF;
                ctrlWord3 = 2;

                if (!_box->setCommCtrl(valueOpen, valueClose) ||
                    !_box->setCommLine(sendLine, recvLine) ||
                    !_box->setCommLink(ctrlWord1, ctrlWord2, ctrlWord3) ||
                    !_box->setCommBaud(19200) ||
                    !_box->setCommTime(BOX::C::SETBYTETIME, JM_TIMER_TO_MS(100)) ||
                    !_box->setCommTime(BOX::C::SETWAITTIME, JM_TIMER_TO_MS(1)) ||
                    !_box->setCommTime(BOX::C::SETRECBBOUT, JM_TIMER_TO_MS(400)) ||
                    !_box->setCommTime(BOX::C::SETRECFROUT, JM_TIMER_TO_MS(500)) ||
                    !_box->setCommTime(BOX::C::SETLINKTIME, JM_TIMER_TO_MS(500)))
                {
                    return JM_ERROR_GENERIC;
                }

                g_usleep(JM_TIMER_TO_SEC(1));
                return JM_ERROR_SUCCESS;

            }

            void finishExecute(gboolean isFinish)
            {
                if (isFinish)
                {
                    _box->stopNow(TRUE);
                    _box->delBatch(_shared->buffID);
                    _box->checkResult(JM_TIMER_TO_MS(500));
                }
            }

            size_t sendOneFrame(const guint8 *data, size_t count)
            {
                return sendOneFrame(data, count, FALSE);
            }

            size_t sendFrames(const guint8 *data, size_t count)
            {
                return sendOneFrame(data, count);
            }

            size_t readOneFrame(guint8 *data)
            {
                return readOneFrame(data, TRUE);
            }

            size_t readFrames(guint8 *data)
            {
                return readOneFrame(data);
            }

            gint32 setKeepLink(const guint8 *data, size_t count)
            {
                guint8 buff[256];
                size_t length;

                length = pack(data, count, buff);

                if (length <= 0)
                    return JM_ERROR_GENERIC;

                return _default->setKeepLink(buff, length);
            }
        private:
            size_t sendOneFrame(const guint8 *data, size_t count, gboolean needRecv)
            {
                return _default->sendOneFrame(data, count, needRecv);
            }

            size_t readOneFrame(guint8 *data, gboolean isFinish)
            {
                size_t tempLength = 0;
                guint8 temp[256] = {0};
                guint8 before = 0;
                guint8 *p = temp;

                while (_box->readBytes(p++, 1) == 1)
                {
                    tempLength++;
                    if (before == 0x0D && (*p) == 0x0A)
                    {
                        break;
                    }
                    before = *p;
                }

                if (before == 0x0D && *p == 0x0A)
                {
                    // break normal
                    tempLength = unpack(temp, tempLength, data);
                }
                else
                {
                    tempLength = 0;
                }
                finishExecute(isFinish);
                return tempLength;
            }


        private:
            BOX *_box;
            Shared *_shared;
            Default<BOX, JM::V1::Mikuni<BOX> > *_default;
        };
    }
}

#endif


