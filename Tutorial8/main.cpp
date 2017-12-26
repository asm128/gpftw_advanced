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

struct SDisplayPlatformDetail {
						::HWND																			WindowHandle						= {};
						::WNDCLASSEX																	WindowClass							= {};
	static constexpr	const char																		WindowClassName	[256]				= "FTWL_WINDOW";
};

struct SDisplay {
						::SDisplayPlatformDetail														PlatformDetail							= {};
};

struct SApplication {
						::SDisplay																		MainWindow							= {};
						::ftwl::SBitmapOffscreen<::ftwl::SColorBGRA, ::SCREEN_WIDTH, ::SCREEN_HEIGHT>	BitmapOffsceen						= {};
						::SRuntimeValues																RuntimeValues						= {};
						::SInput																		SystemInput							= {};
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
	
static	::SApplication																				* g_ApplicationInstance						= 0;

struct SOffscreenPlatformDetail {
	uint32_t																							BitmapInfoSize								= 0;
	::BITMAPINFO																						* BitmapInfo								= 0;
	::HDC																								IntermediateDeviceContext					= 0;    // <- note, we're creating, so it needs to be destroyed
	::HBITMAP																							IntermediateBitmap							= 0;

																										~SOffscreenPlatformDetail					()																		{
		if(BitmapInfo					) free			(BitmapInfo					); 
		if(IntermediateBitmap			) DeleteObject	(IntermediateBitmap			); 
		if(IntermediateDeviceContext	) DeleteDC		(IntermediateDeviceContext	); 
	}
};
	
		::ftwl::error_t																				drawBuffer									(HDC hdc, int width, int height, ::ftwl::SColorBGRA *colorArray)				{
	const uint32_t																							bytesToCopy									= sizeof(::RGBQUAD) * width * height;
	::SOffscreenPlatformDetail																				offscreenDetail								= {};
	offscreenDetail.BitmapInfoSize																		= sizeof(::BITMAPINFO) + bytesToCopy;
	offscreenDetail.BitmapInfo																			= (::BITMAPINFO*)::malloc(offscreenDetail.BitmapInfoSize);
	for(uint32_t iPixel = 0, pixelCount = width * height; iPixel < pixelCount; ++iPixel)
		((::RGBQUAD*)offscreenDetail.BitmapInfo->bmiColors)[iPixel]											= {colorArray[iPixel].b, colorArray[iPixel].g, colorArray[iPixel].r, 0xFF};

	offscreenDetail.BitmapInfo->bmiHeader.biSize														= sizeof(::BITMAPINFO);
	offscreenDetail.BitmapInfo->bmiHeader.biWidth														= width;
	offscreenDetail.BitmapInfo->bmiHeader.biHeight														= height;
	offscreenDetail.BitmapInfo->bmiHeader.biPlanes														= 1;
	offscreenDetail.BitmapInfo->bmiHeader.biBitCount													= 32;
	offscreenDetail.BitmapInfo->bmiHeader.biCompression													= BI_RGB;
	offscreenDetail.BitmapInfo->bmiHeader.biSizeImage													= width * height * sizeof(::RGBQUAD);
	offscreenDetail.BitmapInfo->bmiHeader.biXPelsPerMeter												= 0x0ec4; // Paint and PSP use these values.
	offscreenDetail.BitmapInfo->bmiHeader.biYPelsPerMeter												= 0x0ec4; // Paint and PSP use these values.
	offscreenDetail.BitmapInfo->bmiHeader.biClrUsed														= 0; 
	offscreenDetail.BitmapInfo->bmiHeader.biClrImportant												= 0;

	offscreenDetail.IntermediateDeviceContext															= ::CreateCompatibleDC(hdc);    // <- note, we're creating, so it needs to be destroyed
	char																									* ppvBits									= 0;
	offscreenDetail.IntermediateBitmap																	= ::CreateDIBSection(offscreenDetail.IntermediateDeviceContext, offscreenDetail.BitmapInfo, DIB_RGB_COLORS, (void**) &ppvBits, NULL, 0);
	if(0 == ::SetDIBits(NULL, offscreenDetail.IntermediateBitmap, 0, height, offscreenDetail.BitmapInfo->bmiColors, offscreenDetail.BitmapInfo, DIB_RGB_COLORS)) {
		OutputDebugString("Cannot copy bits into dib section.\n");
		return -1;
	}
	::HBITMAP																								hBmpOld										= (::HBITMAP)::SelectObject(offscreenDetail.IntermediateDeviceContext, offscreenDetail.IntermediateBitmap);    // <- altering state
	if(FALSE == ::BitBlt(hdc, 0, 0, width, height, offscreenDetail.IntermediateDeviceContext, 0, 0, SRCCOPY)) {
		char																									buffer[256]									= {};
		::sprintf_s(buffer, "error blitting bitmap: 0x%x.\n", ::GetLastError());
		OutputDebugString(buffer);
		::va_list																								arguments									= {};
		::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, ::GetLastError(), 0, buffer, 256, &arguments);
		OutputDebugString(buffer);
	}  // <- no significnce

	::SelectObject(hdc, hBmpOld);	// put the old bitmap back in the DC (restore state)
	return 0;
}

		void																						update										(::SApplication& applicationInstance);
		void																						draw										(::SApplication& applicationInstance);
		LRESULT WINAPI																				mainWndProc									(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)							{
	::SApplication																							& applicationInstance						= *g_ApplicationInstance;
	::RECT																									finalClientRect								= {100, 100, 100 + ::SCREEN_WIDTH, 100 + ::SCREEN_HEIGHT};
	::AdjustWindowRectEx(&finalClientRect, WS_OVERLAPPEDWINDOW, FALSE, 0);
	::PAINTSTRUCT																							ps;
	::SDisplayPlatformDetail																				& displayDetail								= applicationInstance.MainWindow.PlatformDetail;
	switch(uMsg) {
	default: break;		
	case WM_CLOSE			: DestroyWindow(hWnd); return 0;
	case WM_GETMINMAXINFO	:	// Catch this message so to prevent the window from becoming too small.
		((MINMAXINFO*)lParam)->ptMinTrackSize																= {finalClientRect.right - finalClientRect.left, finalClientRect.bottom - finalClientRect.top}; 
		((MINMAXINFO*)lParam)->ptMaxTrackSize																= {finalClientRect.right - finalClientRect.left, finalClientRect.bottom - finalClientRect.top}; 
		return 0;
	case WM_SIZE			: //break;
	case WM_PAINT			: 
		::BeginPaint(displayDetail.WindowHandle, &ps);
		::draw		(applicationInstance);
		::EndPaint	(displayDetail.WindowHandle, &ps);
		return 0;
	case WM_DESTROY			: 
		::PostQuitMessage(0); 
		displayDetail.WindowHandle																			= 0;
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

		void																						initWndClass								(::HINSTANCE hInstance, const TCHAR* className, ::WNDCLASSEX& wndClassToInit)	{
	wndClassToInit																						= {sizeof(::WNDCLASSEX),};
	wndClassToInit.lpfnWndProc																			= ::mainWndProc;
	wndClassToInit.hInstance																			= hInstance;
	wndClassToInit.hCursor																				= LoadCursor(NULL, IDC_ARROW);
	wndClassToInit.hbrBackground																		= (::HBRUSH)(COLOR_3DFACE + 1);
	wndClassToInit.lpszClassName																		= className;
}

		void																						updateMainWindow							(::SApplication& applicationInstance)											{ 
	::MSG																									msg											= {};
	while(::PeekMessage(&msg, applicationInstance.MainWindow.PlatformDetail.WindowHandle, 0, 0, PM_REMOVE)) {
		::TranslateMessage(&msg);
		::DispatchMessage	(&msg);
		if(msg.message == WM_QUIT) {
			applicationInstance.MainWindow.PlatformDetail.WindowHandle											= 0;
			break;
		}
		break;
	}
}

// --- Cleanup application resources.
		void																						cleanup										(::SApplication& applicationInstance)											{ 
	::SDisplayPlatformDetail																				& displayDetail								= applicationInstance.MainWindow.PlatformDetail;
	if(displayDetail.WindowHandle) {
		::DestroyWindow(displayDetail.WindowHandle);
		::updateMainWindow(applicationInstance);
	}

	::UnregisterClass(displayDetail.WindowClassName, displayDetail.WindowClass.hInstance);
	::ftwl::asciiDisplayDestroy	();
	::ftwl::asciiTargetDestroy	(applicationInstance.ASCIIRenderTarget);
	g_ApplicationInstance																				= 0;
}

// --- Initialize console.
		void																						setup										(::SApplication& applicationInstance)											{ 
	g_ApplicationInstance																				= &applicationInstance;
	::ftwl::asciiTargetCreate	(applicationInstance.ASCIIRenderTarget, ::SCREEN_WIDTH, ::SCREEN_HEIGHT);
	::ftwl::asciiDisplayCreate	(applicationInstance.ASCIIRenderTarget.Width(), applicationInstance.ASCIIRenderTarget.Height());

	::SDisplayPlatformDetail																				& displayDetail								= applicationInstance.MainWindow.PlatformDetail;
	::initWndClass(applicationInstance.RuntimeValues.hInstance, displayDetail.WindowClassName, displayDetail.WindowClass);
	::RegisterClassEx(&displayDetail.WindowClass);

	::RECT																									finalClientRect								= {100, 100, 100 + ::SCREEN_WIDTH, 100 + ::SCREEN_HEIGHT};
	::AdjustWindowRectEx(&finalClientRect, WS_OVERLAPPEDWINDOW, FALSE, 0);
	applicationInstance.MainWindow.PlatformDetail.WindowHandle											= CreateWindowEx(0, displayDetail.WindowClassName, TEXT("Window FTW"), WS_OVERLAPPEDWINDOW
		, finalClientRect.left
		, finalClientRect.top
		, finalClientRect.right		- finalClientRect.left
		, finalClientRect.bottom	- finalClientRect.top
		, 0, 0, displayDetail.WindowClass.hInstance, 0
		);
	::ShowWindow	(displayDetail.WindowHandle, SW_SHOW);
	::UpdateWindow	(displayDetail.WindowHandle);
}

		void																						update										(::SApplication& applicationInstance)											{ 
	::ftwl::asciiDisplayPresent	(applicationInstance.ASCIIRenderTarget);
	::ftwl::asciiTargetClear	(applicationInstance.ASCIIRenderTarget);
	::HWND																									windowHandle								= applicationInstance.MainWindow.PlatformDetail.WindowHandle;
	if(windowHandle) {
		::HDC																								dc											= ::GetDC(windowHandle);
		::drawBuffer(dc, applicationInstance.BitmapOffsceen.Width, applicationInstance.BitmapOffsceen.Height, &applicationInstance.BitmapOffsceen.Colors[0][0]);
		::ReleaseDC(windowHandle, dc);
	}
	applicationInstance.Timer.Frame();
	applicationInstance.FrameInfo.Frame(applicationInstance.Timer.LastTimeMicroseconds);																							
	::updateMainWindow(applicationInstance);
}

		void																						draw										(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::ftwl::SASCIITarget																					& asciiTarget								= applicationInstance.ASCIIRenderTarget;
	uint32_t																								color0										= (0xFF & applicationInstance.FrameInfo.FrameNumber * 1) | ((0xFF & applicationInstance.FrameInfo.FrameNumber * 2) << 8) | ((0xFF & applicationInstance.FrameInfo.FrameNumber * 5) << 16);
	uint32_t																								color1										= (0xFF & applicationInstance.FrameInfo.FrameNumber * 2) | ((0xFF & applicationInstance.FrameInfo.FrameNumber * 1) << 8) | ((0xFF & applicationInstance.FrameInfo.FrameNumber * 3) << 16);
	uint32_t																								color2										= (0xFF & applicationInstance.FrameInfo.FrameNumber * 3) | ((0xFF & applicationInstance.FrameInfo.FrameNumber * 5) << 8) | ((0xFF & applicationInstance.FrameInfo.FrameNumber * 2) << 16);
	uint32_t																								color3										= (0xFF & applicationInstance.FrameInfo.FrameNumber * 5) | ((0xFF & applicationInstance.FrameInfo.FrameNumber * 3) << 8) | ((0xFF & applicationInstance.FrameInfo.FrameNumber * 1) << 16);
	applicationInstance.Palette[0]																		= color0;
	applicationInstance.Palette[1]																		= color1;
	applicationInstance.Palette[2]																		= color2;
	applicationInstance.Palette[3]																		= color3;
	::ftwl::asciiDisplayPaletteSet({&applicationInstance.Palette[0], 16});

	::ftwl::SCoord2		<int32_t>																			screenCenter								= {::SCREEN_WIDTH / 2, ::SCREEN_HEIGHT / 2};
	::ftwl::SRectangle2D<int32_t>																			geometry0									= {{2, 2}, {(int32_t)((applicationInstance.FrameInfo.FrameNumber / 2) % (::SCREEN_WIDTH - 2)), 5}};
	::ftwl::SCircle2D	<int32_t>																			geometry1									= {5.0 + (applicationInstance.FrameInfo.FrameNumber / 5) % 5, screenCenter};	
	::ftwl::STriangle2D	<int32_t>																			geometry2									= {{0, 0}, {15, 15}, {-5, 10}};	
	geometry2.A																							+= screenCenter + ::ftwl::SCoord2<int32_t>{(int32_t)geometry1.Radius, (int32_t)geometry1.Radius};
	geometry2.B																							+= screenCenter + ::ftwl::SCoord2<int32_t>{(int32_t)geometry1.Radius, (int32_t)geometry1.Radius};
	geometry2.C																							+= screenCenter + ::ftwl::SCoord2<int32_t>{(int32_t)geometry1.Radius, (int32_t)geometry1.Radius};
	::ftwl::drawRectangle	(asciiTarget, {'!', ::ftwl::ASCII_COLOR_CYAN		}, geometry0);
	::ftwl::drawRectangle	(asciiTarget, {'!', ::ftwl::ASCII_COLOR_BLUE		}, {geometry0.Offset + ::ftwl::SCoord2<int32_t>{1, 1}, geometry0.Size - ::ftwl::SCoord2<int32_t>{2, 2}});
	::ftwl::drawCircle		(asciiTarget, {'?', ::ftwl::ASCII_COLOR_GREEN		}, geometry1);
	::ftwl::drawCircle		(asciiTarget, {'?', ::ftwl::ASCII_COLOR_RED			}, {geometry1.Radius - 1, geometry1.Center});
	::ftwl::drawTriangle	(asciiTarget, {'o', ::ftwl::ASCII_COLOR_YELLOW		}, geometry2);
	::ftwl::drawLine		(asciiTarget, {'o', ::ftwl::ASCII_COLOR_MAGENTA		}, ::ftwl::SLine2D<int32_t>{geometry2.A, geometry2.B});
	::ftwl::drawLine		(asciiTarget, {'o', ::ftwl::ASCII_COLOR_WHITE		}, ::ftwl::SLine2D<int32_t>{geometry2.B, geometry2.C});
	::ftwl::drawLine		(asciiTarget, {'o', ::ftwl::ASCII_COLOR_LIGHTGREY	}, ::ftwl::SLine2D<int32_t>{geometry2.C, geometry2.A});

	::HWND																									windowHandle								= applicationInstance.MainWindow.PlatformDetail.WindowHandle;
	if(windowHandle) {
		::ftwl::SBitmapTargetBGRA																				bmpTarget									= {{&applicationInstance.BitmapOffsceen.Colors[0][0], applicationInstance.BitmapOffsceen.Width, applicationInstance.BitmapOffsceen.Height},};
		::memset(&bmpTarget.Colors[0][0], 0, sizeof(::ftwl::SColorBGRA) * bmpTarget.Colors.size());
		::ftwl::drawRectangle	(bmpTarget, ::ftwl::SColorRGBA(applicationInstance.Palette[::ftwl::ASCII_COLOR_BLUE			]), geometry0);
		::ftwl::drawRectangle	(bmpTarget, ::ftwl::SColorRGBA(applicationInstance.Palette[::ftwl::ASCII_COLOR_BLUE			]), {geometry0.Offset + ::ftwl::SCoord2<int32_t>{1, 1}, geometry0.Size - ::ftwl::SCoord2<int32_t>{2, 2}});
		::ftwl::drawCircle		(bmpTarget, ::ftwl::SColorRGBA(applicationInstance.Palette[::ftwl::ASCII_COLOR_GREEN		]), geometry1);
		::ftwl::drawCircle		(bmpTarget, ::ftwl::SColorRGBA(applicationInstance.Palette[::ftwl::ASCII_COLOR_RED			]), {geometry1.Radius - 1, geometry1.Center});
		::ftwl::drawTriangle	(bmpTarget, ::ftwl::SColorRGBA(applicationInstance.Palette[::ftwl::ASCII_COLOR_YELLOW		]), geometry2);
		::ftwl::drawLine		(bmpTarget, ::ftwl::SColorRGBA(applicationInstance.Palette[::ftwl::ASCII_COLOR_MAGENTA		]), ::ftwl::SLine2D<int32_t>{geometry2.A, geometry2.B});
		::ftwl::drawLine		(bmpTarget, ::ftwl::SColorRGBA(applicationInstance.Palette[::ftwl::ASCII_COLOR_WHITE		]), ::ftwl::SLine2D<int32_t>{geometry2.B, geometry2.C});
		::ftwl::drawLine		(bmpTarget, ::ftwl::SColorRGBA(applicationInstance.Palette[::ftwl::ASCII_COLOR_LIGHTGREY	]), ::ftwl::SLine2D<int32_t>{geometry2.C, geometry2.A});
	}
}

		int																						rtMain										(::SRuntimeValues& runtimeValues)												{
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
	::SApplication																						* applicationInstance						= new ::SApplication(runtimeValues);		// Create a new instance of our application.
	if(0 == applicationInstance)
		return -1;	// return error because we couldn't allocate the main instance of our application.

	::setup(*applicationInstance);	// Call setup()
	
	while(true) {	// Execute code between braces while the condition inside () evaluates to true.
		::update	(*applicationInstance);		// Update frame.
		::draw		(*applicationInstance);		// Render frame.
		if(::GetAsyncKeyState(VK_ESCAPE))		// Check for escape key pressed.
			break;								// Exit while() loop.
	}

	::cleanup(*applicationInstance);
	delete(applicationInstance);	// Destroy the applcation instance and release its memory.
	return 0;
}

		int																						main										()																				{
	::SRuntimeValues																					runtimeValues								= {GetModuleHandle(NULL), 0, 0, SW_SHOW};
	return ::ftwl::failed(::rtMain(runtimeValues)) ? EXIT_FAILURE : EXIT_SUCCESS;	// just redirect to our generic main() function.		
}

		int	WINAPI																				WinMain										
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
