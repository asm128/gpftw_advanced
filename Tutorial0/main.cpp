// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
//
#include "gpftw_ascii_screen.h"
#include "gpftw_ascii_color.h"

#include <stdio.h>      // for printf()
#include <windows.h>    // for interacting with Windows

static constexpr	const int			SCREEN_WIDTH						= 32	;
static constexpr	const int			SCREEN_HEIGHT						= 16	;

// Define some functions to use from main(). These functions will contain our game code.
struct SApplication {
	::gpftw::SASCIIScreen					ASCIIScreen							= {};
	int										FrameCounter						= 0;	// Declare and initialize a variable of (int)eger type for keeping track of the number of frame since execution began.
};

void									cleanup								(::SApplication& applicationInstance)				{ ::gpftw::destroyConsole(applicationInstance.ASCIIScreen);									}	// 
void									setup								(::SApplication& applicationInstance)				{ ::gpftw::createConsole(applicationInstance.ASCIIScreen, ::SCREEN_WIDTH, ::SCREEN_HEIGHT);	}	// initialize console
void									update								(::SApplication& applicationInstance)				{ ++applicationInstance.FrameCounter;														}	// increase our frame counter by 1

// This function now will draw some coloured symbols in each cell of the ASCII screen.
void									draw								(::SApplication& applicationInstance)				{ 
	::gpftw::SASCIIScreen						&asciiTarget						= applicationInstance.ASCIIScreen;
	uint32_t									color0								= (0xFF & applicationInstance.FrameCounter * 1) | ((0xFF & applicationInstance.FrameCounter * 5) << 8) | ((0xFF & applicationInstance.FrameCounter * 2) << 16);
	uint32_t									color1								= (0xFF & applicationInstance.FrameCounter * 2) | ((0xFF & applicationInstance.FrameCounter * 3) << 8) | ((0xFF & applicationInstance.FrameCounter * 1) << 16);
	uint32_t									color2								= (0xFF & applicationInstance.FrameCounter * 3) | ((0xFF & applicationInstance.FrameCounter * 2) << 8) | ((0xFF & applicationInstance.FrameCounter * 5) << 16);
	uint32_t									color3								= (0xFF & applicationInstance.FrameCounter * 5) | ((0xFF & applicationInstance.FrameCounter * 1) << 8) | ((0xFF & applicationInstance.FrameCounter * 3) << 16);
	asciiTarget.Palette[COLOR_DARKGREY	]	= color0;
	asciiTarget.Palette[COLOR_LIGHTGREY	]	= color1;
	asciiTarget.Palette[COLOR_GREEN		]	= color2;
	asciiTarget.Palette[COLOR_BLUE		]	= color3;

	// 2d loop
	for(uint32_t y = 0; y < asciiTarget.Height	; ++y)
	for(uint32_t x = 0; x < asciiTarget.Width	; ++x) {
		uint32_t										linearIndex							= y * asciiTarget.Width	+ x;	// calculate the linear index of the screen array from the current x and y of the screen
		
		// Set the target cell with a given character and color.
		asciiTarget.Characters	[linearIndex]		= (((applicationInstance.FrameCounter % 2) + y + x) % 2) ? '!' : '?';
		asciiTarget.Colors		[linearIndex]		= (((applicationInstance.FrameCounter % 2) + y + x) % 2) ? COLOR_GREEN | (COLOR_DARKGREY << 4) : COLOR_BLUE | (COLOR_LIGHTGREY << 4) ;
	}
}

int										main								()												{
	::SApplication								* applicationInstance				= new ::SApplication();	// Create a new instance of our application.
	if( 0 == applicationInstance )
		return -1;	// return error because we couldn't allocate the main instance of our application.

	::setup(*applicationInstance); /// Call setup()
	
	while( true ) { /// Execute code between braces while the condition inside () evaluates to true.
		::update	(*applicationInstance);		// Update frame.
		::draw		(*applicationInstance);		// Render frame.

		::gpftw::presentConsole(applicationInstance->ASCIIScreen);	// Present the drawn image.

		if(::GetAsyncKeyState(VK_ESCAPE))		/// Check for escape key pressed.
			break;	/// Exit while() loop.
	}

	::cleanup(*applicationInstance);

	delete( applicationInstance );	// Destroy the applcation instance and release its memory.
	return 0; /// Exit from the function returning an (int)eger.
}

int	WINAPI					WinMain								
	(	_In_		HINSTANCE		// hInstance
	,	_In_opt_	HINSTANCE		// hPrevInstance
	,	_In_		LPSTR			// lpCmdLine
	,	_In_		INT				// nShowCmd
	)
{
	return ( 0 > main() ) ? EXIT_FAILURE : EXIT_SUCCESS;	// just redirect to our generic main() function.
}