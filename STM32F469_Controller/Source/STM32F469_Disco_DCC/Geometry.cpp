// http://www.richardssoftware.net/2014/07/clipping-lines-to-rectangle-using-cohen.html

#include "Geometry.h"

using namespace touchgfx;

enum OutCode: uint8_t {
	Inside = 0,
	Left   = 1,
	Right  = 2,
	Bottom = 4,
	Top    = 8
};


static uint8_t ComputeOutCode(float x, float y, Rect r) 
{
    uint8_t code = OutCode::Inside;

    if (x < r.x) code |= OutCode::Left;
    if (x > r.right()) code |= OutCode::Right;
    if (y < r.y) code |= OutCode::Top;
    if (y > r.bottom()) code |= OutCode::Bottom;

    return code;
}

static void CalculateIntersection(Rect r, float x1, float y1, float x2, float y2, float &clippedX, float &clippedY, uint8_t clipTo) 
{
	float dx = x2 - x1;
	float dy = y2 - y1;

    float slopeY = dx / dy; // slope to use for possibly-vertical lines
    float slopeX = dy / dx; // slope to use for possibly-horizontal lines

    if (clipTo && OutCode::Top) 
    {
	    clippedX = x1 + slopeY * (r.y - y1);
	    clippedY = r.y;
    }
    else if (clipTo && OutCode::Bottom) 
    {
	    clippedX = x1 + slopeY * (r.bottom() - y1);
	    clippedY = r.bottom();
    }
    else if (clipTo && OutCode::Right) {
	    clippedX = r.right();
	    clippedY = y1 + slopeX * (r.right() - x1);
    }
    else if (clipTo && OutCode::Left) {
	    clippedX = r.x;
	    clippedY = y1 + slopeX * (r.x - x1);
    }
}


bool Geometry::ClipSegment(Rect r, float &x1, float &y1, float &x2, float &y2 ) 
{
    // classify the endpoints of the line
    uint8_t outCodeP1 = ComputeOutCode(x1, y1, r);
    uint8_t outCodeP2 = ComputeOutCode(x2, y2, r);
    bool accept = false;
            
    while (true) { // should only iterate twice, at most
        // Case 1:
        // both endpoints are within the clipping region
        if ((outCodeP1 | outCodeP2) == OutCode::Inside) 
        {
            accept = true;
            break;
        }

        // Case 2:
        // both endpoints share an excluded region, impossible for a line between them to be within the clipping region
        if ((outCodeP1 & outCodeP2) != 0) 
        {
            break;
        }

        // Case 3:
        // The endpoints are in different regions, and the segment is partially within the clipping rectangle

        // Select one of the endpoints outside the clipping rectangle
        uint8_t outCode = outCodeP1 != OutCode::Inside ? outCodeP1 : outCodeP2;

	    // calculate the intersection of the line with the clipping rectangle
		float clippedX, clippedY;
        CalculateIntersection(r, x1, y1, x2, y2, clippedX, clippedY, outCode);

        // update the point after clipping and recalculate outcode
        if (outCode == outCodeP1) 
        {
	        x1 = clippedX;
	        y1 = clippedY;
            outCodeP1 = ComputeOutCode(x1, y1, r);
        } 
	    else 
	    {
		    x2 = clippedX;
		    y2 = clippedY;
            outCodeP2 = ComputeOutCode(x2, y2, r);
        }
    }
	
	return accept;
}