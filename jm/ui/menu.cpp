#include "menu.hpp"
#include "message.hpp"

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
    std::string btn = Message::inst().btnClicked(true);
    if (btn.compare(".") == 0)
        return Message::inst().menuSelected();
    else
        return std::string();
}

void Menu::addItems(const StringVector &strVec)
{
    _strVec = strVec;
}

void Menu::operator()()
{
    do 
    {
        Message::inst().msgBoxHide();
        Message::inst().menuItemClr();Message::inst().menuAddItems(_strVec);
        Message::inst().menuShow();
    } while (selectEvent(selected()));
}

}
}