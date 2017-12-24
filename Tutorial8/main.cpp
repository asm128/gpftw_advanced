// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
//
#include "ftw_ascii_display.h"
#include "ftw_frameinfo.h"
#include "ftw_timer.h"
#include "ftw_bitmap_target.h"

#include <windows.h>    // for interacting with Windows

static constexpr	const int																		SCREEN_WIDTH						= 132	;
static constexpr	const int																		SCREEN_HEIGHT						= 50	;

struct SInput {
						uint8_t																			KeyboardPrevious	[256]			= {};
						uint8_t																			KeyboardCurrent		[256]			= {};
};

struct SRuntimeValues {
						::HINSTANCE																		hInstance							= {}; 
						::HINSTANCE																		hPrevInstance						= {}; 
						::LPSTR																			lpCmdLine							= {}; 
						::INT																			nShowCmd							= {}; 
};

struct SApplication {
						::HWND																			MainWindowHandle					= {};
						::WNDCLASSEX																	MainWindowClass						= {};
	static constexpr	const char																		MainWindowClassName	[256]			= "FTWL_WINDOW";
						::SRuntimeValues																RuntimeValues						= {};
						::SInput																		SystemInput							= {};
						::ftwl::SBitmapOffscreen<::ftwl::SColorBGRA, ::SCREEN_WIDTH, ::SCREEN_HEIGHT>	BitmapOffsceen						= {};
						::ftwl::STimer																	Timer								= {};
						::ftwl::SFrameInfo																FrameInfo							= {};
						::ftwl::SASCIITarget															ASCIIRenderTarget					= {};
						::ftwl::SBitmapTargetRGBA														BitmapRenderTarget					= {};
						::ftwl::SPalette																Palette								= 
		{	(uint32_t)::ftwl::ASCII_COLOR_INDEX_0		
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_1 	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_2 	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_3 	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_4 	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_5 	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_6 	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_7 	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_8 	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_9 	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_10	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_11	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_12	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_13	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_14	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_15	
		};

																									SApplication								(SRuntimeValues& runtimeValues)													: RuntimeValues(runtimeValues) {}
};
	
static	::SApplication																			* g_ApplicationInstance						= 0;

	
		::ftwl::error_t																			drawBuffer									(HDC hdc, int width, int height, ::ftwl::SColorBGRA *colorArray) {
	const uint32_t																						bytesToCopy									= sizeof(::RGBQUAD) * width * height;
	const uint32_t																						bitmapInfoSize								= sizeof(::BITMAPINFO) + bytesToCopy;
	::BITMAPINFO																						* bi										= (::BITMAPINFO*)::malloc(bitmapInfoSize);
	for(uint32_t iPixel = 0, pixelCount = width * height; iPixel < pixelCount; ++iPixel)
		((::RGBQUAD*)bi->bmiColors)[iPixel]																= {colorArray[iPixel].b, colorArray[iPixel].g, colorArray[iPixel].r, 0xFF};

	bi->bmiHeader.biSize																			= sizeof(::BITMAPINFO);
	bi->bmiHeader.biWidth																			= width;
	bi->bmiHeader.biHeight																			= height;
	bi->bmiHeader.biPlanes																			= 1;
	bi->bmiHeader.biBitCount																		= 32;
	bi->bmiHeader.biCompression																		= BI_RGB;
	bi->bmiHeader.biSizeImage																		= width * height * sizeof(::RGBQUAD);
	bi->bmiHeader.biXPelsPerMeter																	= 0x0ec4; // Paint and PSP use these values.
	bi->bmiHeader.biYPelsPerMeter																	= 0x0ec4; // Paint and PSP use these values.
	bi->bmiHeader.biClrUsed																			= 0; 
	bi->bmiHeader.biClrImportant																	= 0;

	::HDC																								dcIntermediate								= ::CreateCompatibleDC(hdc);    // <- note, we're creating, so it needs to be destroyed
	char																								* ppvBits									= 0;
	::HBITMAP																							hBitmap										= ::CreateDIBSection(dcIntermediate, bi, DIB_RGB_COLORS, (void**) &ppvBits, NULL, 0);
	if(0 == ::SetDIBits(NULL, hBitmap, 0, height, bi->bmiColors, bi, DIB_RGB_COLORS)) {
		OutputDebugString("Cannot copy bits into dib section.\n");
		return -1;
	}
	::HBITMAP																							hBmpOld										= (::HBITMAP)::SelectObject(dcIntermediate, hBitmap);    // <- altering state
	if(FALSE == ::BitBlt(hdc, 0, 0, width, height, dcIntermediate, 0, 0, SRCCOPY)) {
		char buffer[256] = {};
		::sprintf_s(buffer, "error blitting bitmap: 0x%x.\n", ::GetLastError());
		OutputDebugString(buffer);
		::va_list arguments = {};
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, ::GetLastError(), 0, buffer, 256, &arguments);
		OutputDebugString(buffer);
	}  // <- no significnce

	//// this is how you clean up:
	//::SelectObject(hdc, hBmpOld);  // put the old bitmap back in the DC (restore state)
	//::DeleteObject(hBitmap);  // delete the bitmap we created
	::DeleteObject(hBmpOld);  // delete the bitmap we created
	::DeleteDC(dcIntermediate);  // delete the DC we created
	::free(bi);
	return 0;
}

LRESULT WINAPI																					mainWndProc									(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)							{
	::SApplication																						& applicationInstance						= *g_ApplicationInstance;
	switch(uMsg) {
	default: break;		
	case WM_GETMINMAXINFO	: ((MINMAXINFO*)lParam)->ptMinTrackSize = {::SCREEN_WIDTH, ::SCREEN_HEIGHT}; return 0;	// Catch this message so to prevent the window from becoming too small.
	case WM_PAINT	: 
		//::PAINTSTRUCT ps;
		//::BeginPaint(hWnd, &ps);
		//::drawBuffer(::GetDC(applicationInstance.MainWindowHandle), applicationInstance.BitmapOffsceen.Width, applicationInstance.BitmapOffsceen.Height, &applicationInstance.BitmapOffsceen.Colors[0][0]);
		//::EndPaint(hWnd, &ps);
		//return 0;
		break;
	case WM_CLOSE	: DestroyWindow		(hWnd)	; return 0;
	case WM_DESTROY	: 
		::PostQuitMessage	(0); 
		applicationInstance.MainWindowHandle															= 0;
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void																							initWndClass								(::HINSTANCE hInstance, const char* className, ::WNDCLASSEX& wndClassToInit)	{
	wndClassToInit																					= {sizeof(::WNDCLASSEX),};
	wndClassToInit.lpfnWndProc																		= ::mainWndProc;
	wndClassToInit.hInstance																		= hInstance;
	wndClassToInit.hCursor																			= LoadCursor(NULL, IDC_ARROW);
	wndClassToInit.hbrBackground																	= (::HBRUSH)(COLOR_3DFACE + 1);
	wndClassToInit.lpszClassName																	= className;
}

void																							updateMainWindow							(::SApplication& applicationInstance)											{ 
	::MSG																								msg											= {};
	while(PeekMessage(&msg, applicationInstance.MainWindowHandle, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage	(&msg);
		if(msg.message == WM_QUIT) {
			applicationInstance.MainWindowHandle															= 0;
			break;
		}
	}
}

// --- Cleanup application resources.
void																							cleanup										(::SApplication& applicationInstance)											{ 
	if(applicationInstance.MainWindowHandle) {
		::DestroyWindow(applicationInstance.MainWindowHandle);
		::updateMainWindow(applicationInstance);
	}

	UnregisterClass(applicationInstance.MainWindowClassName, applicationInstance.MainWindowClass.hInstance);
	::ftwl::asciiDisplayDestroy	();
	::ftwl::asciiTargetDestroy	(applicationInstance.ASCIIRenderTarget);
	g_ApplicationInstance																			= 0;
}

// --- Initialize console.
void																							setup										(::SApplication& applicationInstance)											{ 
	g_ApplicationInstance																			= &applicationInstance;
	::ftwl::asciiTargetCreate	(applicationInstance.ASCIIRenderTarget, ::SCREEN_WIDTH, ::SCREEN_HEIGHT);
	::ftwl::asciiDisplayCreate	(applicationInstance.ASCIIRenderTarget.Width(), applicationInstance.ASCIIRenderTarget.Height());
	::initWndClass(applicationInstance.RuntimeValues.hInstance, applicationInstance.MainWindowClassName, applicationInstance.MainWindowClass);
	RegisterClassEx(&applicationInstance.MainWindowClass);
	applicationInstance.MainWindowHandle															= CreateWindow(applicationInstance.MainWindowClassName, "Window FTW", WS_OVERLAPPEDWINDOW, 10, 10, ::SCREEN_WIDTH, ::SCREEN_HEIGHT, 0, 0, applicationInstance.MainWindowClass.hInstance, 0);
	::ShowWindow	(applicationInstance.MainWindowHandle, SW_SHOW);
	::UpdateWindow	(applicationInstance.MainWindowHandle);
}

void																							update										(::SApplication& applicationInstance)											{ 
	::ftwl::asciiTargetClear(applicationInstance.ASCIIRenderTarget);
	applicationInstance.Timer.Frame();
	applicationInstance.FrameInfo.Frame(applicationInstance.Timer.LastTimeMicroseconds);																						
	::updateMainWindow(applicationInstance);
}

void																							draw										(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::ftwl::SASCIITarget																				& asciiTarget								= applicationInstance.ASCIIRenderTarget;
	uint32_t																							color0										= (0xFF & applicationInstance.FrameInfo.FrameNumber * 1) | ((0xFF & applicationInstance.FrameInfo.FrameNumber * 2) << 8) | ((0xFF & applicationInstance.FrameInfo.FrameNumber * 5) << 16);
	uint32_t																							color1										= (0xFF & applicationInstance.FrameInfo.FrameNumber * 2) | ((0xFF & applicationInstance.FrameInfo.FrameNumber * 1) << 8) | ((0xFF & applicationInstance.FrameInfo.FrameNumber * 3) << 16);
	uint32_t																							color2										= (0xFF & applicationInstance.FrameInfo.FrameNumber * 3) | ((0xFF & applicationInstance.FrameInfo.FrameNumber * 5) << 8) | ((0xFF & applicationInstance.FrameInfo.FrameNumber * 2) << 16);
	uint32_t																							color3										= (0xFF & applicationInstance.FrameInfo.FrameNumber * 5) | ((0xFF & applicationInstance.FrameInfo.FrameNumber * 3) << 8) | ((0xFF & applicationInstance.FrameInfo.FrameNumber * 1) << 16);
	applicationInstance.Palette[0]																	= color0;
	applicationInstance.Palette[1]																	= color1;
	applicationInstance.Palette[2]																	= color2;
	applicationInstance.Palette[3]																	= color3;
	::ftwl::asciiDisplayPaletteSet({&applicationInstance.Palette[0], 16});

	::ftwl::SCoord2		<int32_t>																		screenCenter								= {::SCREEN_WIDTH / 2, ::SCREEN_HEIGHT / 2};
	::ftwl::SRectangle2D<int32_t>																		geometry0									= {{2, 2}, {(int32_t)((applicationInstance.FrameInfo.FrameNumber / 2) % (::SCREEN_WIDTH - 2)), 5}};
	::ftwl::SCircle2D	<int32_t>																		geometry1									= {5.0 + (applicationInstance.FrameInfo.FrameNumber / 5) % 5, screenCenter};	
	::ftwl::STriangle2D	<int32_t>																		geometry2									= {{0, 0}, {15, 15}, {-5, 10}};	
	geometry2.A																						+= screenCenter + ::ftwl::SCoord2<int32_t>{(int32_t)geometry1.Radius, (int32_t)geometry1.Radius};
	geometry2.B																						+= screenCenter + ::ftwl::SCoord2<int32_t>{(int32_t)geometry1.Radius, (int32_t)geometry1.Radius};
	geometry2.C																						+= screenCenter + ::ftwl::SCoord2<int32_t>{(int32_t)geometry1.Radius, (int32_t)geometry1.Radius};
	::ftwl::drawRectangle	(asciiTarget, {'!', ::ftwl::ASCII_COLOR_CYAN		}, geometry0);
	::ftwl::drawRectangle	(asciiTarget, {'!', ::ftwl::ASCII_COLOR_BLUE		}, {geometry0.Offset + ::ftwl::SCoord2<int32_t>{1, 1}, geometry0.Size - ::ftwl::SCoord2<int32_t>{2, 2}});
	::ftwl::drawCircle		(asciiTarget, {'?', ::ftwl::ASCII_COLOR_GREEN		}, geometry1);
	::ftwl::drawCircle		(asciiTarget, {'?', ::ftwl::ASCII_COLOR_RED			}, {geometry1.Radius - 1, geometry1.Center});
	::ftwl::drawTriangle	(asciiTarget, {'o', ::ftwl::ASCII_COLOR_YELLOW		}, geometry2);
	::ftwl::drawLine		(asciiTarget, {'o', ::ftwl::ASCII_COLOR_MAGENTA		}, ::ftwl::SLine2D<int32_t>{geometry2.A, geometry2.B});
	::ftwl::drawLine		(asciiTarget, {'o', ::ftwl::ASCII_COLOR_WHITE		}, ::ftwl::SLine2D<int32_t>{geometry2.B, geometry2.C});
	::ftwl::drawLine		(asciiTarget, {'o', ::ftwl::ASCII_COLOR_LIGHTGREY	}, ::ftwl::SLine2D<int32_t>{geometry2.C, geometry2.A});

	::ftwl::SBitmapTargetBGRA																			bmpTarget									= {};
	bmpTarget.Colors																				= ::ftwl::grid_view<::ftwl::SColorBGRA>(&applicationInstance.BitmapOffsceen.Colors[0][0], applicationInstance.BitmapOffsceen.Width, applicationInstance.BitmapOffsceen.Height);
	::ftwl::drawRectangle	(bmpTarget, {0x80, 0x80, 0x80, 0x00}, {{0, 0}, {::SCREEN_WIDTH, ::SCREEN_HEIGHT}});
	::ftwl::drawRectangle	(bmpTarget, {0xFF, 0x00, 0x00, 0x00}, geometry0);
	::ftwl::drawRectangle	(bmpTarget, {0x00, 0xFF, 0x00, 0x00}, {geometry0.Offset + ::ftwl::SCoord2<int32_t>{1, 1}, geometry0.Size - ::ftwl::SCoord2<int32_t>{2, 2}});
	::ftwl::drawCircle		(bmpTarget, {0x00, 0x00, 0xFF, 0x00}, geometry1);
	::ftwl::drawCircle		(bmpTarget, {0xFF, 0x00, 0xFF, 0x00}, {geometry1.Radius - 1, geometry1.Center});
	::ftwl::drawTriangle	(bmpTarget, {0xFF, 0xFF, 0x00, 0x00}, geometry2);
	::ftwl::drawLine		(bmpTarget, {0x00, 0xFF, 0xFF, 0x00}, ::ftwl::SLine2D<int32_t>{geometry2.A, geometry2.B});
	::ftwl::drawLine		(bmpTarget, {0xFF, 0xFF, 0xFF, 0x00}, ::ftwl::SLine2D<int32_t>{geometry2.B, geometry2.C});
	::ftwl::drawLine		(bmpTarget, {0xC0, 0xC0, 0xC0, 0x00}, ::ftwl::SLine2D<int32_t>{geometry2.C, geometry2.A});

	::drawBuffer(::GetDC(applicationInstance.MainWindowHandle), applicationInstance.BitmapOffsceen.Width, applicationInstance.BitmapOffsceen.Height, &applicationInstance.BitmapOffsceen.Colors[0][0]);
}

int																								rtMain										(::SRuntimeValues& runtimeValues)												{
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
	::SApplication																						* applicationInstance						= new ::SApplication(runtimeValues);		// Create a new instance of our application.
	if(0 == applicationInstance)
		return -1;	// return error because we couldn't allocate the main instance of our application.

	::setup(*applicationInstance);	// Call setup()
	
	while(true) {	// Execute code between braces while the condition inside () evaluates to true.
		::update	(*applicationInstance);		// Update frame.
		::draw		(*applicationInstance);		// Render frame.
		::ftwl::asciiDisplayPresent(applicationInstance->ASCIIRenderTarget);
		if(::GetAsyncKeyState(VK_ESCAPE))		// Check for escape key pressed.
			break;								// Exit while() loop.
	}

	::cleanup(*applicationInstance);
	delete(applicationInstance);	// Destroy the applcation instance and release its memory.
	return 0;
}

int																								main										()																				{
	::SRuntimeValues																					runtimeValues								= {};
	return ::ftwl::failed(::rtMain(runtimeValues)) ? EXIT_FAILURE : EXIT_SUCCESS;	// just redirect to our generic main() function.		
}

int	WINAPI																						WinMain										
	(	_In_		HINSTANCE		hInstance
	,	_In_opt_	HINSTANCE		hPrevInstance
	,	_In_		LPSTR			lpCmdLine
	,	_In_		INT				nShowCmd
	)
{
	::SRuntimeValues																					runtimeValues								= 
		{	hInstance
		,	hPrevInstance
		,	lpCmdLine
		,	nShowCmd
		};
	return ::ftwl::failed(::rtMain(runtimeValues)) ? EXIT_FAILURE : EXIT_SUCCESS;	// just redirect to our generic main() function.
}
