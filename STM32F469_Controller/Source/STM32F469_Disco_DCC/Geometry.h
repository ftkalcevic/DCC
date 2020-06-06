#pragma once

#include <touchgfx\hal\Types.hpp>

using namespace touchgfx;

class Geometry
{
public:
	// clip line for (x1,y1) to (x2,y2) within the rectangle r.  Return true if any part of the line remains in the rectangle.
	static bool ClipSegment(Rect r, float &x1, float &y1, float &x2, float &y2);
};
