#pragma once

#include "typedefs.hpp"
#include <vector>

namespace velo {
	struct Rect {
		short x, y, width, height;
	};
	struct CharInfo {
		union {
			Int16 unicodeChar;
			Int8 asciiChar;
		}pixel;
		Int16 attrib;
	};

	struct RenderElement {
		void* hConsole;
		std::vector<CharInfo> buffer;
		Rect viewSpace;
	};

	class RenderTarget {
	public:
		RenderTarget();
		RenderTarget(RenderElement* re);
		~RenderTarget();

		void setPixel(Int16 x, Int16 y, Word c = 0x2588, Word attrib = 0x00FF);

		void renderLine(Int16 x1, Int16 y1, Int16 x2, Int16 y2, Word c = 0x2588, Word attrib = 0x00FF);

		void renderTriangle(Int16 x1, Int16 y1, Int16 x2, Int16 y2, Int16 x3, Int16 y3, Word c = 0x2588, Word attrib = 0x00FF);

		bool checkInBoundaries(Int16 x, Int16 y, Rect rect);

	protected:
		RenderElement re;
		friend class Console;
		CharInfo getPixelAt(Int16 x, Int16 y);
		CharInfo getPixelBuffer() const;

	};
}
