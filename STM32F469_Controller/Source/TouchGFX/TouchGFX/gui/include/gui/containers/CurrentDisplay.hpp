#ifndef CURRENTDISPLAY_HPP
#define CURRENTDISPLAY_HPP

#include <gui_generated/containers/CurrentDisplayBase.hpp>

class CurrentDisplay : public CurrentDisplayBase
{
public:
    CurrentDisplay();
    virtual ~CurrentDisplay() {}

    virtual void initialize();
protected:
};

#endif // CURRENTDISPLAY_HPP
