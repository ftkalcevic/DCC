#pragma once

#include <touchgfx/Color.hpp>
#include <touchgfx\hal\Types.hpp>
#include "Geometry.h"

class PaintContext
{
	Rect clippingArea;
	const Drawable *drawable;
	int x, y;
	colortype c;
	void drawPixel(int x, int y, colortype c)
	{
		HAL::lcd().fillRect(Rect(x, y, 1, 1), c);
	}
	

	void drawLine(int x1, int y1, int x2, int y2, colortype c)
	{
		if (x1 == x2)
		{
			if (y1 > y2)
			{
				int temp = y1;
				y1 = y2;
				y2 = temp;
			}
			HAL::lcd().drawVerticalLine(x1, y1, y2 - y1 + 1, 1, c);
		}
		else if (y1 == y2)
		{
			if (x1 > x2)
			{
				int temp = x1;
				x1 = x2;
				x2 = temp;
			}
			HAL::lcd().drawHorizontalLine(x1, y1, x2 - x1 + 1, 1, c);
		}
		else
		{
			// https://gist.github.com/bert/1085538
			int dx =  abs(x2 - x1);
			int dy = -abs(y2 - y1);
			int sx = x1 < x2 ? 1 : -1;
			int sy = y1 < y2 ? 1 : -1; 
			int e1 = dx + dy;
			int e2;
 
			for (;;)
			{  
				drawPixel(x1, y1, c);
				if (x1 == x2 && y1 == y2) 
					break;
				e2 = 2 * e1;
				if (e2 >= dy) 
				{ 
					e1 += dy; 
					x1 += sx; 
				}
				if (e2 <= dx) 
				{ 
					e1 += dx; 
					y1 += sy; 
				}
			}
		}
	}
	
public:
	PaintContext(const Drawable *drawable, Rect clippingArea) : drawable(drawable), clippingArea(clippingArea)
	{
		x = 0;
		y = 0;
	}
	
	void setColor(colortype colour)
	{
		c = colour;
	}
	
	void clear()
	{
	    Rect dirty = drawable->getSolidRect() & clippingArea;
        drawable->translateRectToAbsolute(dirty);

		HAL::lcd().fillRect(dirty, c);
	}
	
	void moveTo(int newX, int newY)
	{
		x = newX;
		y = newY;
	}
	void lineTo(int toX, int toY)
	{
		float x1 = x, y1 = y;
		float x2 = toX, y2 = toY;
		
		// clip the line 
		if(Geometry::ClipSegment(clippingArea, x1, y1, x2, y2))
		{
			Rect r1(x1, y1, 0, 0);
			Rect r2(x2, y2, 0, 0);
			drawable->translateRectToAbsolute(r1);
			drawable->translateRectToAbsolute(r2);
			
			drawLine(r1.x, r1.y, r2.x, r2.y, c);
		}
		
		// draw the clipped line
		x = toX;
		y = toY;
	}
};