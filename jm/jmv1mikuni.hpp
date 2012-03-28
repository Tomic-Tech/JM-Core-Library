#ifndef __JM_V1_MIKUNI_HPP__
#define __JM_V1_MIKUNI_HPP__

#include "jmmikuni.hpp"
#include "jmv1default.hpp"

namespace JM
{
    namespace V1
    {
        template<typename BoxType>
        class Mikuni : public JM::Mikuni
        {
        public:
            Mikuni(const boost::shared_ptr<BoxType> &box, const boost::shared_ptr<Shared> &shared)
                : _box(box)
                , _shared(shared)
                , _default()
            {
                _default.reset(new JM::V1::Default<BoxType, JM::V1::Mikuni<BoxType> >(_box, _shared, this));
            }

            ~Mikuni()
            {
            }

            boost::int32_t init()
            {
                boost::uint8_t valueOpen;
                boost::uint8_t valueClose;
                boost::uint8_t sendLine;
                boost::uint8_t recvLine;
                boost::uint8_t ctrlWord1;
                boost::uint8_t ctrlWord2;
                boost::uint8_t ctrlWord3;

                valueOpen = BoxType::Constant::PWC | BoxType::Constant::RZFC | BoxType::Constant::CK | BoxType::Constant::REFC;
                valueClose = BoxType::Constant::SET_NULL;
                sendLine = BoxType::Constant::SK_NO;
                recvLine = BoxType::Constant::RK1;
                ctrlWord1 = BoxType::Constant::RS_232 | BoxType::Constant::BIT9_MARK | BoxType::Constant::SEL_SL | BoxType::Constant::UN_DB20;
                ctrlWord2 = 0xFF;
                ctrlWord3 = 2;

                if (!_box->setCommCtrl(valueOpen, valueClose) ||
                    !_box->setCommLine(sendLine, recvLine) ||
                    !_box->setCommLink(ctrlWord1, ctrlWord2, ctrlWord3) ||
                    !_box->setCommBaud(19200) ||
                    !_box->setCommTime(BoxType::Constant::SETBYTETIME, BoxType::toMicroSeconds(BoxType::MilliSeconds(100))) ||
                    !_box->setCommTime(BoxType::Constant::SETWAITTIME, BoxType::toMicroSeconds(BoxType::MilliSeconds(1))) ||
                    !_box->setCommTime(BoxType::Constant::SETRECBBOUT, BoxType::toMicroSeconds(BoxType::MilliSeconds(400))) ||
                    !_box->setCommTime(BoxType::Constant::SETRECFROUT, BoxType::toMicroSeconds(BoxType::MilliSeconds(500))) ||
                    !_box->setCommTime(BoxType::Constant::SETLINKTIME, BoxType::toMicroSeconds(BoxType::MilliSeconds(500))))
                {
                    return JM_ERROR_GENERIC;
                }

				BoxType::sleep(BoxType::Seconds(1));
                return JM_ERROR_SUCCESS;

            }

            void finishExecute(bool isFinish)
            {
                if (isFinish)
                {
                    _box->stopNow(true);
                    _box->delBatch(_shared->buffID);
                    _box->checkResult(BoxType::toMicroSeconds(BoxType::MilliSeconds(500)));
                }
            }

            std::size_t sendOneFrame(const boost::uint8_t *data, std::size_t count)
            {
                return sendOneFrame(data, count, false);
            }

            std::size_t sendFrames(const boost::uint8_t *data, std::size_t count)
            {
                return sendOneFrame(data, count);
            }

            std::size_t readOneFrame(boost::uint8_t *data, std::size_t maxLength)
            {
                return readOneFrame(data, maxLength, true);
            }

            std::size_t readFrames(boost::uint8_t *data, std::size_t maxLength)
            {
                return readOneFrame(data, maxLength);
            }

            boost::int32_t setKeepLink(const boost::uint8_t *data, std::size_t count)
            {
				boost::array<boost::uint8_t, 256> buff;
                std::size_t length;

                length = pack(data, count, buff.data(), buff.size());

                if (length <= 0)
                    return JM_ERROR_GENERIC;

                return _default->setKeepLink(buff.data(), length);
            }
        private:
            std::size_t sendOneFrame(const boost::uint8_t *data, std::size_t count, bool needRecv)
            {
                return _default->sendOneFrame(data, count, needRecv);
            }

            std::size_t readOneFrame(boost::uint8_t *data, std::size_t maxLength, bool isFinish)
            {
                std::size_t tempLength = 0;
                boost::uint8_t temp[256] = {0};
                boost::uint8_t before = 0;
                boost::uint8_t *p = temp;

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
                    tempLength = unpack(temp, tempLength, data, maxLength);
                }
                else
                {
                    tempLength = 0;
                }
                finishExecute(isFinish);
                return tempLength;
            }


        private:
            boost::shared_ptr<BoxType> _box;
            boost::shared_ptr<Shared> _shared;
            boost::shared_ptr<Default<BoxType, JM::V1::Mikuni<BoxType> > > _default;
        };
    }
}

#endif


