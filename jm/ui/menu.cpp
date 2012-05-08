#include "menu.h"
#include "message.h"

namespace JM
{
namespace UI
{

Menu::Menu()
: _strVec()
{

}

Menu::~Menu()
{
    
}

std::string Menu::selected()
{
    std::string btn = Message::inst().btn_clicked(true);
    if (btn.compare(".") == 0)
        return Message::inst().menu_selected();
    else
        return std::string();
}

void Menu::add_items(const StringVector& strVec)
{
    _strVec = strVec;
}

void Menu::operator() ()
{
    do
    {
        Message::inst().msg_box_hide();
        Message::inst().menu_item_clr();
        Message::inst().menu_add_items(_strVec);
        Message::inst().menu_show();
    } while (selectEvent(selected()));
}

}
}
