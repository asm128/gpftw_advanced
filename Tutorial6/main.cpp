// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "application.h"	// for
#include "ftw_ascii_display.h"	// for ::ftwl::SScreenASCII

#include <stdio.h>		// for printf()
#include <windows.h>	// for interacting with Windows

static constexpr const uint32_t											SCREEN_WIDTH													= 128;
static constexpr const uint32_t											SCREEN_HEIGHT													= 36;

// Cleanup application resources.
::ftwl::error_t															ftwapp::cleanup													(::ftwapp::SApplication& applicationInstance)			{ 
	::ftwl::asciiDisplayDestroy	();								
	::ftwl::asciiTargetDestroy	(applicationInstance.ASCIIRenderTarget);								
	return 0;
}

// Use this function to setup our game data
::ftwl::error_t															ftwapp::setup													(::ftwapp::SApplication& applicationInstance)			{ // Accepts an address pointing to an SGame instance
	::ftwl::asciiTargetCreate(applicationInstance.ASCIIRenderTarget, ::SCREEN_WIDTH, ::SCREEN_HEIGHT);
	::ftwl::asciiDisplayCreate(applicationInstance.ASCIIRenderTarget.Width(), applicationInstance.ASCIIRenderTarget.Height());
	applicationInstance.Game.CombatAreaSizeEffective						= {::SCREEN_WIDTH, ::SCREEN_HEIGHT * 2};
	applicationInstance.Game.CombatAreaSizeVisible							= {::SCREEN_WIDTH - 5, ::SCREEN_HEIGHT * 2 - 5};
	applicationInstance.Palette[::ftwl::ASCII_COLOR_DARKBLUE]				= 0x100000;
	::ftwl::asciiDisplayPaletteSet({applicationInstance.Palette.data(), 16});
	::ftwl::asciiDisplayTitleSet("Spaceship Game v0.01");
	::game::setup(applicationInstance.Game);
	srand(0);
	return 0;
}

// Use this function to update our game data
::ftwl::error_t															ftwapp::update													(::ftwapp::SApplication& applicationInstance)			{ // Accepts an address of an SGame instance
	::ftwl::asciiDisplayPresent(applicationInstance.ASCIIRenderTarget);

	::ftwl::SColorRGBA															& oldColor0							= applicationInstance.Palette[::ftwl::ASCII_COLOR_DARKBLUE];
	oldColor0.r																= oldColor0.r;
	oldColor0.g																= oldColor0.g;
	oldColor0.b																= ((applicationInstance.Game.FrameInfo.FrameNumber % 32) >= 16) ? oldColor0.b + 1 : oldColor0.b - 1;
	::ftwl::asciiDisplayPaletteSet({applicationInstance.Palette.data(), 16});
	::ftwl::STimer																& timerInstance													= applicationInstance.Timer;
	::game::update(applicationInstance.Game, timerInstance.LastTimeMicroseconds);
	timerInstance.Frame();
	return 0;
}

::ftwl::error_t															ftwapp::render													(::ftwapp::SApplication& applicationInstance)			{
	::ftwl::SASCIITarget														& screenAscii													= applicationInstance.ASCIIRenderTarget;
	::memset(screenAscii.Characters	.begin(), 0, screenAscii.Characters	.size());
	::memset(screenAscii.Colors		.begin(), 0, screenAscii.Colors		.size() * sizeof(uint16_t));
	::game::render(applicationInstance.Game, screenAscii);
	return 0;
}

int																		main															()														{
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_DELAY_FREE_MEM_DF);
	::ftwapp::SApplication														* applicationInstance											= new ::ftwapp::SApplication();	// Create a new instance of our application.
	if( 0 == applicationInstance )
		return -1;	// return error because we couldn't allocate the main instance of our application.

	::ftwapp::setup		(*applicationInstance); 

	while( true ) { /// Execute code between braces while the condition inside () evaluates to true.
		::ftwapp::update	(*applicationInstance);		/// Update frame.
		::ftwapp::render	(*applicationInstance);		/// Render frame.
		if(::GetAsyncKeyState(VK_ESCAPE))		/// Check for escape key pressed.
			break;	/// Exit while() loop.
		Sleep(1);
	}

	::ftwapp::cleanup	(*applicationInstance);

	delete( applicationInstance );	// Destroy the applcation instance and release its memory.
	return 0; /// Exit from the function returning an (int)eger.
}

int	WINAPI																WinMain								
	(	_In_		::HINSTANCE		// hInstance
	,	_In_opt_	::HINSTANCE		// hPrevInstance
	,	_In_		::LPSTR			// lpCmdLine
	,	_In_		::INT			// nShowCmd
	)
{
	return ::ftwl::failed(::main()) ? EXIT_FAILURE : EXIT_SUCCESS;	// just redirect to our generic main() function.
}
