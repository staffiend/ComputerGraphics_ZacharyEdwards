#pragma once

namespace Collision
{
	enum { NO_COLLISION = 0, TOP_OF_1, RIGHT_OF_1, BOTTOM_OF_1, LEFT_OF_1 };

	int minkowski( float x0, float y0, float w0, float h0, float x1, float y1, float w1, float h1 )
	{
		float w = 0.5 * (w0 + w1);
		float h = 0.5 * (h0 + h1);
		float dx = x0 - x1 + 0.5*(w0 - w1);
		float dy = y0 - y1 + 0.5*(h0 - h1);

		if ( dx*dx <= w * w && dy*dy <= h * h )
		{
			float wy = w * dy;
			float hx = h * dx;

			if ( wy > hx )
			{
				return (wy + hx > 0) ? BOTTOM_OF_1 : LEFT_OF_1;
			}
			else
			{
				return (wy + hx > 0) ? RIGHT_OF_1 : TOP_OF_1;
			}
		}

		return NO_COLLISION;
	}
}
