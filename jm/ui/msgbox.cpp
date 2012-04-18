#include "msgbox.hpp"
#include "message.hpp"

namespace JM
{
    namespace UI
    {
        MsgBox::MsgBox(const std::string &title, const std::string &msg)
            : _title(title)
            , _msg(msg)
            , _btns()
        {

        }

        MsgBox::~MsgBox()
        {
            Message::inst().msgBoxHide();
        }

        void MsgBox::addBtns(const StringVector &btns)
        {
            _btns = btns;
        }

        std::string MsgBox::exec()
        {
            Message::inst().msgBoxBtnClr();
            for (StringVector::const_iterator it = _btns.cbegin(); it != _btns.cend(); ++it)
            {
                Message::inst().msgBoxAddBtn(*it);
            }
            Message::inst().msgBoxSetTitle(_title);
            Message::inst().msgBoxSetMsg(_msg);
            Message::inst().msgBoxShow();
            return Message::inst().btnClicked(true);
        }
    }
}