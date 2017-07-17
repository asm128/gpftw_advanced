// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "application.h"	// for
#include "ftw_ascii_color.h"

#include <stdio.h>		// for printf()
#include <windows.h>	// for interacting with Windows

static constexpr const uint32_t												SCREEN_WIDTH													= 128;
static constexpr const uint32_t												SCREEN_HEIGHT													= 64;

// Cleanup application resources.
::ftwlib::error_t															ftwapp::cleanup													(::ftwapp::SApplication& applicationInstance)			{ 
	::ftwlib::destroyConsole(applicationInstance.ScreenASCII);								
	return 0;
}

// Use this function to setup our game data
::ftwlib::error_t															ftwapp::setup													(::ftwapp::SApplication& applicationInstance)			{ // Accepts an address pointing to an SGame instance
	::ftwlib::createConsole(applicationInstance.ScreenASCII, ::SCREEN_WIDTH, ::SCREEN_HEIGHT);
	applicationInstance.Game.CombatAreaSizeEffective							= {::SCREEN_WIDTH, ::SCREEN_HEIGHT * 2};
	applicationInstance.Game.CombatAreaSizeVisible								= {::SCREEN_WIDTH - 5, ::SCREEN_HEIGHT * 2 - 5};
	applicationInstance.ScreenASCII.Palette[::ftwlib::ASCII_COLOR_DARKBLUE]		= 0x100000;

	::game::setup(applicationInstance.Game);
	srand(0);
	return 0;
}

// Use this function to update our game data
::ftwlib::error_t															ftwapp::update													(::ftwapp::SApplication& applicationInstance)			{ // Accepts an address of an SGame instance
	::ftwlib::presentConsole(applicationInstance.ScreenASCII);
	::ftwlib::STimer																& timerInstance													= applicationInstance.Timer;
	::game::update(applicationInstance.Game, timerInstance.LastTimeMicroseconds);
	timerInstance.Frame();
	return 0;
}

::ftwlib::error_t															ftwapp::render													(::ftwapp::SApplication& applicationInstance)			{
	//::ftwlib::clearConsole(applicationInstance.ScreenASCII);
	::ftwlib::SScreenASCII															& screenAscii													= applicationInstance.ScreenASCII;
	::memset(screenAscii.Characters	.begin(), 0, screenAscii.Characters	.size());
	//::memset(screenAscii.Colors		.begin(), 0, screenAscii.Colors		.size() * sizeof(uint16_t));
	for(uint32_t i=0; i<screenAscii.Colors.size(); ++i)
		screenAscii.Colors[i]														= ::ftwlib::ASCII_COLOR_WHITE;
	::game::render(applicationInstance.Game, screenAscii);
	return 0;
}

int																			main															()														{
	::ftwapp::SApplication															* applicationInstance											= new ::ftwapp::SApplication();	// Create a new instance of our application.
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

int	WINAPI																	WinMain								
	(	_In_		::HINSTANCE		// hInstance
	,	_In_opt_	::HINSTANCE		// hPrevInstance
	,	_In_		::LPSTR			// lpCmdLine
	,	_In_		::INT			// nShowCmd
	)
{
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_DELAY_FREE_MEM_DF);
	return ::ftwlib::failed( 0 > main() ) ? EXIT_FAILURE : EXIT_SUCCESS;	// just redirect to our generic main() function.
}
