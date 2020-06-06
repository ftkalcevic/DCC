#ifndef CURRENTDISPLAY_HPP
#define CURRENTDISPLAY_HPP

#include <gui_generated/containers/CurrentDisplayBase.hpp>
#include <touchgfx/widgets/canvas/PainterRGB888.hpp>

#include "CurrentGraphWidget.h"

class CurrentDisplay : public CurrentDisplayBase
{
public:
    CurrentDisplay();
    virtual ~CurrentDisplay() {}

    virtual void initialize();
	
    void Update(uint16_t mainTrackCurrent, uint16_t progTrackCurrent);	
protected:
	CurrentGraphWidget graph;
};

#endif // CURRENTDISPLAY_HPP
