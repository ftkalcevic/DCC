#include <gui/containers/StatusBar.hpp>

StatusBar::StatusBar()
{
    textAreaStatusBar.setWildcard((uint16_t *)u"Status bar...");
}

void StatusBar::initialize()
{
    StatusBarBase::initialize();
}
