#include "TUIpp.hpp"

namespace tui {

	template<typename T>
	 Rect<T>::Rect() : x(0), y(0), width(0), height(0) {}

	template<typename T>
	 Rect<T>::Rect(T rectLeft, T rectTop, T rectWidth, T rectHeight) : x(rectLeft), y(rectTop), width(rectWidth), height(rectHeight) {}

	template<typename T>
	 Rect<T>::Rect(const Vec2<T>& p, const Vec2<T>& size) : x(p.x), y(p.y), width(size.x), height(size.y) {}

	template<typename T>
	 bool Rect<T>::contains(T x, T y) const {

		T minX = std::min(this->x, static_cast<T>(this->x + width));
		T maxX = std::max(this->x, static_cast<T>(this->x + width));
		T minY = std::min(this->y, static_cast<T>(this->y + height));
		T maxY = std::max(this->y, static_cast<T>(this->y + height));

		return (x >= minX) && (x < maxX) && (y >= minY) && (y < maxY);

	}

	template<typename T>
	 bool Rect<T>::contains(const Vec2<T>& point) const {
		return contains(point.x, point.y);
	}

	template <typename T>
	bool Rect<T>::intersects(const Rect<T>& rectangle) const {
		Rect<T> intersection;
		return intersects(rectangle, intersection);
	}

	template<typename T>
	bool Rect<T>::isInsideBounds(const Vec2<T>& p) const {
		return (p.x >= this->x && p.x <= this->width && p.y >= this->y && p.y <= this->height);
	}


	template <typename T>
	bool Rect<T>::intersects(const Rect<T>& rectangle, Rect<T>& intersection) const {
		// Rectangles with negative dimensions are allowed, so we must handle them correctly

		// Compute the min and max of the first rectangle on both axes
		T r1MinX = std::min(x, static_cast<T>(x + width));
		T r1MaxX = std::max(x, static_cast<T>(x + width));
		T r1MinY = std::min(y, static_cast<T>(y + height));
		T r1MaxY = std::max(y, static_cast<T>(y + height));

		// Compute the min and max of the second rectangle on both axes
		T r2MinX = std::min(rectangle.x, static_cast<T>(rectangle.x + rectangle.width));
		T r2MaxX = std::max(rectangle.x, static_cast<T>(rectangle.x + rectangle.width));
		T r2MinY = std::min(rectangle.y, static_cast<T>(rectangle.y + rectangle.height));
		T r2MaxY = std::max(rectangle.y, static_cast<T>(rectangle.y + rectangle.height));

		// Compute the intersection boundaries
		T interLeft = std::max(r1MinX, r2MinX);
		T interTop = std::max(r1MinY, r2MinY);
		T interRight = std::min(r1MaxX, r2MaxX);
		T interBottom = std::min(r1MaxY, r2MaxY);

		// If the intersection is valid (positive non zero area), then there is an intersection
		if ((interLeft < interRight) && (interTop < interBottom)) {
			intersection = Rect<T>(interLeft, interTop, interRight - interLeft, interBottom - interTop);
			return true;
		}
		else {
			intersection = Rect<T>(0, 0, 0, 0);
			return false;
		}
	}

	template <typename T>
	Vec2<T> Rect<T>::getPosition() const {
		return Vec2<T>(x, y);
	}

	template <typename T>
	Vec2<T> Rect<T>::getSize() const {
		return Vec2<T>(width, height);
	}

	template <typename T>
	 bool operator ==(const Rect<T>& left, const Rect<T>& right) {
		return (left.x == right.x) && (left.width == right.width) &&
			(left.y == right.y) && (left.height == right.height);
	}


	template <typename T>
	 bool operator !=(const Rect<T>& left, const Rect<T>& right) {
		return !(left == right);
	}

	template<typename T>
	template<typename U>
	Rect<T>::Rect(const Rect<U>& rectangle) :
		x(static_cast<T>(rectangle.x)),
		y(static_cast<T>(rectangle.y)),
		width(static_cast<T>(rectangle.width)),
		height(static_cast<T>(rectangle.height)) {}

#if defined(_WIN32) || defined(_WIN64)
	static RenderElement* g_re;
	/*static void VisualDebuggerBreakPoint() {
		SMALL_RECT rect = { 0, 0, g_re->viewport.width, g_re->viewport.height };
		WriteConsoleOutputW(g_re->hConsole, (PCHAR_INFO)g_re->screenBuffer, { (short)g_re->viewport.width, (short)g_re->viewport.height }, {}, &rect);
		while (!(GetAsyncKeyState(VK_SPACE) & 0x8000)) {
			Sleep(1);
		}
	}*/

	/// ////
	ConsoleWindow_ImplNativeWin32::ConsoleWindow_ImplNativeWin32(NativeHandle window, const String& title, const Vec2<Word>& dimension) :
		bIsOpen(false),
		nwh(nullptr) {
		using CW = ConsoleWindow_ImplNativeWin32;
		if (title.empty() && dimension.x == 0 && dimension.y == 0) {
			if (CW::initConsoleWindow()) {
				CW::bIsOpen = (CW::nwh != nullptr);
			}
		}
		else if (CW::initConsoleWindow()) {
			CW::bIsOpen = (CW::nwh != nullptr);
			CW::setSize(dimension);

			RECT wndRect, deskRect;

			::GetWindowRect(CW::nwh, &wndRect);
			::GetWindowRect(::GetDesktopWindow(), &deskRect);

			auto wnd = Vec2<Int32>(wndRect.right - wndRect.left, wndRect.bottom - wndRect.top);
			auto desk = Vec2<Int32>(deskRect.right - deskRect.left, deskRect.bottom - deskRect.top);

			auto position = Vec2<Word>((desk.x - wnd.x) / 2ui16, (desk.y - wnd.y) / 2ui16);

			CW::setPosition(position);

			CW::setTitle(title);
		}

		CW::setTitleBarColor(RGB(0, 96, 0));
	}
	 ConsoleWindow_ImplNativeWin32::~ConsoleWindow_ImplNativeWin32() {
		FreeConsole();
	}
	 bool ConsoleWindow_ImplNativeWin32::isOpen() const {
		return bIsOpen;
	}
	 NativeHandle ConsoleWindow_ImplNativeWin32::getNativeHandle() const {
		return nwh;
	}
	 void ConsoleWindow_ImplNativeWin32::setPosition(const Vec2<Word>& p) {
		RECT wndRect;
		GetWindowRect(nwh, &wndRect);
		MoveWindow(nwh, p.x, p.y, wndRect.right, wndRect.bottom, TRUE);
	}
	 void ConsoleWindow_ImplNativeWin32::setSize(const Vec2<Word>& size) {
		RECT wndRect;
		GetWindowRect(nwh, &wndRect);
		MoveWindow(nwh, wndRect.left, wndRect.top, size.x, size.y, TRUE);
	}



	 Vec2<Word> ConsoleWindow_ImplNativeWin32::getSize() const {
		RECT wndRect;
		GetWindowRect(nwh, &wndRect);
		return Vec2<Word>(wndRect.right, wndRect.bottom);
	}
	 Vec2<Word> ConsoleWindow_ImplNativeWin32::getPosition() const {
		RECT wndRect;
		GetWindowRect(nwh, &wndRect);
		return Vec2<Word>(wndRect.left, wndRect.top);
	}
	 const String ConsoleWindow_ImplNativeWin32::getTitle() const {
#if defined(TUIPP_USE_UTF8_STRING)
		char8_t buffer[MAX_PATH]{};
		GetWindowTextA(nwh, (char*)buffer, MAX_PATH);
		return buffer;
#elif defined(TUIPP_USE_UTF16_STRING)
		char16_t buffer[MAX_PATH]{};
		GetWindowTextW(nwh, (wchar_t*)buffer, MAX_PATH);
		return buffer;
#elif defined(TUIPP_USE_UTF32_STRING)
#error UTF-32 is still in development!
#endif
	}
	 void ConsoleWindow_ImplNativeWin32::setTitle(const String& title) {
#if defined(TUIPP_USE_UTF8_STRING)
		SetWindowTextA(nwh, (char*)title.c_str());
#elif defined(TUIPP_USE_UTF16_STRING)
		SetWindowTextW(nwh, (wchar_t*)title.c_str());
#elif defined(TUIPP_USE_UTF32_STRING)
#error UTF-32 is still in development!
#endif
	}
	 void ConsoleWindow_ImplNativeWin32::close() {
		bIsOpen = false;
	}
	 void ConsoleWindow_ImplNativeWin32::display(RenderElement& re) {
		SMALL_RECT rect = { 0, 0, re.viewport.width, re.viewport.height };
		WriteConsoleOutputW(re.hConsole, (PCHAR_INFO)re.screenBuffer, { (short)re.viewport.width, (short)re.viewport.height }, {}, &rect);
	}
	 void ConsoleWindow_ImplNativeWin32::setTitleBarColor(Dword color) {
		DwmSetWindowAttribute(nwh, DWMWA_CAPTION_COLOR, &color, sizeof(color));
	}
	 void ConsoleWindow_ImplNativeWin32::setTitleBarTextColor(Dword color) {
		DwmSetWindowAttribute(nwh, DWMWA_TEXT_COLOR, &color, sizeof(color));
	}
	 bool ConsoleWindow_ImplNativeWin32::initConsoleWindow() {
		using CW = ConsoleWindow_ImplNativeWin32;
		CW::nwh = GetConsoleWindow();
		if (!CW::nwh) {
			AllocConsole();
			CW::nwh = GetConsoleWindow();
			if (!CW::nwh) {
				MessageBoxA(nullptr, "Could not init console window!", "TUIpp *Error* ", MB_ICONERROR | MB_OK);
				return false;
			}
		}
		return true;
	}
	 ConsoleWindow_ImplWin32GL_1_1::ConsoleWindow_ImplWin32GL_1_1(NativeHandle window, const String& title, const Vec2<Word>& dimension, const Vec2<Word>& pxlDimension, const std::shared_ptr<EventProcessor_ImplWin32>& ep) {
		this->pxlDimension = pxlDimension;
		this->dimension = dimension;
		this->stretch = false;
		this->prevBuffer = nullptr;
		this->eventProcessor = ep;
		using CW = ConsoleWindow_ImplWin32GL_1_1;
		if (CW::initWindowClass()) {
			if (CW::createWindowInstance(title, dimension)) {
				if (CW::initGL_1_1()) {

					//hBlankCursor = CW::createBlankCursor();
					ShowCursor(FALSE);

					CW::setTitleBarColor(RGB(0, 0, 0));
					CW::setTitleBarTextColor(RGB(128, 128, 128));

					CW::bakeSplashScreen();
					CW::loadFont();

					RECT wndRect, deskRect;

					::GetWindowRect(CW::nwh, &wndRect);
					::GetWindowRect(::GetDesktopWindow(), &deskRect);

					auto wnd = Vec2<Int32>(wndRect.right - wndRect.left, wndRect.bottom - wndRect.top);
					auto desk = Vec2<Int32>(deskRect.right - deskRect.left, deskRect.bottom - deskRect.top);

					auto position = Vec2<Word>((desk.x - wnd.x) / 2ui16, (desk.y - wnd.y) / 2ui16);

					RECT rect = { 0, 0, (Int32)dimension.x, (Int32)dimension.y };

					AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

					CW::setPosition(position);
					CW::setSize(Vec2w(rect.right, rect.bottom));

					QueryPerformanceFrequency(&freq);
					QueryPerformanceCounter(&counter);
					Int32 row = dimension.x / pxlDimension.x;
					Int32 col = dimension.y / pxlDimension.y;
					this->bgVertices.resize(row * col);
					this->glyphVertices.resize(row * col);
					ShowWindow(nwh, SW_SHOW);
					UpdateWindow(nwh);
				}
			}
		}
	}

	ConsoleWindow_ImplWin32GL_1_1::~ConsoleWindow_ImplWin32GL_1_1() {
		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(glrc);
		ReleaseDC(nwh, hdc);
		DestroyCursor(hBlankCursor);
		DestroyWindow(nwh);
		UnregisterClass(TEXT("TUIPP:ConsoleWindow_ImplWin32GL_1_1"), GetModuleHandleA(nullptr));
	}

	 bool ConsoleWindow_ImplWin32GL_1_1::isOpen() const {
		MSG msg{};
		while (PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE) > 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return (msg.message != WM_QUIT);
	}

	 NativeHandle ConsoleWindow_ImplWin32GL_1_1::getNativeHandle() const {
		return nwh;
	}

	 void ConsoleWindow_ImplWin32GL_1_1::setPosition(const Vec2<Word>& p) {
		RECT wndRect;
		GetWindowRect(nwh, &wndRect);
		MoveWindow(nwh, p.x, p.y, wndRect.right, wndRect.bottom, TRUE);
	}

	 void ConsoleWindow_ImplWin32GL_1_1::setSize(const Vec2<Word>& size) {
		RECT wndRect;
		GetWindowRect(nwh, &wndRect);
		MoveWindow(nwh, wndRect.left, wndRect.top, size.x, size.y, TRUE);
	}

	 Vec2<Word> ConsoleWindow_ImplWin32GL_1_1::getSize() const {
		RECT wndRect;
		GetWindowRect(nwh, &wndRect);
		return Vec2<Word>(wndRect.right, wndRect.bottom);
	}

	 Vec2<Word> ConsoleWindow_ImplWin32GL_1_1::getPosition() const {
		RECT wndRect;
		GetWindowRect(nwh, &wndRect);
		return Vec2<Word>(wndRect.left, wndRect.top);
	}

	 const String ConsoleWindow_ImplWin32GL_1_1::getTitle() const {
		return String();
	}

	 void ConsoleWindow_ImplWin32GL_1_1::setTitle(const String& title) {}

	 void ConsoleWindow_ImplWin32GL_1_1::close() {
		SendMessage(nwh, WM_CLOSE, 0, 0);
	}

	 void ConsoleWindow_ImplWin32GL_1_1::display(RenderElement& re) {
		//static LARGE_INTEGER start = {};
		//LARGE_INTEGER current;

		//if (start.QuadPart == 0)
		//	QueryPerformanceCounter(&start);

		//QueryPerformanceCounter(&current);

		//double elapsedSeconds =
		//	double(current.QuadPart - start.QuadPart) /
		//	double(freq.QuadPart);


		//// Clear frame
		//gl.Clear(gl.color_buffer_bit);


		//// Setup 2D pixel coordinates
		//glViewport(0, 0, dimension.x, dimension.y);

		//glMatrixMode(GL_PROJECTION);
		//glLoadIdentity();

		//glOrtho(
		//	0,
		//	dimension.x,
		//	dimension.y,
		//	0,
		//	-1,
		//	1
		//);

		//glMatrixMode(GL_MODELVIEW);
		//glLoadIdentity();


		////
		//// Splash screen (first 3 seconds)
		////
		//if (elapsedSeconds <= 10.5) {
		//	gl.Enable(gl.texture_2d);

		//	gl.Enable(gl.blend);
		//	gl.BlendFunc(
		//		gl.src_alpha,
		//		gl.one_minus_src_alpha
		//	);

		//	gl.BindTexture(gl.texture_2d, texture);


		//	gl.Begin(gl.quads);

		//	gl.TexCoord2f(0, 0);
		//	gl.Vertex2f(0, 0);

		//	gl.TexCoord2f(1, 0);
		//	gl.Vertex2f(dimension.x, 0);

		//	gl.TexCoord2f(1, 1);
		//	gl.Vertex2f(dimension.x, dimension.y);

		//	gl.TexCoord2f(0, 1);
		//	gl.Vertex2f(0, dimension.y);

		//	gl.End();
		//}
		//else {
		//	//
		//	// Test character
		//	//
		//	CharInfo c;
		//	c.pixel.unicodeChar = 0x2588;
		//	c.attrib = 0xFF;


		//	Word row = dimension.x / pxlDimension.x;
		//	Word column = dimension.y / pxlDimension.y;
		//	for (Word x = 0; x < row; x++) {
		//		for (Word y = 0; y < column; y++) {
		//			drawCell(x, y, re.screenBuffer[y * re.viewport.width + x]);
		//		}
		//	}
		//}

		//SwapBuffers(hdc);

		//QueryPerformanceCounter(&counter);

		Word row = dimension.x / pxlDimension.x;
		Word column = dimension.y / pxlDimension.y;


		if (prevBuffer == nullptr) {
			prevBuffer = new CharInfo[re.viewport.width * re.viewport.height]{};
		}

		static LARGE_INTEGER start = {};
		LARGE_INTEGER current;

		if (start.QuadPart == 0)
			QueryPerformanceCounter(&start);

		QueryPerformanceCounter(&current);

		double elapsedSeconds =
			double(current.QuadPart - start.QuadPart) /
			double(freq.QuadPart);


		gl.Clear(gl.color_buffer_bit);

		gl.Viewport(
			0,
			0,
			dimension.x,
			dimension.y
		);


		gl.MatrixMode(gl.projection);
		gl.LoadIdentity();

		gl.Ortho(
			0,
			dimension.x,
			dimension.y,
			0,
			-1,
			1
		);


		gl.MatrixMode(gl.modelview);
		gl.LoadIdentity();



		bgVertices.clear();
		glyphVertices.clear();


		//
		// Splash screen (first 3 seconds)
		//
		if (elapsedSeconds <= 0.5) {
			gl.Enable(gl.texture_2d);

			gl.Enable(gl.blend);
			gl.BlendFunc(
				gl.src_alpha,
				gl.one_minus_src_alpha
			);

			gl.BindTexture(gl.texture_2d, texture);


			gl.Begin(gl.quads);

			gl.TexCoord2f(0, 0);
			gl.Vertex2f(0, 0);

			gl.TexCoord2f(1, 0);
			gl.Vertex2f(dimension.x, 0);

			gl.TexCoord2f(1, 1);
			gl.Vertex2f(dimension.x, dimension.y);

			gl.TexCoord2f(0, 1);
			gl.Vertex2f(0, dimension.y);

			gl.End();
		}
		else {


			//
			// Build console
			//

			for (int y = 0; y < column; y++) {
				for (int x = 0; x < row; x++) {
					Int32 thisPxl = *reinterpret_cast<Int32*>(&re.screenBuffer[y * re.viewport.width + x]);

					//this line crashes because the console window resizes, and it contains the dimensions of the old size
					//Int32 prevPxl = *reinterpret_cast<Int32*>(&prevBuffer[y * row + x]);
					//if (thisPxl != prevPxl) {
					drawCellBatch(x, y, re.screenBuffer[y * re.viewport.width + x]);
					//}
				}
			}

			memcpy(prevBuffer, re.screenBuffer, sizeof(CharInfo) * re.viewport.width * re.viewport.height);

			//
			// Draw backgrounds
			//
			gl.Disable(gl.texture_2d);
			gl.Disable(gl.blend);

			gl.Begin(gl.quads);

			for (auto& v : bgVertices) {
				gl.Color4ub(
					v.r,
					v.g,
					v.b,
					v.a
				);

				gl.Vertex2f(
					v.x,
					v.y
				);
			}

			gl.End();

			//
			// Draw glyphs
			//
			gl.Enable(gl.texture_2d);

			gl.Enable(gl.blend);

			gl.BlendFunc(
				gl.src_alpha,
				gl.one_minus_src_alpha
			);

			gl.BindTexture(
				gl.texture_2d,
				fontAtlas
			);

			gl.Begin(gl.quads);

			for (auto& v : glyphVertices) {
				gl.Color4ub(
					v.r,
					v.g,
					v.b,
					v.a
				);

				gl.TexCoord2f(
					v.u,
					v.v
				);

				gl.Vertex2f(
					v.x,
					v.y
				);
			}


			gl.End();

		}

		SwapBuffers(hdc);
		QueryPerformanceCounter(&counter);

	}



	 void ConsoleWindow_ImplWin32GL_1_1::drawCell(
		Word x,
		Word y,
		CharInfo c
	) {
		char bg = (c.attrib & 0xF0) >> 4;
		char fg = c.attrib & 0x0F;


		COLORREF fgRGB = translate8Bit2RGB(fg);
		COLORREF bgRGB = translate8Bit2RGB(bg);


		//
		// Draw background rectangle
		//
		gl.Disable(gl.texture_2d);
		gl.Disable(gl.blend);


		gl.Color3ub(
			GetRValue(bgRGB),
			GetGValue(bgRGB),
			GetBValue(bgRGB)
		);


		float px = float(x * pxlDimension.x);
		float py = float(y * pxlDimension.y);

		float w = float(pxlDimension.x);
		float h = float(pxlDimension.y);


		gl.Begin(gl.quads);

		gl.Vertex2f(px, py);
		gl.Vertex2f(px + w, py);
		gl.Vertex2f(px + w, py + h);
		gl.Vertex2f(px, py + h);

		gl.End();



		//
// Draw foreground glyph
//
		gl.Enable(gl.texture_2d);

		gl.Enable(gl.blend);
		gl.BlendFunc(
			gl.src_alpha,
			gl.one_minus_src_alpha
		);

		gl.BindTexture(gl.texture_2d, fontAtlas);


		tui::util::Glyph& g =
			font.glyphs[c.pixel.unicodeChar];


		// Texture coordinates
		float u0 = float(g.x) / float(font.scaleW);
		float v0 = float(g.y) / float(font.scaleH);

		float u1 = float(g.x + g.width) / float(font.scaleW);
		float v1 = float(g.y + g.height) / float(font.scaleH);


		// Glyph position inside cell
		float gx = float(x * pxlDimension.x);
		float gy = float(y * pxlDimension.y);


		gx += g.xOffset;
		gy += g.yOffset;


		// Actual glyph size
		float gw = float(g.width);
		float gh = float(g.height);


		gl.Color4ub(
			GetRValue(fgRGB),
			GetGValue(fgRGB),
			GetBValue(fgRGB),
			255
		);


		gl.Begin(gl.quads);

		gl.TexCoord2f(u0, v0);
		gl.Vertex2f(gx, gy);

		gl.TexCoord2f(u1, v0);
		gl.Vertex2f(gx + gw, gy);

		gl.TexCoord2f(u1, v1);
		gl.Vertex2f(gx + gw, gy + gh);

		gl.TexCoord2f(u0, v1);
		gl.Vertex2f(gx, gy + gh);

		gl.End();
	}

	 void ConsoleWindow_ImplWin32GL_1_1::drawCellBatch(Word x, Word y, const CharInfo& c) {
		char bg = (c.attrib & 0xF0) >> 4;
		char fg = c.attrib & 0x0F;

		COLORREF fgRGB = translate8Bit2RGB(fg);
		COLORREF bgRGB = translate8Bit2RGB(bg);


		float px = x * pxlDimension.x;
		float py = y * pxlDimension.y;
		float w = pxlDimension.x;
		float h = pxlDimension.y;

		//
		// Background quad
		//
		auto addBG =
			[&](float vx, float vy) {
			ConsoleVertex v;

			v.x = vx;
			v.y = vy;

			v.u = 0;
			v.v = 0;

			v.r = GetRValue(bgRGB);
			v.g = GetGValue(bgRGB);
			v.b = GetBValue(bgRGB);
			v.a = 255;

			Int32 row = dimension.x / pxlDimension.x;
			//bgVertices[y * row + x] = v;
			bgVertices.push_back(v);

			};


		addBG(px, py);
		addBG(px + w, py);
		addBG(px + w, py + h);
		addBG(px, py + h);

		//
		// Glyph
		//

		tui::util::Glyph& g = font.glyphs[c.pixel.unicodeChar];

		float u0 = float(g.x) / float(font.scaleW);
		float v0 = float(g.y) / float(font.scaleH);

		float u1 = float(g.x + g.width) / float(font.scaleW);
		float v1 = float(g.y + g.height) / float(font.scaleH);

		float gx = px + g.xOffset;
		float gy = py + g.yOffset;

		auto addGlyph =
			[&](float vx, float vy, float uu, float vv) {
			ConsoleVertex v{};

			v.x = vx;
			v.y = vy;

			v.u = uu;
			v.v = vv;

			v.r = GetRValue(fgRGB);
			v.g = GetGValue(fgRGB);
			v.b = GetBValue(fgRGB);
			v.a = 255;


			Int32 row = dimension.x / pxlDimension.x;
			//glyphVertices[y * row + x] = v;

			glyphVertices.push_back(v);
			};

		if (stretch) {
			//
			// Stretch glyph to full cell
			//

			addGlyph(
				px,
				py,
				u0,
				v0
			);

			addGlyph(
				px + pxlDimension.x,
				py,
				u1,
				v0
			);

			addGlyph(
				px + pxlDimension.x,
				py + pxlDimension.y,
				u1,
				v1
			);

			addGlyph(
				px,
				py + pxlDimension.y,
				u0,
				v1
			);
		}
		else {
			addGlyph(
				gx,
				gy,
				u0,
				v0
			);


			addGlyph(
				gx + g.width,
				gy,
				u1,
				v0
			);


			addGlyph(
				gx + g.width,
				gy + g.height,
				u1,
				v1
			);


			addGlyph(
				gx,
				gy + g.height,
				u0,
				v1
			);
		}
	}

	HCURSOR ConsoleWindow_ImplWin32GL_1_1::createBlankCursor() {
		// 32x32 transparent bitmap.
		HBITMAP colorBitmap = CreateBitmap(32, 32, 1, 32, nullptr);
		HBITMAP maskBitmap = CreateBitmap(32, 32, 1, 1, nullptr);

		ICONINFO ii = {};
		ii.fIcon = FALSE;          // Cursor, not icon
		ii.xHotspot = 0;
		ii.yHotspot = 0;
		ii.hbmMask = maskBitmap;
		ii.hbmColor = colorBitmap;

		HCURSOR cursor = CreateIconIndirect(&ii);

		DeleteObject(colorBitmap);
		DeleteObject(maskBitmap);

		return cursor;
	}



	 Uint32 ConsoleWindow_ImplWin32GL_1_1::translate8Bit2RGB(Uint8 c) {
		switch (c) {
		case 0x00: return 0x00000000;
		case 0x01: return 0x00FF0000;
		case 0x02: return 0x0000FF00;
		case 0x03: return 0x00FFFF00;
		case 0x04: return 0x000000FF;
		case 0x05: return 0x00800080;
		case 0x06: return 0x0000BBBB;
		case 0x07: return 0x00AAAAAA;
		case 0x08: return 0x00888888;
		case 0x09: return 0x0090D5FF;
		case 0x0A: return 0x0088E788;
		case 0x0B: return 0x0082FFF3;
		case 0x0C: return 0x00EE2400;
		case 0x0D: return 0x00733B73;
		case 0x0E: return 0x00FFFFC5;
		case 0x0F: return 0x00FFFFFF;

		default:
			return 0x000000;
		}
	}

	 void ConsoleWindow_ImplWin32GL_1_1::setTitleBarColor(Dword color) {
		DwmSetWindowAttribute(nwh, DWMWA_CAPTION_COLOR, &color, sizeof(color));
	}
	 void ConsoleWindow_ImplWin32GL_1_1::setTitleBarTextColor(Dword color) {
		DwmSetWindowAttribute(nwh, DWMWA_TEXT_COLOR, &color, sizeof(color));
	}




	 bool ConsoleWindow_ImplWin32GL_1_1::loadFont() {
		std::vector<Uint8> fontBin;
		util::decodeB64(fontDescriptor, fontBin);

		if (fontBin[0] != 'B' || fontBin[1] != 'M' || fontBin[2] != 'F') {
			return false;
		}

		if (fontBin[3] != 3) {
			return false;
		}

		Uint32 rIdx = 4;
		while (rIdx < fontBin.size()) {
			// Need at least 1 byte for type + 4 bytes for size
			if (rIdx + 5 > fontBin.size())
				return false;

			Uint8 blockType = fontBin[rIdx++];

			Uint32 blockSize;
			memcpy(&blockSize, fontBin.data() + rIdx, sizeof(Uint32));
			rIdx += sizeof(Uint32);

			// Make sure the block fits in the buffer
			if (rIdx + blockSize > fontBin.size())
				return false;

			std::vector<Uint8> data(blockSize);
			memcpy(data.data(), fontBin.data() + rIdx, blockSize);
			rIdx += blockSize;

			switch (blockType) {
			case 1:
				// Info block (optional)
				break;

			case 2:
			{
				memcpy(&font.lineHeight, data.data() + 0, 2);
				memcpy(&font.base, data.data() + 2, 2);
				memcpy(&font.scaleW, data.data() + 4, 2);
				memcpy(&font.scaleH, data.data() + 6, 2);
				break;
			}

			case 3:
			{
				font.textureFile = std::string(reinterpret_cast<char*>(data.data()));
				break;
			}

			case 4:
			{
				const size_t count = blockSize / 20;

				for (size_t i = 0; i < count; ++i) {
					const Uint8* p = data.data() + i * 20;

					util::Glyph g{};

					memcpy(&g.id, p + 0, 4);
					memcpy(&g.x, p + 4, 2);
					memcpy(&g.y, p + 6, 2);
					memcpy(&g.width, p + 8, 2);
					memcpy(&g.height, p + 10, 2);
					memcpy(&g.xOffset, p + 12, 2);
					memcpy(&g.yOffset, p + 14, 2);
					memcpy(&g.xAdvance, p + 16, 2);

					g.page = p[18];
					g.channel = p[19];

					font.glyphs[g.id] = g;
				}

				break;
			}

			case 5:
				// Kerning pairs (optional)
				break;

			default:
				// Unknown block, skip
				break;
			}
		}

		std::vector<Uint8> fontBMFB64;
		util::decodeB64(fontBMF, fontBMFB64);

		std::vector<Uint8> fontBMFRLE;
		util::decompRLE(fontBMFB64, fontBMFRLE);

		Byte* data = fontBMFRLE.data();

		BITMAPFILEHEADER* fileHeader =
			reinterpret_cast<BITMAPFILEHEADER*>(data);

		if (fileHeader->bfType != 0x4D42) {
			MessageBoxA(nullptr,
				"Invalid BMP file.",
				"Error",
				MB_OK | MB_ICONERROR);
			return false;
		}

		BITMAPINFOHEADER* infoHeader =
			reinterpret_cast<BITMAPINFOHEADER*>(
				data + sizeof(BITMAPFILEHEADER));


		if (infoHeader->biBitCount != 32) {
			MessageBoxA(nullptr,
				"BMP is not 32-bit.",
				"Error",
				MB_OK | MB_ICONERROR);
			return false;
		}


		/*if (infoHeader->biCompression != BI_RGB) {
			MessageBoxA(nullptr,
				"Compressed BMPs are not supported.",
				"Error",
				MB_OK | MB_ICONERROR);
			return false;
		}*/


		const int width = infoHeader->biWidth;
		const int height = abs(infoHeader->biHeight);


		// Pixel data
		Byte* pixels =
			data + fileHeader->bfOffBits;


		// 32-bit BMP = 4 bytes per pixel
		const int stride = width * 4;


		// RGBA output
		std::vector<BYTE> rgba(width * height * 4);


		const bool bottomUp = (infoHeader->biHeight > 0);


		for (Int32 y = 0; y < height; y++) {
			int srcY = bottomUp ? (height - 1 - y) : y;

			Byte* src = pixels + srcY * stride;


			for (Int32 x = 0; x < width; x++) {
				// BMP format is BGRA
				Byte* pixel = src + x * 4;

				Int32 dst = (y * width + x) * 4;

				rgba[dst + 0] = pixel[2]; // R
				rgba[dst + 1] = pixel[1]; // G
				rgba[dst + 2] = pixel[0]; // B
				rgba[dst + 3] = pixel[3]; // A
				/*std::string dMsg = std::to_string(x) + " R: " + std::to_string(pixel[2]) + " G: " + std::to_string(pixel[1]) + " B: " + std::to_string(pixel[0]) + " A: " + std::to_string(pixel[3]) + "\n";
				OutputDebugStringA(dMsg.c_str());*/
			}
		}
		gl.GenTextures(1, &fontAtlas);
		gl.BindTexture(gl.texture_2d, fontAtlas);

		gl.PixelStorei(gl.unpack_alignment, 1);

		gl.TexParameteri(gl.texture_2d,
			gl.texture_min_filter,
			gl.nearest);

		gl.TexParameteri(gl.texture_2d,
			gl.texture_mag_filter,
			gl.nearest);

		gl.TexParameteri(gl.texture_2d,
			gl.texture_wrap_s,
			gl.clamp);

		gl.TexParameteri(gl.texture_2d,
			gl.texture_wrap_t,
			gl.clamp);

		gl.TexImage2D(
			gl.texture_2d,
			0,
			0x1908, //GL_RGBA
			width,
			height,
			0,
			0x1908,
			gl.unsigned_byte,
			rgba.data()
		);

		return true;
	}



	 void ConsoleWindow_ImplWin32GL_1_1::bakeSplashScreen() {
		std::vector<Uint8> bmpRLE;
		util::decodeB64(splashscreen, bmpRLE);

		std::vector<Uint8> bmp;
		util::decompRLE(bmpRLE, bmp);
		BYTE* data = bmp.data();

		BITMAPFILEHEADER* fileHeader =
			reinterpret_cast<BITMAPFILEHEADER*>(data);

		if (fileHeader->bfType != 0x4D42) {
			MessageBoxA(nullptr,
				"Invalid BMP file.",
				"Error",
				MB_OK | MB_ICONERROR);
			return;
		}

		BITMAPINFOHEADER* infoHeader =
			reinterpret_cast<BITMAPINFOHEADER*>(
				data + sizeof(BITMAPFILEHEADER));

		if (infoHeader->biBitCount != 8) {
			MessageBoxA(nullptr,
				"BMP is not 8-bit.",
				"Error",
				MB_OK | MB_ICONERROR);
			return;
		}

		if (infoHeader->biCompression != BI_RGB) {
			MessageBoxA(nullptr,
				"Compressed BMPs are not supported.",
				"Error",
				MB_OK | MB_ICONERROR);
			return;
		}

		const int width = infoHeader->biWidth;
		const int height = abs(infoHeader->biHeight);

		// Palette starts immediately after the info header
		RGBQUAD* palette =
			reinterpret_cast<RGBQUAD*>(
				data +
				sizeof(BITMAPFILEHEADER) +
				infoHeader->biSize);

		// Pixel data
		BYTE* pixels = data + fileHeader->bfOffBits;

		// BMP rows are padded to multiples of 4 bytes
		const int stride = ((width + 3) & ~3);

		// Convert indexed pixels to RGB
		std::vector<BYTE> rgb(width * height * 3);

		const bool bottomUp = (infoHeader->biHeight > 0);

		for (int y = 0; y < height; y++) {
			int srcY = bottomUp ? (height - 1 - y) : y;

			BYTE* src = pixels + srcY * stride;

			for (int x = 0; x < width; x++) {
				RGBQUAD& c = palette[src[x]];

				int dst = (y * width + x) * 3;

				rgb[dst + 0] = c.rgbRed;
				rgb[dst + 1] = c.rgbGreen;
				rgb[dst + 2] = c.rgbBlue;
			}
		}

		// Upload to OpenGL

		gl.GenTextures(1, &texture);
		gl.BindTexture(gl.texture_2d, texture);

		gl.PixelStorei(gl.unpack_alignment, 1);

		gl.TexParameteri(gl.texture_2d,
			gl.texture_min_filter,
			gl.nearest);

		gl.TexParameteri(gl.texture_2d,
			gl.texture_mag_filter,
			gl.nearest);

		gl.TexParameteri(gl.texture_2d,
			gl.texture_wrap_s,
			gl.clamp);

		gl.TexParameteri(gl.texture_2d,
			gl.texture_wrap_t,
			gl.clamp);

		gl.TexImage2D(
			gl.texture_2d,
			0,
			gl.rgb,
			width,
			height,
			0,
			gl.rgb,
			gl.unsigned_byte,
			rgb.data());
	}

	 bool ConsoleWindow_ImplWin32GL_1_1::initWindowClass() {
		WNDCLASS wc{};
		wc.lpfnWndProc = [](HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam) -> LRESULT {
			static ConsoleWindow_ImplWin32GL_1_1* console = nullptr;
			switch (nMsg) {
			case WM_CLOSE:
				PostQuitMessage(0);
				break;
			case WM_NCCREATE:
			{
				CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);

				console = (ConsoleWindow_ImplWin32GL_1_1*)cs->lpCreateParams;
				break;
			}
			case WM_KEYDOWN:
			{
				//console->_priv_setNewKeyboard(wParam);
				break;
			}
			case WM_KEYUP:
			{
				//console->m_NewKeyboardCondition[wParam];
				break;
			}
			case WM_SETCURSOR:
			{
				SetCursor(console->hBlankCursor);
				break;
			}

			case WM_SIZE:
			{
				console->gl.Viewport(0, 0, LOWORD(lParam), HIWORD(lParam));
				console->dimension.x = LOWORD(lParam);
				console->dimension.y = HIWORD(lParam);
				break;
			}
			default:
				break;
			}
			if (console != nullptr) {
				return console->eventProcessor->processEvents_Win32Proc(hWnd, nMsg, wParam, lParam);
			}
			else {
				return DefWindowProc(hWnd, nMsg, wParam, lParam);
			}
			};
		wc.hInstance = GetModuleHandleA(nullptr);
		wc.lpszClassName = TEXT("TUIPP:ConsoleWindow_ImplWin32GL_1_1");

		if (!RegisterClass(&wc)) {
			MessageBox(nullptr, TEXT("Could not Init Window Class!"), TEXT("Error"), MB_OK | MB_ICONERROR);
			return false;
		}
		return true;
	}

	 bool ConsoleWindow_ImplWin32GL_1_1::createWindowInstance(const String& title, const Vec2<Word>& dimension) {
#if defined(TUIPP_USE_UTF8_STRING)
		this->nwh = CreateWindowA("TUIPP:ConsoleWindow_ImplWin32GL_1_1", (PCHAR)title.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, dimension.x, dimension.y, nullptr, nullptr, GetModuleHandleA(nullptr), nullptr);
#elif defined(TUIPP_USE_UTF16_STRING)

		this->nwh = CreateWindowW(L"TUIPP:ConsoleWindow_ImplWin32GL_1_1", (PWCHAR)title.c_str(), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, dimension.x, dimension.y, nullptr, nullptr, GetModuleHandleW(nullptr), this);
#endif
		if (this->nwh == nullptr) MessageBox(nullptr, TEXT("Could not Create Window!"), TEXT("Error"), MB_OK | MB_ICONERROR);
		return (nwh != nullptr);
	}

	 bool ConsoleWindow_ImplWin32GL_1_1::initGL_1_1() {
		hdc = GetDC(nwh);
		PIXELFORMATDESCRIPTOR pfd = {};
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags =
			PFD_DRAW_TO_WINDOW |
			PFD_SUPPORT_OPENGL |
			PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 24;
		pfd.cStencilBits = 8;
		pfd.iLayerType = PFD_MAIN_PLANE;

		Int32 format = ChoosePixelFormat(hdc, &pfd);
		SetPixelFormat(hdc, format, &pfd);

		glrc = wglCreateContext(hdc);

		if (!glrc) {
			return false;
		}
		wglMakeCurrent(hdc, glrc);

		HMODULE ogl = LoadLibraryA("opengl32.dll");

		gl.ClearColor = (GL::PFNCLEARCOLOR)GetProcAddress(ogl, "glClearColor");
		gl.Clear = (GL::PFNCLEAR)GetProcAddress(ogl, "glClear");

		gl.Begin = (GL::PFNBEGIN)GetProcAddress(ogl, "glBegin");
		gl.End = (GL::PFNEND)GetProcAddress(ogl, "glEnd");
		gl.Enable = (GL::PFNENABLE)GetProcAddress(ogl, "glEnable");
		gl.TexCoord2f = (GL::PFNTEXCOORD2F)GetProcAddress(ogl, "glTexCoord2f");
		gl.Vertex2f = (GL::PFNVERTEX2F)GetProcAddress(ogl, "glVertex2f");
		gl.GenTextures = (GL::PFNGENTEXTURES)GetProcAddress(ogl, "glGenTextures");
		gl.BindTexture = (GL::PFNBINDTEXTURE)GetProcAddress(ogl, "glBindTexture");
		gl.PixelStorei = (GL::PFNPIXELSTOREI)GetProcAddress(ogl, "glPixelStorei");
		gl.TexParameteri = (GL::PFNTEXPARAMETERI)GetProcAddress(ogl, "glTexParameteri");
		gl.TexImage2D = (GL::PFNTEXIMAGE2D)GetProcAddress(ogl, "glTexImage2D");
		gl.BlendFunc = (GL::PFNBLENDFUNC)GetProcAddress(ogl, "glBlendFunc");

		gl.Disable = (GL::PFNDISABLE)GetProcAddress(ogl, "glDisable");
		gl.Color3ub = (GL::PFNCOLOR3UB)GetProcAddress(ogl, "glColor3ub");
		gl.Color4ub = (GL::PFNCOLOR4UB)GetProcAddress(ogl, "glColor4ub");
		gl.Viewport = (GL::PFNVIEWPORT)GetProcAddress(ogl, "glViewport");
		gl.MatrixMode = (GL::PFNMATRIXMODE)GetProcAddress(ogl, "glMatrixMode");
		gl.LoadIdentity = (GL::PFNLOADIDENTITY)GetProcAddress(ogl, "glLoadIdentity");
		gl.Ortho = (GL::PFNORTHO)GetProcAddress(ogl, "glOrtho");

		FreeLibrary(ogl);
		return true;
	}

	 EventProcessor_ImplNativeWin32::EventProcessor_ImplNativeWin32(NativeHandle consoleInput) {
		hInput = consoleInput;
	}

	 ConsoleInputEvents::KeyStrokesCondition EventProcessor_ImplNativeWin32::keyboard(INT ID) {
		return cie.m_KeyboardCondition[ID];
	}

	 Vec2<Word> EventProcessor_ImplNativeWin32::getMousePos() {
		return cie.mousePos;
	}

	 ConsoleInputEvents::KeyStrokesCondition EventProcessor_ImplNativeWin32::mouse(Uint8 ID) {
		if (ID <= 3 && ID >= 0) {
			return cie.m_MouseCondition[ID];
		}
	}

	 void EventProcessor_ImplNativeWin32::processEvents() {
		//Handle KeyBoard Input
		for (INT i = 0; i < 256; i++) {
			cie.m_NewKeyboardCondition[i] = GetAsyncKeyState(i);

			cie.m_KeyboardCondition[i].bStrokePressed = FALSE;
			cie.m_KeyboardCondition[i].bStrokeReleased = FALSE;

			if (cie.m_NewKeyboardCondition[i] != cie.m_OldKeyboardCondition[i]) {
				if (cie.m_NewKeyboardCondition[i] & 0x8000) {
					cie.m_KeyboardCondition[i].bStrokePressed = !cie.m_KeyboardCondition[i].bStrokeIsHeld;
					cie.m_KeyboardCondition[i].bStrokeIsHeld = TRUE;
				}
				else {
					cie.m_KeyboardCondition[i].bStrokeReleased = TRUE;
					cie.m_KeyboardCondition[i].bStrokeIsHeld = FALSE;
				}
			}
			cie.m_OldKeyboardCondition[i] = cie.m_NewKeyboardCondition[i];
		}

		SetConsoleMode(hInput, ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT | ENABLE_EXTENDED_FLAGS);

		//Handle Mouse Input
		INPUT_RECORD inBuf[32];
		DWORD events = 0;
		GetNumberOfConsoleInputEvents(hInput, &events);
		if (events > 0)
			ReadConsoleInput(hInput, (PINPUT_RECORD)&inBuf, events, &events);

		for (DWORD i = 0; i < events; i++) {
			switch (inBuf[i].EventType) {
			case MOUSE_EVENT:
			{
				switch (inBuf[i].Event.MouseEvent.dwEventFlags) {
				case MOUSE_MOVED:
				{
					cie.mousePos.x = inBuf[i].Event.MouseEvent.dwMousePosition.X;
					cie.mousePos.y = inBuf[i].Event.MouseEvent.dwMousePosition.Y;
				}
				break;

				case 0:
				{
					for (int m = 0; m < 5; m++)
						cie.m_NewMouseCondition[m] = (inBuf[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;
				}
				break;

				default:
					break;
				}
			}
			break;

			default:
				break;

			}
		}
		for (int m = 0; m < 5; m++) {
			cie.m_MouseCondition[m].bStrokePressed = false;
			cie.m_MouseCondition[m].bStrokeReleased = false;

			if (cie.m_NewMouseCondition[m] != cie.m_OldMouseCondition[m]) {
				if (cie.m_NewMouseCondition[m]) {
					cie.m_MouseCondition[m].bStrokePressed = true;
					cie.m_MouseCondition[m].bStrokeIsHeld = true;
				}
				else {
					cie.m_MouseCondition[m].bStrokeReleased = true;
					cie.m_MouseCondition[m].bStrokeIsHeld = false;
				}
			}

			cie.m_OldMouseCondition[m] = cie.m_NewMouseCondition[m];
		}
	}

	 void EventProcessor_ImplNativeWin32::initEventProcessor(NativeHandle hConsoleInput) {
		hInput = hConsoleInput;
	}



	 EventProcessor_ImplWin32::EventProcessor_ImplWin32() {
		memset(&cie, 0, sizeof(ConsoleInputEvents));
	}

	 ConsoleInputEvents::KeyStrokesCondition EventProcessor_ImplWin32::keyboard(INT ID) {
		return cie.m_KeyboardCondition[ID];
	}

	 Vec2<Word> EventProcessor_ImplWin32::getMousePos() {
		return cie.mousePos;
	}

	 ConsoleInputEvents::KeyStrokesCondition EventProcessor_ImplWin32::mouse(Uint8 ID) {
		if (ID <= 3 && ID >= 0) {
			return cie.m_MouseCondition[ID];
		}
	}

	 Int64 EventProcessor_ImplWin32::processEvents_Win32Proc(NativeHandle nwh, Uint32 nMsg, Uint64 wParam, Int64 lParam) {
		switch (nMsg) {
		case WM_KEYDOWN:
		{
			cie.m_KeyboardCondition[wParam].bStrokePressed = TRUE;
			cie.m_KeyboardCondition[wParam].bStrokeIsHeld = TRUE;
			cie.m_KeyboardCondition[wParam].bStrokeReleased = FALSE;
			break;
		}
		case WM_KEYUP:
		{
			cie.m_KeyboardCondition[wParam].bStrokePressed = FALSE;
			cie.m_KeyboardCondition[wParam].bStrokeIsHeld = FALSE;
			cie.m_KeyboardCondition[wParam].bStrokeReleased = TRUE;
			break;
		}
		case WM_MOUSEMOVE:
		{
			Int32 x = GET_X_LPARAM(lParam);
			Int32 y = GET_Y_LPARAM(lParam);

			cie.mousePos.x = x / shared::pxlDimension.x;
			cie.mousePos.y = y / shared::pxlDimension.y;
			break;
		}

		case WM_LBUTTONDOWN:
		{
			cie.m_MouseCondition[cie.MouseLeft].bStrokePressed = TRUE;
			cie.m_MouseCondition[cie.MouseLeft].bStrokeIsHeld = TRUE;
			cie.m_MouseCondition[cie.MouseLeft].bStrokeReleased = FALSE;
			break;
		}
		case WM_LBUTTONUP:
		{
			cie.m_MouseCondition[cie.MouseLeft].bStrokePressed = FALSE;
			cie.m_MouseCondition[cie.MouseLeft].bStrokeIsHeld = FALSE;
			cie.m_MouseCondition[cie.MouseLeft].bStrokeReleased = TRUE;
			break;
		}
		case WM_RBUTTONDOWN:
		{
			cie.m_MouseCondition[cie.MouseRight].bStrokePressed = TRUE;
			cie.m_MouseCondition[cie.MouseRight].bStrokeIsHeld = TRUE;
			cie.m_MouseCondition[cie.MouseRight].bStrokeReleased = FALSE;
			break;
		}
		case WM_RBUTTONUP:
		{
			cie.m_MouseCondition[cie.MouseRight].bStrokePressed = FALSE;
			cie.m_MouseCondition[cie.MouseRight].bStrokeIsHeld = FALSE;
			cie.m_MouseCondition[cie.MouseRight].bStrokeReleased = TRUE;
			break;
		}
		case WM_MBUTTONDOWN:
		{
			cie.m_MouseCondition[cie.MouseMiddle].bStrokePressed = TRUE;
			cie.m_MouseCondition[cie.MouseMiddle].bStrokeIsHeld = TRUE;
			cie.m_MouseCondition[cie.MouseMiddle].bStrokeReleased = FALSE;
			break;
		}
		case WM_MBUTTONUP:
		{
			cie.m_MouseCondition[cie.MouseMiddle].bStrokePressed = FALSE;
			cie.m_MouseCondition[cie.MouseMiddle].bStrokeIsHeld = FALSE;
			cie.m_MouseCondition[cie.MouseMiddle].bStrokeReleased = TRUE;
			break;
		}
		case WM_SIZE:
		{
			cie.resized = true;
			cie.resize.x = LOWORD(lParam) / shared::pxlDimension.x;
			cie.resize.y = HIWORD(lParam) / shared::pxlDimension.y;
			break;
		}
		default:
			break;
		}
		return DefWindowProc(nwh, nMsg, wParam, lParam);
	}

#endif

	 Component::Component(Type cType, RenderElement& re, const String& id) :
		type(cType), re(&re), id(id), bTargeted(false) {

	}

	 void Component::setSize(const Word& width, const Word& height, bool resizeBuffer) {
		if (resizeBuffer) {
			int reqSize = width * height;
			Int32 size = re->viewport.width * re->viewport.height;
			if (size != reqSize) {
				Component::re->viewport.width = width;
				Component::re->viewport.height = height;
				CharInfo* _newBuffer = new CharInfo[reqSize]{};
				delete re->screenBuffer;
				re->screenBuffer = _newBuffer;
			}
		}
		Component::viewport.width = width;
		Component::viewport.height = height;
	}

	 void Component::setPosition(const Word& x, const Word& y) {
		Component::viewport.x = x;
		Component::viewport.y = y;
	}


	 Vec2<Word> Component::getSize() const {
		return Component::viewport.getSize();
	}

	 Vec2<Word> Component::getPosition() const {
		return Component::viewport.getPosition();
	}

	 void Component::setID(const String& id) {
		Component::id = id;
	}

	 String Component::getID() const {
		return Component::id;
	}
	 Vec2<Word> Component::getDefaultPosition() const {
		return Component::defaultPosition;
	}

	 Component::Type Component::getType() const {
		return Component::type;
	}

	 RenderElement& Component::getRenderElement() const {
		return *this->re;
	}
	 void Component::setComponentType(Type type) {
		this->type = type;
	}

	RenderTarget::RenderTarget(RenderElement* re) : re(*re) {

	}

	RenderTarget::~RenderTarget() {
		if (re.screenBuffer != nullptr) {
			delete re.screenBuffer;
			re.screenBuffer = nullptr;
		}
	}
	 void RenderTarget::flushTo(RenderTarget* out, Rect<Word> rect) {
		Vec2<Word> p1 = { rect.x, rect.y };
		Vec2<Word> p2 = { Word(rect.x + rect.width), Word(rect.y + rect.height) };
		for (Int16 x = p1.x; x < p2.x; x++) {
			for (Int16 y = p1.y; y < p2.y; y++) {
				Int16 px = (x - p1.x);
				Int16 py = (y - p1.y);

				auto pxl = RenderTarget::getPixelAt(px, py);
				out->setPixel(x, y, pxl.pixel.unicodeChar, pxl.attrib);
			}
		}
	}
	 void RenderTarget::flushTo(RenderTarget* out, Rect<Word> rect, Vec2<Word> start) {
		Vec2<Word> p1 = { rect.x, rect.y };
		Vec2<Word> p2 = { Word(rect.x + rect.width), Word(rect.y + rect.height) };
		for (Int16 x = p1.x; x < p2.x; x++) {
			for (Int16 y = p1.y; y < p2.y; y++) {
				Int16 px = (x - p1.x + start.x);
				Int16 py = (y - p1.y + start.y);

				auto pxl = RenderTarget::getPixelAt(px, py);
				out->setPixel(x, y, pxl.pixel.unicodeChar, pxl.attrib);
			}
		}
	}
	 void RenderTarget::setPixel(const Word& x, const Word& y, Int16 c, Int16 att) {
		if (re.viewport.contains(x, y)) {
			re.screenBuffer[y * re.viewport.width + x].pixel.unicodeChar = c;
			re.screenBuffer[y * re.viewport.width + x].attrib = att;
		}
	}

	 void RenderTarget::setPixel(const Vec2<Word>& p, Int16 c, Int16 att) {
		RenderTarget::setPixel(p.x, p.y, c, att);
	}

	 void RenderTarget::fill(const Word& x1, const Word& y1, const Word& x2, const Word& y2, Int16 c, Int16 att) {
		Int16 nx1 = x1;
		Int16 nx2 = x2;
		Int16 ny1 = y1;
		Int16 ny2 = y2;
		RenderTarget::calcClipOn(nx1, ny1);
		RenderTarget::calcClipOn(nx2, ny2);

		for (Word h = ny1; h < ny2; h++) {
			for (Word w = nx1; w < nx2; w++) {
				RenderTarget::setPixel(w, h, c, att);
			}
		}
	}

	 void RenderTarget::fill(const Vec2<Word>& p1, const Vec2<Word>& p2, Int16 c, Int16 att) {
		RenderTarget::fill(p1.x, p1.y, p2.x, p2.y, c, att);
	}

	 void RenderTarget::fill(const Rect<Word>& rect, Int16 c, Int16 att) {
		RenderTarget::fill(rect.getPosition(), rect.getSize(), c, att);
	}

	 void RenderTarget::renderLine(const Word& _x1, const Word& _y1, const Word& _x2, const Word& _y2, Int16 c, Int16 att) {
		Int32 x, y, dx, dy, dx1, dy1, px, py, xe, ye, i, x1, x2, y1, y2;
		x1 = static_cast<Int16>(_x1);
		x2 = static_cast<Int16>(_x2);
		y1 = static_cast<Int16>(_y1);
		y2 = static_cast<Int16>(_y2);
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

			RenderTarget::setPixel(x, y, c, att);

			for (i = 0; x < xe; i++) {
				x = x + 1;
				if (px < 0) {
					px = px + 2 * dy1;
				}
				else {
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1; else y = y - 1;
					px = px + 2 * (dy1 - dx1);
				}
				RenderTarget::setPixel(x, y, c, att);
			}
		}
		else {
			if (dy >= 0) {
				x = x1; y = y1; ye = y2;
			}
			else {
				x = x2; y = y2; ye = y1;
			}

			RenderTarget::setPixel(x, y, c, att);

			for (i = 0; y < ye; i++) {
				y = y + 1;
				if (py <= 0)
					py = py + 2 * dx1;
				else {
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x = x + 1; else x = x - 1;
					py = py + 2 * (dx1 - dy1);
				}
				RenderTarget::setPixel(x, y, c, att);
			}
		}
	}

	 void RenderTarget::renderLine(const Vec2<Word>& p1, const Vec2<Word>& p2, Int16 c, Int16 att) {
		RenderTarget::renderLine(p1.x, p1.y, p2.x, p2.y, c, att);
	}

	 void RenderTarget::renderLine(const Rect<Word>& p, Int16 c, Int16 att) {
		RenderTarget::renderLine(p.getPosition(), p.getSize(), c, att);
	}

	 void RenderTarget::renderTriangle(const Word& x1, const Word& y1, const Word& x2, const Word& y2, const Word& x3, const Word& y3, Int16 c, Int16 att) {
		RenderTarget::renderLine(x1, y1, x2, y2);
		RenderTarget::renderLine(x2, y2, x3, y3);
		RenderTarget::renderLine(x3, y3, x1, y1);
	}

	 void RenderTarget::renderTriangle(const Vec2<Word>& p1, const Vec2<Word>& p2, const Vec2<Word>& p3, Int16 c, Int16 att) {
		RenderTarget::renderTriangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, c, att);
	}

	 void RenderTarget::rasterizeTriangle(const Word& _x1, const Word& _y1, const Word& _x2, const Word& _y2, const Word& _x3, const Word& _y3, Int16 c, Int16 att) {
		Int16 x1 = _x1;
		Int16 x2 = _x2;
		Int16 x3 = _x3;
		Int16 y1 = _y1;
		Int16 y2 = _y2;
		Int16 y3 = _y3;

		auto SWAP = [](int16_t& x, int16_t& y) { int32_t t = x; x = y; y = t; };
		auto SWAP32 = [](int32_t& x, int32_t& y) { int32_t t = x; x = y; y = t; };
		auto drawline = [&](int32_t sx, int32_t ex, int32_t ny) { for (int32_t i = sx; i <= ex; i++) RenderTarget::setPixel(i, ny, c, att); };

		int32_t t1x, t2x, y, minx, maxx, t1xp, t2xp;
		bool changed1 = false;
		bool changed2 = false;
		int32_t signx1, signx2, dx1, dy1, dx2, dy2;
		int32_t e1, e2;
		// Sort vertices
		if (y1 > y2) { SWAP(y1, y2); SWAP(x1, x2); }
		if (y1 > y3) { SWAP(y1, y3); SWAP(x1, x3); }
		if (y2 > y3) { SWAP(y2, y3); SWAP(x2, x3); }

		t1x = t2x = x1; y = y1;   // Starting poINTs
		dx1 = (int32_t)(x2 - x1); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
		else signx1 = 1;
		dy1 = (int32_t)(y2 - y1);

		dx2 = (int32_t)(x3 - x1); if (dx2 < 0) { dx2 = -dx2; signx2 = -1; }
		else signx2 = 1;
		dy2 = (int32_t)(y3 - y1);

		if (dy1 > dx1) {   // swap values
			SWAP32(dx1, dy1);
			changed1 = true;
		}
		if (dy2 > dx2) {   // swap values
			SWAP32(dy2, dx2);
			changed2 = true;
		}

		e2 = (int32_t)(dx2 >> 1);
		// Flat top, just process the second half
		if (y1 == y2) goto next;
		e1 = (int32_t)(dx1 >> 1);

		for (int32_t i = 0; i < dx1;) {
			t1xp = 0; t2xp = 0;
			if (t1x < t2x) { minx = t1x; maxx = t2x; }
			else { minx = t2x; maxx = t1x; }
			// process first line until y value is about to change
			while (i < dx1) {
				i++;
				e1 += dy1;
				while (e1 >= dx1) {
					e1 -= dx1;
					if (changed1) t1xp = signx1;//t1x += signx1;
					else          goto next1;
				}
				if (changed1) break;
				else t1x += signx1;
			}
			// Move line
		next1:
			// process second line until y value is about to change
			while (1) {
				e2 += dy2;
				while (e2 >= dx2) {
					e2 -= dx2;
					if (changed2) t2xp = signx2;//t2x += signx2;
					else          goto next2;
				}
				if (changed2)     break;
				else              t2x += signx2;
			}
		next2:
			if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
			if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
			drawline(minx, maxx, y);    // Draw line from min to max poINTs found on the y
			// Now increase y
			if (!changed1) t1x += signx1;
			t1x += t1xp;
			if (!changed2) t2x += signx2;
			t2x += t2xp;
			y += 1;
			if (y == y2) break;

		}
	next:
		// Second half
		dx1 = (int32_t)(x3 - x2); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
		else signx1 = 1;
		dy1 = (int32_t)(y3 - y2);
		t1x = x2;

		if (dy1 > dx1) {   // swap values
			SWAP32(dy1, dx1);
			changed1 = true;
		}
		else changed1 = false;

		e1 = (int32_t)(dx1 >> 1);

		for (int32_t i = 0; i <= dx1; i++) {
			t1xp = 0; t2xp = 0;
			if (t1x < t2x) { minx = t1x; maxx = t2x; }
			else { minx = t2x; maxx = t1x; }
			// process first line until y value is about to change
			while (i < dx1) {
				e1 += dy1;
				while (e1 >= dx1) {
					e1 -= dx1;
					if (changed1) { t1xp = signx1; break; }//t1x += signx1;
					else          goto next3;
				}
				if (changed1) break;
				else   	   	  t1x += signx1;
				if (i < dx1) i++;
			}
		next3:
			// process second line until y value is about to change
			while (t2x != x3) {
				e2 += dy2;
				while (e2 >= dx2) {
					e2 -= dx2;
					if (changed2) t2xp = signx2;
					else          goto next4;
				}
				if (changed2)     break;
				else              t2x += signx2;
			}
		next4:

			if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
			if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
			drawline(minx, maxx, y);
			if (!changed1) t1x += signx1;
			t1x += t1xp;
			if (!changed2) t2x += signx2;
			t2x += t2xp;
			y += 1;
			if (y > y3) return;
		}
	}

	 void RenderTarget::rasterizeTriangle(const Vec2<Word>& p1, const Vec2<Word>& p2, const Vec2<Word>& p3, Int16 c, Int16 att) {
		RenderTarget::rasterizeTriangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, c, att);
	}

	 void RenderTarget::renderText(const Word& x, const Word& y, const String& txt, Int16 att) {
		for (int i = 0; i < txt.length(); i++) {
			RenderTarget::setPixel(x + i, y, txt[i], att);
		}
	}

	 void RenderTarget::renderText(const Vec2<Word>& p, const String& txt, Int16 att) {
		RenderTarget::renderText(p.x, p.y, txt, att);
	}

	 void RenderTarget::clear(Int16 c, Int16 att) {
		RenderTarget::fill(re.viewport, c, att);
	}

	 void RenderTarget::calcClipOn(Int16& x, Int16& y) {
		if (x < 0) x = 0;
		if (x > RenderTarget::re.viewport.width) x = RenderTarget::re.viewport.width;
		if (y < 0) y = 0;
		if (y > RenderTarget::re.viewport.height) y = RenderTarget::re.viewport.height;
	}

	 CharInfo RenderTarget::getPixelAt(const Word& x, const Word& y) const {
		if (re.viewport.contains(x, y)) {
			return re.screenBuffer[y * re.viewport.width + x];
		}
		else {
			return {};
		}
	}

	 CharInfo RenderTarget::getPixelAt(const Vec2<Word>& p) const {
		return getPixelAt(p.x, p.y);
	}

	 CharInfo* RenderTarget::getPixelScreenBuffer() const {
		return re.screenBuffer;
	}

	 EventProcessor::EventProcessor(NativeHandle CIN) {
		this->hInput = CIN;
	}
	 void EventProcessor::processEvents() {
		if (!this->isInit) {

			//Handle KeyBoard Input
			for (INT i = 0; i < 256; i++) {
				m_NewKeyboardCondition[i] = GetAsyncKeyState(i);

				m_KeyboardCondition[i].bStrokePressed = FALSE;
				m_KeyboardCondition[i].bStrokeReleased = FALSE;

				if (m_NewKeyboardCondition[i] != m_OldKeyboardCondition[i]) {
					if (m_NewKeyboardCondition[i] & 0x8000) {
						m_KeyboardCondition[i].bStrokePressed = !m_KeyboardCondition[i].bStrokeIsHeld;
						m_KeyboardCondition[i].bStrokeIsHeld = TRUE;
					}
					else {
						m_KeyboardCondition[i].bStrokeReleased = TRUE;
						m_KeyboardCondition[i].bStrokeIsHeld = FALSE;
					}
				}
				m_OldKeyboardCondition[i] = m_NewKeyboardCondition[i];
			}

			SetConsoleMode(hInput, ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT | ENABLE_EXTENDED_FLAGS);

			//Handle Mouse Input
			INPUT_RECORD inBuf[32];
			DWORD events = 0;
			GetNumberOfConsoleInputEvents(hInput, &events);
			if (events > 0)
				ReadConsoleInput(hInput, (PINPUT_RECORD)&inBuf, events, &events);

			for (DWORD i = 0; i < events; i++) {
				switch (inBuf[i].EventType) {
				case MOUSE_EVENT:
				{
					switch (inBuf[i].Event.MouseEvent.dwEventFlags) {
					case MOUSE_MOVED:
					{
						mousePos.x = inBuf[i].Event.MouseEvent.dwMousePosition.X;
						mousePos.y = inBuf[i].Event.MouseEvent.dwMousePosition.Y;
					}
					break;

					case 0:
					{
						for (int m = 0; m < 5; m++)
							m_NewMouseCondition[m] = (inBuf[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;
					}
					break;

					default:
						break;
					}
				}
				break;

				default:
					break;

				}
			}
			for (int m = 0; m < 5; m++) {
				m_MouseCondition[m].bStrokePressed = false;
				m_MouseCondition[m].bStrokeReleased = false;

				if (m_NewMouseCondition[m] != m_OldMouseCondition[m]) {
					if (m_NewMouseCondition[m]) {
						m_MouseCondition[m].bStrokePressed = true;
						m_MouseCondition[m].bStrokeIsHeld = true;
					}
					else {
						m_MouseCondition[m].bStrokeReleased = true;
						m_MouseCondition[m].bStrokeIsHeld = false;
					}
				}

				m_OldMouseCondition[m] = m_NewMouseCondition[m];
			}
		}
		else {
			//Handle KeyBoard Input
			for (INT i = 0; i < 256; i++) {
				m_KeyboardCondition[i].bStrokePressed = FALSE;
				m_KeyboardCondition[i].bStrokeReleased = FALSE;

				if (m_NewKeyboardCondition[i] != m_OldKeyboardCondition[i]) {
					if (m_NewKeyboardCondition[i] & 0x8000) {
						m_KeyboardCondition[i].bStrokePressed = !m_KeyboardCondition[i].bStrokeIsHeld;
						m_KeyboardCondition[i].bStrokeIsHeld = TRUE;
					}
					else {
						m_KeyboardCondition[i].bStrokeReleased = TRUE;
						m_KeyboardCondition[i].bStrokeIsHeld = FALSE;
					}
				}
				m_OldKeyboardCondition[i] = m_NewKeyboardCondition[i];
			}
		}
	}
	 void EventProcessor::initEventProcessor(NativeHandle hConsoleInput) {
		hInput = hConsoleInput;
		this->isInit = true;
	}
	 EventProcessor::KeyStrokesCondition EventProcessor::keyboard(INT ID) {
		return m_KeyboardCondition[ID];
	}
	 Vec2<Word> EventProcessor::getMousePos() {
		return mousePos;
	}
	 EventProcessor::KeyStrokesCondition EventProcessor::mouse(MouseType ID) {
		switch (ID) {
		case MouseType::Left:
			return m_MouseCondition[0];
		case MouseType::Right:
			return m_MouseCondition[1];
		case MouseType::Middle:
			return m_MouseCondition[2];
		default:
			break;
		}
	}

	 Panel::Panel() :
		Component(Component::Type::Panel, RenderTarget::re, _T("")) {
		isResizingLeft = isResizingBottom = isResizingRight = false;
	}

	 Panel::Panel(const String& title, const Word& width, const Word& height) :
		Component(Component::Type::Panel, RenderTarget::re, title) {
		isResizingLeft = isResizingBottom = isResizingRight = false;
		Panel::create(title, width, height);
	}

	 Panel::~Panel() {

	}



	 Intrusive<Component> Panel::createInstance(const String& title, const Word& width, const Word& height) {
		auto comp = Intrusive<Panel>::make(title, width, height);
		if (comp->getRenderElement().screenBuffer == nullptr) {
			return nullptr;
		}
		comp->setPosition(1, 1);
		return dynamicPtrCast<Component>(comp);
	}

	 bool Panel::create(const String& title, const Word& width, const Word& height) {
		Rect<Word> reqViewport = { 0, 0, 512, 512 };
		if (!reqViewport.contains(width, height)) {
			return false;
		}
		else {
			Component::setID(title);

			//Component.viewport is used to tell the panel size and location on the main console window or other tui components
			Component::viewport.width = width;
			Component::viewport.height = height;
			Panel::props.title = title;

			RenderTarget::re.screenBuffer = new CharInfo[width * height]{};
			
			//RenderElement viewport is used to tell where to render the content buffer to the panel
			RenderTarget::re.viewport = { 0, 0, width, height };
			return true;
		}
	}

	bool Panel::insertComponent(const Intrusive<Component>& comp) {
		auto cType = comp->getType();
		if (cType == Component::Panel) {
			components.push_back(comp);
		}
		return true;
	}

	void Panel::handleMousePress(
		Rect<Int16>& rect, 
		Vec2<Int16>& pPosition, 
		Vec2<Int16>& pOverallSize, 
		Vec2<Word>& pSize, 
		const Vec2<Int16>& mPos) {
		if (Panel::props.isMovable) {
			rect = {
				pPosition.x,
				Int16(pPosition.y - 1),
				pOverallSize.x,
				Int16(pPosition.y - 1)
			};
			if (rect.isInsideBounds(mPos)) {
				isDragging = true;
				offset = {
					Word(mPos.x - pPosition.x),
					Word(mPos.y - pPosition.y)
				};
			}
		}

		Panel::targeted = Panel::isHovering;

		if (Panel::props.isResizable) {
			//Resizable logic
			const Word resizeMargin = 0;
			////Right edge resizing
			//rect = {
			//	Int16(pOverallSize.x - resizeMargin),
			//	pPosition.y,
			//	pOverallSize.x,
			//	Int16(pOverallSize.y - 1)
			//};
			//if (rect.isInsideBounds(mPos)) {
			//	Panel::resizeDragging = true;
			//	Panel::isResizingRight = true;
			//	Panel::resizeOffset = {
			//		Word(mPos.x - pPosition.x),
			//		pSize.y
			//	};
			//}

			////Bottom edge resizing
			//rect = {
			//	Int16(pPosition.x + 1),
			//	Int16(pOverallSize.y - resizeMargin),
			//	Int16(pOverallSize.x - 1),
			//	Int16(pOverallSize.y)
			//};
			//if (rect.isInsideBounds(mPos)) {
			//	Panel::resizeDragging = true;
			//	Panel::isResizingBottom = true;
			//	Panel::resizeOffset = {
			//		Word(mPos.x - pPosition.x),
			//		pSize.y
			//	};
			//}

			//Left edge resizing 
			rect = {
				Int16(pPosition.x - 1),
				Int16(pPosition.y),
				Int16(pPosition.x + resizeMargin - 1),
				Int16(pOverallSize.y - 1)
			};
			if (rect.isInsideBounds(mPos)) {
				Panel::isResizingLeft = true;
				Panel::resizeDragging = true;
				Panel::resizeOffset = {
					Word(mPos.x - pPosition.x + 1),
					pSize.y
				};
			}

			// Right Bottom
			if (pOverallSize.x == mPos.x && pOverallSize.y == mPos.y) {
				Panel::isResizingRight = Panel::isResizingBottom = Panel::resizeDragging = true;
				Panel::resizeOffset = {
					Word(mPos.x - pPosition.x),
					Word(mPos.y - pPosition.y)
				};
			}

			if (pPosition.x - 1 == mPos.x && pOverallSize.y == mPos.y) {
				Panel::isResizingLeft = Panel::isResizingBottom = Panel::resizeDragging = true;
				Panel::resizeOffset = {
					Word(mPos.x - pPosition.x),
					Word(mPos.y - pPosition.y)
				};
			}
		}
	}

	void Panel::handleMouseDragging(
		Rect<Int16>& rect,
		Vec2<Int16>& pPosition,
		Vec2<Int16>& pOverallSize,
		Vec2<Word>& pSize,
		const Vec2<Int16>& mPos) {
		if (Panel::isDragging) {
			Panel::setPosition(mPos.x - offset.x, mPos.y - offset.y);
		}
		// handle resizing

		if (Panel::resizeDragging) {
			Int16 minWidth = 0;

			if (Panel::props.titleAlignment == TitleAlignment::Center) {
				minWidth = Panel::props.title.length() * 4;
			}
			else {
				minWidth = Panel::props.title.length() * 2;
			}

			const Int16 minHeight = 1;

			Int16 newX = pPosition.x;
			Int16 newWidth = pSize.x;
			Int16 newHeight = pSize.y;

			if (Panel::isResizingRight) {
				newWidth = mPos.x - pPosition.x;
				if (newWidth < minWidth) {
					newWidth = minWidth;
				}
			}

			// Left-edge resizing
			if (isResizingLeft) {
				Word leftDelta = pPosition.x - mPos.x;
				newWidth = pSize.x + leftDelta;

				if (newWidth >= minWidth) {
					// Adjust position only if the width is valid
					newX = mPos.x;
				}
				else {
					// Prevent width from going below the minimum
					newWidth = minWidth;
					newX = pOverallSize.x - minWidth;
				}
			}

			// Bottom-edge resizing
			if (isResizingBottom) {
				newHeight = mPos.y - pPosition.y;
				if (newHeight < minHeight)
					newHeight = minHeight;
			}

			Panel::setPosition(newX, pPosition.y);
			Panel::setSize(newWidth, newHeight, false);
		}
	}

	 void Panel::onUpdate(const ConsoleInputEvents& ep) {
		Vec2<Int16> pPosition = { Int16(Panel::getPosition().x), Int16(Panel::getPosition().y) };
		Vec2<Word> pSize = Panel::getSize();
		Vec2<Int16> pOverallSize = Vec2<Int16>(pPosition.x + pSize.x, pPosition.y + pSize.y);
		Vec2<Int16> mPos = Vec2<Int16>(ep.mousePos.x, ep.mousePos.y);
		
		{
			ConsoleInputEvents cie = ep;
			cie.mousePos.x = ep.mousePos.x - pPosition.x;
			cie.mousePos.y = ep.mousePos.y - pPosition.y;
			for (auto& component : Panel::components) {
				component->onUpdate(cie);
			}
		}

		//Check if the mouse is hovering over the panel
		Rect<Int16> rect(pPosition, pOverallSize);
		if (rect.isInsideBounds(mPos)) {
			Panel::mousePosition = {
				Word(mPos.x - pPosition.x),
				Word(mPos.y - pPosition.y)
			};

			Panel::isHovering = true;
		}
		else {
			Panel::isHovering = false;
		}

		// Handle Mouse press events
		if (ep.m_MouseCondition[ep.MouseLeft].bStrokePressed) {
			Panel::handleMousePress(rect, pPosition, pOverallSize, pSize, mPos);
		}

		// handle dragging
		if (ep.m_MouseCondition[ep.MouseLeft].bStrokeIsHeld) {
			Panel::handleMouseDragging(rect, pPosition, pOverallSize, pSize, mPos);
		}

		// handle mouse release
		if (ep.m_MouseCondition[ep.MouseLeft].bStrokeReleased) {
			Panel::isDragging = false;
			Panel::resizeDragging = false;
			this->isResizingLeft = this->isResizingBottom = this->isResizingRight = false;
		}

		auto mouseState = ep.m_MouseCondition[0]; // left

		const Rectw panelRect(Component::getPosition(), Component::getSize());
		Vec2<Int32> local(mPos.x - panelRect.x, mPos.y - panelRect.y);

		if (Panel::targeted) {
			
			const Int32 viewport = panelRect.height;
			const Int32 content = (Int32)Panel::getDisplayBufferSize().y;

			const Int32 maxScroll = std::max(0, content - viewport);

			if (maxScroll > 0) {
				const Int32 thumbHeight = std::max(1, viewport * viewport / content);

				const Int32 thumbTravel = std::max(1, viewport - thumbHeight);

				const Int32 thumbY = verticalScroll * thumbTravel / maxScroll;

				const bool onThumb = local.x == panelRect.width &&
					local.y >= thumbY &&
					local.y < thumbY + thumbHeight;

				if (mouseState.bStrokePressed && onThumb) {
					isDraggingScrollBar = true;

					dragStartY = local.y - thumbY;
				}

				if (mouseState.bStrokeReleased) {
					isDraggingScrollBar = false;
				}

				if (isDraggingScrollBar && mouseState.bStrokeIsHeld) {
					Int32 newThumbY = local.y - dragStartY;

					newThumbY = std::clamp(newThumbY, 0, thumbTravel);

					verticalScroll = newThumbY * maxScroll / thumbTravel;

					verticalScroll = std::clamp(verticalScroll, 0, maxScroll);
				}
			}

			// horizontal scrollbar
			const int viewportWidth = (int)panelRect.width;
			const int contentWidth = (int)Panel::getDisplayBufferSize().x;

			const int maxHorizontalScroll = std::max(0, contentWidth - viewportWidth);

			if (maxHorizontalScroll > 0) {
				const int thumbWidth =
					std::max(1, viewportWidth * viewportWidth / contentWidth);

				const int thumbTravel =
					viewportWidth - thumbWidth;

				const int thumbX =
					(horizontalScroll * thumbTravel) / maxHorizontalScroll;


				// Mouse is on the horizontal thumb
				const bool onThumb =
					local.y == panelRect.height&&
					local.x >= thumbX &&
					local.x < thumbX + thumbWidth;


				if (mouseState.bStrokePressed && onThumb) {
					isDraggingHScrollBar = true;

					// Keep the click position inside the thumb
					dragStartX = local.x - thumbX;
				}


				if (mouseState.bStrokeReleased) {
					isDraggingHScrollBar = false;
				}


				if (isDraggingHScrollBar && mouseState.bStrokeIsHeld) {
					int newThumbX = local.x - dragStartX;

					newThumbX = std::clamp(
						newThumbX,
						0,
						thumbTravel
					);


					horizontalScroll =
						newThumbX * maxHorizontalScroll / thumbTravel;


					horizontalScroll =
						std::clamp(
							horizontalScroll,
							0,
							maxHorizontalScroll
						);
				}
			}
		}
	}

	 void Panel::onRender(RenderTarget* out) {
		Vec2<Word> pPosition = Panel::getPosition();
		Vec2<Word> pSize = Panel::getSize();
		Vec2<Word> pOverallSize = Vec2<Word>(pPosition.x + pSize.x, pPosition.y + pSize.y);
		
		//RenderTarget::clear(0x2588, 0x11);

		Rect<Word> viewport_out(horizontalScroll, verticalScroll, Component::getSize().x, Component::getSize().y);

		for (auto& component : Panel::components) {
			component->onRender(this);
		}

		RenderTarget::flushTo(out, Component::viewport, Vec2<Word>(horizontalScroll, verticalScroll));

		if (Panel::targeted) {
			Panel::setUpFrame(out, Component::viewport, Panel::props.borderColor);
		}
		else {
			Panel::setUpFrame(out, Component::viewport, 0x08);
		}

		switch (Panel::props.titleAlignment) {
		default:
			break;

		case TitleAlignment::Left:
			out->renderText(pPosition.x + Panel::props.titleAlignmentOffset + (Panel::props.title.length() * 0.25), pPosition.y - 1, Panel::props.title, 0x000F);
			break;
		case TitleAlignment::Center:
			out->renderText(pPosition.x + (pSize.x / 2u) - (Panel::props.title.length() / 2), pPosition.y - 1, Panel::props.title, 0x000F);
			break;
		case TitleAlignment::Right:
			out->renderText(pOverallSize.x + Panel::props.titleAlignmentOffset - (Panel::props.title.length() * 1.25), pPosition.y - 1, Panel::props.title, 0x000F);
			break;
		};
		
		const Int32 contentHeight = (Int32)Panel::getDisplayBufferSize().y;
		const Int32 viewportHeight = (Int32)Component::viewport.height;

		if (contentHeight > viewportHeight) {
			const Int32 scrollbarX = pOverallSize.x;

			const Int32 maxScroll = contentHeight - viewportHeight;
			const Int32 thumbHeight = std::max(1, viewportHeight * viewportHeight / contentHeight);

			const Int32 thumbTravel = viewportHeight - thumbHeight;

			const Int32 thumbY = (verticalScroll * thumbTravel) / maxScroll;

			for (Int32 y = 0; y < thumbHeight; y++) {
				out->setPixel(scrollbarX, pPosition.y + thumbY + y, 0x2588, 0x77);
			}
		}

		//
		// Horizontal scrollbar
		//
		const int contentWidth = (int)Panel::getDisplayBufferSize().x;
		const int viewportWidth = (int)Component::viewport.width;

		if (contentWidth > viewportWidth) {
			const int scrollbarY = pOverallSize.y;

			const int maxScroll = contentWidth - viewportWidth;

			const int thumbWidth = std::max(1, viewportWidth * viewportWidth / contentWidth);

			const int thumbTravel = viewportWidth - thumbWidth;

			const int thumbX = (horizontalScroll * thumbTravel) / maxScroll;


			// Thumb
			for (int x = 0; x < thumbWidth; x++) {
				out->setPixel(
					pPosition.x + thumbX + x,
					scrollbarY,
					0x2588,
					0x77
				);
			}
		}
	}

	 void Panel::onInit() {

	}

	 Vec2<Word> Panel::getMousePos() const {
		return mousePosition;
	}

	 Panel::Properties& Panel::getProperties() {
		return props;
	}

	 void Panel::setDisplayBufferSize(const Word& width, const Word& height) {
		 RenderTarget::re.viewport.width = width;
		 RenderTarget::re.viewport.height = height;
		 delete[] RenderTarget::re.screenBuffer;
		 RenderTarget::re.screenBuffer = new CharInfo[width * height]{};
	 }
	 void Panel::setDisplayBufferSize(const Vec2w& size) {
		 setDisplayBufferSize(size.x, size.y);
	 }

	 Vec2w Panel::getDisplayBufferSize() const {
		 return RenderTarget::re.viewport.getSize();
	 }

	 void Panel::setUpFrame(RenderTarget* out, Rect<Word> rect, Word color) {

		out->renderLine(rect.x - 1, rect.y - 1, rect.x + rect.width, rect.y - 1, 0x2500, color);

		out->renderLine(rect.x - 1, rect.y - 1, rect.x - 1, rect.y + rect.height - 1, 0x2502, color);

		out->renderLine(rect.x - 1, rect.y + rect.height, rect.x + rect.width, rect.y + rect.height, 0x2500, color);

		out->renderLine(rect.x + rect.width, rect.y - 1, rect.x + rect.width, rect.y + rect.height - 1, 0x2502, color);

		out->setPixel(rect.x - 1, rect.y - 1, 0x256D, color);
		out->setPixel(rect.x + rect.width, rect.y - 1, 0x256E, color);
		out->setPixel(rect.x - 1, rect.y + rect.height, 0x2570, color);
		out->setPixel(rect.x + rect.width, rect.y + rect.height, 0x256F, color);
	}

	 Console::Console(const String& title, const Vec2<Word>& dimension, const Vec2<Word>& pxlDimension, Type type) :
		interface(nullptr), type(type) {
		shared::pxlDimension = pxlDimension;
#if defined(_WIN32) || defined(_WIN64)
		if (Console::type == Console::Type::NativeOS) {
			g_re = &this->re;
			Console::interface = std::make_shared<ConsoleWindow_ImplNativeWin32>(nullptr, title, Vec2<Word>(dimension.x * pxlDimension.x, dimension.y * pxlDimension.y));
			if (Console::interface->isOpen()) {
				Console::hInput = (NativeHandle)GetStdHandle(STD_INPUT_HANDLE);
				Console::hOutput = (NativeHandle)GetStdHandle(STD_OUTPUT_HANDLE);

				SMALL_RECT rect = { 0, 0, 1, 1 };
				SetConsoleWindowInfo(hOutput, TRUE, &rect);

				if (!SetConsoleScreenBufferSize(hOutput, { (short)dimension.x, (short)dimension.y })) {
					MessageBoxA(nullptr, "Couldn't set the console screen buffer size!", "Error", MB_ICONERROR | MB_OK);
				}

				SetConsoleActiveScreenBuffer(hOutput);

				CONSOLE_FONT_INFOEX cfi{};
				cfi.cbSize = sizeof(cfi);
				cfi.nFont = 0;
				cfi.dwFontSize.X = pxlDimension.x;
				cfi.dwFontSize.Y = pxlDimension.y;
				cfi.FontFamily = FF_DONTCARE;
				cfi.FontWeight = FW_NORMAL;

				wcscpy_s(cfi.FaceName, L"Consolas");
				SetCurrentConsoleFontEx(Console::getOutputHandle(), FALSE, &cfi);

				Console::viewport.x = 0;
				Console::viewport.y = 0;
				Console::viewport.width = dimension.x - 1;
				Console::viewport.height = dimension.y - 1;
				if (!SetConsoleWindowInfo(Console::getOutputHandle(), TRUE, reinterpret_cast<PSMALL_RECT>(&viewport))) {
					// we force the size
					Console::getInterface()->setSize(Vec2<Word>(dimension.x * pxlDimension.x, dimension.y * pxlDimension.y));
				}

				SetConsoleMode(Console::getInputHandle(), ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);

				Console::re.hConsole = hOutput;
				Console::re.viewport = viewport;
				Console::re.screenBuffer = new CharInfo[viewport.width * viewport.height]{};
				Console::eventProcessor = std::make_shared<EventProcessor_ImplNativeWin32>();

				Console::eventProcessor->initEventProcessor(hInput);
			}

		}
		else if (Console::type == Console::Type::GL_1_1) {
			Console::eventProcessor = std::make_shared<EventProcessor_ImplWin32>();
			Console::interface = std::make_shared<ConsoleWindow_ImplWin32GL_1_1>(nullptr, title, Vec2<Word>(dimension.x * pxlDimension.x, dimension.y * pxlDimension.y), pxlDimension, std::dynamic_pointer_cast<EventProcessor_ImplWin32>(eventProcessor));

			Console::viewport.x = 0;
			Console::viewport.y = 0;
			Console::viewport.width = dimension.x;
			Console::viewport.height = dimension.y;

			Console::re.viewport = viewport;
			Console::re.screenBuffer = new CharInfo[viewport.width * viewport.height]{};
		}
#else
#error UNIX based system isnt supported at this moment!
#endif
	}
	 Console::~Console() {

	}

	 void Console::setSize(const Vec2<Word>& newSize) {
		Console::setSize(newSize.x, newSize.y);
	}


	 void Console::setSize(const Word& newSizeX, const Word& newSizeY) {
		Console::viewport.width = newSizeX;
		Console::viewport.height = newSizeY;

		Console::re.viewport = viewport;
		delete Console::re.screenBuffer;
		Console::re.screenBuffer = new CharInfo[viewport.width * viewport.height]{};
	}

	 std::shared_ptr<ConsoleInterface> Console::getInterface() {
		return interface;
	}

	 std::shared_ptr<EventProcessorInterface> Console::getEventProcessor() {
		return eventProcessor;
	}

	 NativeHandle Console::getInputHandle() {
		return hInput;
	}

	 NativeHandle Console::getOutputHandle() {
		return hOutput;
	}
	 bool Console::insertComponent(const Intrusive<Component>& component) {
		components.push_back(component);
		component->onInit();
		return true;
	}
	 void Console::display() {
		eventProcessor->processEvents();

		for (Int32 i = 0; i < Console::components.size(); i++) {
			auto& comp = Console::components[i];
			comp->onUpdate(eventProcessor->cie);

			comp->onRender(this);
		}
		Console::setPixel(eventProcessor->getMousePos());
		if (Console::interface != nullptr) {
			Console::interface->display(re);
		}
	}
	 bool Console::isOpen() {
		if (Console::interface != nullptr) {
			return Console::interface->isOpen();
		}
		return false;
	}
	 void Console::close() {
		if (Console::interface != nullptr) {
			Console::interface->close();
		}
	}
	 Rect<Word> Console::getViewport() const {
		return viewport;
	}
}
