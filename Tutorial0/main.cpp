// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
//
#include "ftw_ascii_display.h"
#include "ftw_ascii_color.h"

#include <windows.h>    // for interacting with Windows

static constexpr	const int							SCREEN_WIDTH						= 48	;
static constexpr	const int							SCREEN_HEIGHT						= 32	;

struct SApplication {
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
	int														FrameCounter						= 0;	// Declare and initialize a variable of (int)eger type for keeping track of the number of frame since execution began.
};

void													cleanup								(::SApplication& applicationInstance)				{ 
	::ftwl::asciiDisplayDestroy();
	::ftwl::asciiTargetDestroy(applicationInstance.ASCIIRenderTarget);
}	// Cleanup application resources.

void													setup								(::SApplication& applicationInstance)				{ 
	// --- Initialize console.
	::ftwl::asciiTargetCreate(applicationInstance.ASCIIRenderTarget, ::SCREEN_WIDTH, ::SCREEN_HEIGHT);
	::ftwl::asciiDisplayCreate(applicationInstance.ASCIIRenderTarget.Width(), applicationInstance.ASCIIRenderTarget.Height());
}

void													update								(::SApplication& applicationInstance)				{ ++applicationInstance.FrameCounter;														}	// Increase our frame counter by 1.
void													draw								(::SApplication& applicationInstance)				{	
	// This function now will draw some coloured symbols in each cell of the ASCII screen.
	::ftwl::SASCIITarget										&asciiTarget						= applicationInstance.ASCIIRenderTarget;
	uint32_t													color0								= (0xFF & applicationInstance.FrameCounter * 1) | ((0xFF & applicationInstance.FrameCounter * 2) << 8) | ((0xFF & applicationInstance.FrameCounter * 5) << 16);
	uint32_t													color1								= (0xFF & applicationInstance.FrameCounter * 2) | ((0xFF & applicationInstance.FrameCounter * 1) << 8) | ((0xFF & applicationInstance.FrameCounter * 3) << 16);
	uint32_t													color2								= (0xFF & applicationInstance.FrameCounter * 3) | ((0xFF & applicationInstance.FrameCounter * 5) << 8) | ((0xFF & applicationInstance.FrameCounter * 2) << 16);
	uint32_t													color3								= (0xFF & applicationInstance.FrameCounter * 5) | ((0xFF & applicationInstance.FrameCounter * 3) << 8) | ((0xFF & applicationInstance.FrameCounter * 1) << 16);
	applicationInstance.Palette[0]							= color0;
	applicationInstance.Palette[1]							= color1;
	applicationInstance.Palette[2]							= color2;
	applicationInstance.Palette[3]							= color3;
	::ftwl::asciiDisplayPaletteSet({&applicationInstance.Palette[0], 16});
	// 2d loop
	for(uint32_t y = 0; y < asciiTarget.Height	(); ++y)
	for(uint32_t x = 0; x < asciiTarget.Width	(); ++x) {
		// Set the target cell with a given character and color.
		asciiTarget.Characters	[y][x]							= (((applicationInstance.FrameCounter % 2) + y + x) % 2) ? '!' : '?';
		asciiTarget.Colors		[y][x]							= (((applicationInstance.FrameCounter % 2) + y + x) % 2) ? 0 | (2 << 4) : 1 | (3 << 4) ;
	}
}

int														main								()													{
	::SApplication												* applicationInstance				= new ::SApplication();	// Create a new instance of our application.
	if( 0 == applicationInstance )
		return -1;	// return error because we couldn't allocate the main instance of our application.

	::setup(*applicationInstance); /// Call setup()
	
	while( true ) { /// Execute code between braces while the condition inside () evaluates to true.
		::update	(*applicationInstance);		// Update frame.
		::draw		(*applicationInstance);		// Render frame.
		::ftwl::asciiDisplayPresent(applicationInstance->ASCIIRenderTarget);

		if(::GetAsyncKeyState(VK_ESCAPE))		/// Check for escape key pressed.
			break;	/// Exit while() loop.
	}

	::cleanup(*applicationInstance);

	delete( applicationInstance );	// Destroy the applcation instance and release its memory.
	return 0; /// Exit from the function returning an (int)eger.
}

int	WINAPI												WinMain								
	(	_In_		HINSTANCE		// hInstance
	,	_In_opt_	HINSTANCE		// hPrevInstance
	,	_In_		LPSTR			// lpCmdLine
	,	_In_		INT				// nShowCmd
	)
{
	return ::ftwl::failed( 0 > main() ) ? EXIT_FAILURE : EXIT_SUCCESS;	// just redirect to our generic main() function.
}
