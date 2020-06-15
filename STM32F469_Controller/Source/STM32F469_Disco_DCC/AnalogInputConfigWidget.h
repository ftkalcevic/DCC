#pragma once


#include <touchgfx/containers/Container.hpp>
#include <touchgfx/widgets/Image.hpp>

class AnalogInputConfigWidget : public Container
{
public:
	AnalogInputConfigWidget()
	{
		allowUpdate = false;
		reverse = false;
		
		setTouchable(true);

		Container::add(background);
		Container::add(indicatorMin);
		Container::add(indicatorMax);
		Container::add(indicator);

		// Default value range
		setValueRange(0, 100);
	}

	virtual ~AnalogInputConfigWidget()
	{
	}

	void setBitmaps(const Bitmap& sliderBackground, const Bitmap& indicatorRange, const Bitmap& indicatorPos)
	{
		background.setBitmap(sliderBackground);
		indicatorMin.setBitmap(indicatorRange);
		indicatorMax.setBitmap(indicatorRange);
		indicator.setBitmap(indicatorPos);
	}
	
	void setupHorizontalSlider(uint16_t backgroundX, uint16_t backgroundY, uint16_t indicatorY, uint16_t indicatorMinX, uint16_t indicatorMaxX)
	{
		background.setXY(backgroundX, backgroundY);

		uint16_t backgroundWidth = backgroundX + static_cast<uint16_t>(background.getWidth());
		uint16_t indicatorWidth = indicatorMaxX + static_cast<uint16_t>(indicator.getWidth());
		int16_t newWidth = static_cast<int16_t>(MAX(backgroundWidth, indicatorWidth));

		uint16_t backgroundHeight = backgroundY + static_cast<uint16_t>(background.getHeight());
		uint16_t indicatorHeight = indicatorY + static_cast<uint16_t>(indicator.getHeight());
		int16_t newHeight = static_cast<int16_t>(MAX(backgroundHeight, indicatorHeight));

		setWidth(newWidth);
		setHeight(newHeight);

		setValue(currentValue);
	}
    virtual void setValueRange(int minValue, int maxValue)
	{
		valueRangeMin = minValue;
		valueRangeMax = maxValue;
		UpdatePositions();
	}

	virtual void setValue(int value)
	{
		bool change = false;
		if (allowUpdate)
		{
			if (value < valueRangeMin)
			{
				valueRangeMin = value;
				change = true;
			}
			else if (value > valueRangeMax)
			{
				valueRangeMax = value;
				change = true;
			}
		}
		if (value != currentValue)
		{
			currentValue = value;
			change = true;
		}
		if (change)
			UpdatePositions();
	}
	virtual int getValue()const {return currentValue;}
	virtual void setAllowUpdate( bool u ) { allowUpdate = u; }
	virtual void setReverse(bool r) { reverse = r; }
protected:
	bool reverse;
	bool allowUpdate;
	int currentValue;                         ///< The current value represented of the slider

	int valueRangeMin;                        ///< The value range min
	int valueRangeMax;                        ///< The value range max

	Image background;
	Image indicatorMin;
	Image indicatorMax;
	Image indicator;
	
	void UpdatePositions()
	{
		const int MinRange = 0;
		const int MaxRange = 4095;
		
		indicatorMin.moveTo(getWidth() * valueRangeMin/MaxRange, indicatorMin.getY());
		indicatorMax.moveTo(getWidth() * valueRangeMax/MaxRange, indicatorMax.getY());
		indicator.moveTo(getWidth() * currentValue/MaxRange, indicator.getY());
	}
};