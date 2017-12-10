// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
//
#include "ftw_ascii_display.h"
#include "ftw_frameinfo.h"
#include "ftw_timer.h"

#include <windows.h>    // for interacting with Windows

static constexpr	const int							SCREEN_WIDTH						= 132	;
static constexpr	const int							SCREEN_HEIGHT						= 50	;

struct SInput {
	uint8_t													KeyboardPrevious	[256]		= {};
	uint8_t													KeyboardCurrent		[256]		= {};
};

struct SApplication {
	::SInput												SystemInput							= {};
	::ftwl::STimer											Timer								= {};
	::ftwl::SFrameInfo										FrameInfo							= {};
	::ftwl::SASCIITarget									ASCIIRenderTarget					= {};
	::ftwl::SPalette										Palette								= 
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
};

// --- Cleanup application resources.
void													cleanup								(::SApplication& applicationInstance)											{ 
	::ftwl::asciiDisplayDestroy();
	::ftwl::asciiTargetDestroy(applicationInstance.ASCIIRenderTarget);
}

// --- Initialize console.
void													setup								(::SApplication& applicationInstance)				{ 
	::ftwl::asciiTargetCreate(applicationInstance.ASCIIRenderTarget, ::SCREEN_WIDTH, ::SCREEN_HEIGHT);
	::ftwl::asciiDisplayCreate(applicationInstance.ASCIIRenderTarget.Width(), applicationInstance.ASCIIRenderTarget.Height());
}

void													update								(::SApplication& applicationInstance)				{ 
	::ftwl::asciiTargetClear(applicationInstance.ASCIIRenderTarget);
	applicationInstance.Timer.Frame();
	applicationInstance.FrameInfo.Frame(applicationInstance.Timer.LastTimeMicroseconds);														

}	

void													draw								(::SApplication& applicationInstance)				{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::ftwl::SASCIITarget										& asciiTarget						= applicationInstance.ASCIIRenderTarget;
	uint32_t													color0								= (0xFF & applicationInstance.FrameInfo.FrameNumber * 1) | ((0xFF & applicationInstance.FrameInfo.FrameNumber * 2) << 8) | ((0xFF & applicationInstance.FrameInfo.FrameNumber * 5) << 16);
	uint32_t													color1								= (0xFF & applicationInstance.FrameInfo.FrameNumber * 2) | ((0xFF & applicationInstance.FrameInfo.FrameNumber * 1) << 8) | ((0xFF & applicationInstance.FrameInfo.FrameNumber * 3) << 16);
	uint32_t													color2								= (0xFF & applicationInstance.FrameInfo.FrameNumber * 3) | ((0xFF & applicationInstance.FrameInfo.FrameNumber * 5) << 8) | ((0xFF & applicationInstance.FrameInfo.FrameNumber * 2) << 16);
	uint32_t													color3								= (0xFF & applicationInstance.FrameInfo.FrameNumber * 5) | ((0xFF & applicationInstance.FrameInfo.FrameNumber * 3) << 8) | ((0xFF & applicationInstance.FrameInfo.FrameNumber * 1) << 16);
	applicationInstance.Palette[0]							= color0;
	applicationInstance.Palette[1]							= color1;
	applicationInstance.Palette[2]							= color2;
	applicationInstance.Palette[3]							= color3;
	::ftwl::asciiDisplayPaletteSet({&applicationInstance.Palette[0], 16});
	::ftwl::SRectangle2D<int32_t>								geometry0							= {{2, 2}, {(int32_t)((applicationInstance.FrameInfo.FrameNumber / 2) % (SCREEN_WIDTH - 2)), 5}};
	::ftwl::SCircle2D	<int32_t>								geometry1							= {5.0 + (applicationInstance.FrameInfo.FrameNumber / 5) % 5, {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}};	
	::ftwl::STriangle2D	<int32_t>								geometry2							= {{10, 10}, {15, 15}, {5, 12}};	

	::ftwl::drawRectangle	(asciiTarget, {'!', ::ftwl::ASCII_COLOR_CYAN	}, geometry0);
	::ftwl::drawRectangle	(asciiTarget, {'!', ::ftwl::ASCII_COLOR_BLUE	}, {geometry0.Offset + ::ftwl::SCoord2<int32_t>{1, 1}, geometry0.Size - ::ftwl::SCoord2<int32_t>{2, 2}});
	::ftwl::drawCircle		(asciiTarget, {'?', ::ftwl::ASCII_COLOR_GREEN	}, geometry1);
	::ftwl::drawCircle		(asciiTarget, {'?', ::ftwl::ASCII_COLOR_RED		}, {geometry1.Radius - 1, geometry1.Center});
	::ftwl::drawTriangle	(asciiTarget, {'o', ::ftwl::ASCII_COLOR_YELLOW	}, geometry2);
}
 	
int														main								()													{
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
	::SApplication												* applicationInstance				= new ::SApplication();		// Create a new instance of our application.
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
	return 0;						// Exit from the function returning an (int)eger.
}

int	WINAPI												WinMain								
	(	_In_		HINSTANCE		// hInstance
	,	_In_opt_	HINSTANCE		// hPrevInstance
	,	_In_		LPSTR			// lpCmdLine
	,	_In_		INT				// nShowCmd
	)
{
	return ::ftwl::failed(main()) ? EXIT_FAILURE : EXIT_SUCCESS;	// just redirect to our generic main() function.
}
