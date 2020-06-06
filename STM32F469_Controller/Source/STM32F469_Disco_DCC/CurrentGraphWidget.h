#pragma once

#include <touchgfx/widgets/Widget.hpp>
#include <touchgfx/Color.hpp>
#include "CircularBuffer.h"
#include "PaintContext.h"
#include <touchgfx/Color.hpp>

class CurrentGraphWidget : public Widget
{
	#define SAMPLE_TIME		60
public:
	CurrentGraphWidget()
	{
	}
    void Update(uint16_t main, uint16_t prog)
	{
		mainTrackCurrent.Add(main);
		progTrackCurrent.Add(prog);
		invalidate();
	}
	
protected:
	CircularBuffer<uint16_t,SAMPLE_TIME> mainTrackCurrent;
	CircularBuffer<uint16_t,SAMPLE_TIME> progTrackCurrent;

	virtual void draw(const Rect & invalidatedArea) const
  	{
	  	PaintContext p(this,invalidatedArea);
	  	p.setColor(Color::getColorFrom24BitRGB(0, 0, 0));
	  	p.clear();
	  	p.setColor(Color::getColorFrom24BitRGB(255, 255, 255));
		p.moveTo(0, 0);
		p.lineTo(0, getHeight()-1);
		p.lineTo(getWidth()-1, getHeight()-1);
		if (mainTrackCurrent.Count() > 1)
		{
			int maxI=0;
			for (int i = 0; i < mainTrackCurrent.Count(); i++)
			{
				if (mainTrackCurrent[i] > maxI)
					maxI = mainTrackCurrent[i];
				if (progTrackCurrent[i] > maxI)
					maxI = progTrackCurrent[i];
			}

			// round maxI to nearest 1000
			maxI = (maxI/1000+1)*1000;

			float scaleY=(float)(getHeight()-1)/(float)maxI;
			float scaleX=(float)(getWidth()-1)/(float)SAMPLE_TIME;

			for (int i = 0; i <= 10; i++)
			{
				int y = i * 1000;
				y = getHeight() - 1 - y*scaleY;
				if ( y < 0 )
					break;
				p.moveTo(1,y);
				p.lineTo(10,y);
			}

	  		p.setColor(Color::getColorFrom24BitRGB(255, 255, 0));
			p.moveTo(1,getHeight() - 1 - mainTrackCurrent[0]*scaleY);
			for (int i = 1; i < mainTrackCurrent.Count(); i++)
			{
				p.lineTo(i*scaleX+1, getHeight() - 1 - mainTrackCurrent[i]*scaleY);
			}
				
	  		p.setColor(Color::getColorFrom24BitRGB(0, 255, 255));
			p.moveTo(1,getHeight() - 1 - progTrackCurrent[0]*scaleY);
			for (int i = 1; i < progTrackCurrent.Count(); i++)
				p.lineTo(i*scaleX+1, getHeight() - 1 - progTrackCurrent[i]*scaleY);
		}
	}

	Rect getSolidRect() const
	{
		return touchgfx::Rect(0, 0, getWidth(), getHeight());
	}	
};