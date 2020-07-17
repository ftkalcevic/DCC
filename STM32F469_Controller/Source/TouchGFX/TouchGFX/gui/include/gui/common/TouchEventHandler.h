#pragma  once


template<class T>
class TouchEventHandler : public T
{
	#define TOUCHDEBUG (void)
	
	bool dragging = false;
	int count = 0;
protected:	
	virtual void handleClickEvent(const ClickEvent & evt)
	{
		TOUCHDEBUG("%d click %d (%d,%d):%d\n", count++, evt.getType(), evt.getX(), evt.getY(), evt.getForce() );
		if (evt.getType() == ClickEvent::PRESSED)
			dragging = true;
		T::handleClickEvent(evt);
	}
	virtual void handleDragEvent(const DragEvent & evt)
	{
		TOUCHDEBUG("%d drag d(%d,%d) n(%d,%d) o(%d,%d)\n", count++, evt.getDeltaX(), evt.getDeltaY(), evt.getNewX(), evt.getNewY(), evt.getOldX(), evt.getOldY());
		T::handleDragEvent(evt);
	}
	virtual void handleGestureEvent(const GestureEvent & evt)
	{
		TOUCHDEBUG("%d gesture %d %d (%d,%d)\n", count++, evt.getType(), evt.getVelocity(), evt.getX(), evt.getY());
		if (evt.getType() == GestureEvent::SWIPE_VERTICAL && evt.getVelocity() < -EXITSCREEN_SWIPE_VELOCITY && evt.getY() < EXITSCREEN_MAXY )
		{
			SwipePreviousScreen();
		}
		else
			T::handleGestureEvent(evt);
	}
	virtual void handleKeyEvent(uint8_t key)
	{
		TOUCHDEBUG("%d key %d\n", count++, key);
		T::handleKeyEvent(key);
	}
	
	virtual void SwipePreviousScreen()
	{}
};


