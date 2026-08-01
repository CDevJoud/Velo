#include "RenderTarget.hpp"

namespace velo {

	RenderTarget::RenderTarget() : re(nullptr) {

	}
	RenderTarget::RenderTarget(RenderElement* re) : re(re) {

	}
	RenderTarget::~RenderTarget() {
		RenderTarget::re.buffer.clear();
		RenderTarget::re.buffer.shrink_to_fit();
	}

	void RenderTarget::setPixel(Int16 x, Int16 y, Word c, Word attrib) {
		using rt = RenderTarget;
		if (x >= rt::re.viewSpace.x &&
			x < rt::re.viewSpace.width &&
			y >= rt::re.viewSpace.y &&
			y <  rt::re.viewSpace.height
			) {
			rt::re.buffer[y * rt::re.viewSpace.width + x].pixel.unicodeChar = c;
			rt::re.buffer[y * rt::re.viewSpace.width + x].attrib = attrib;
		}
	}

	void RenderTarget::renderLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t c, uint16_t attrib) {
		Int32 x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
		dx = x2 - x1; dy = y2 - y1;
		dx1 = abs(dx); dy1 = abs(dy);
		px = 2 * dy1 - dx1; py = 2 * dx1 - dy1;
		if (dy1 <= dx1) {
			if (dx >= 0) {
				x = x1; y = y1; xe = x2;
			}
			else {
				x = x2; y = y2; xe = x1;
			}

			RenderTarget::setPixel(x, y, c, attrib);

			for (i = 0; x < xe; i++) {
				x = x + 1;
				if (px < 0) {
					px = px + 2 * dy1;
				}
				else {
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1; else y = y - 1;
					px = px + 2 * (dy1 - dx1);
				}
				RenderTarget::setPixel(x, y, c, attrib);
			}
		}
		else {
			if (dy >= 0) {
				x = x1; y = y1; ye = y2;
			}
			else {
				x = x2; y = y2; ye = y1;
			}

			RenderTarget::setPixel(x, y, c, attrib);

			for (i = 0; y < ye; i++) {
				y = y + 1;
				if (py <= 0)
					py = py + 2 * dx1;
				else {
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x = x + 1; else x = x - 1;
					py = py + 2 * (dx1 - dy1);
				}
				RenderTarget::setPixel(x, y, c, attrib);
			}
		}
	}

	void RenderTarget::renderTriangle(Int16 x1, Int16 y1, Int16 x2, Int16 y2, Int16 x3, Int16 y3, Word c, Word attrib) {
		RenderTarget::renderLine(x1, y1, x2, y2, c, attrib);
		RenderTarget::renderLine(x2, y2, x3, y3, c, attrib);
		RenderTarget::renderLine(x3, y3, x1, y1, c, attrib);
	}


	bool RenderTarget::checkInBoundaries(Int16 x, Int16 y, Rect rect) {
		return (x >= rect.x && x <= rect.width &&
				y >= rect.y && y <= rect.height);
	}
}
