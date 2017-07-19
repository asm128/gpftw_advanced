// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
//
#include "game.h"			// for our game functions
#include "ftw_timer.h"	// for our STimer struct

#include "application.h"	// for

#include <stdio.h>		// for printf()
#include <windows.h>	// for interacting with Windows

static constexpr const uint32_t												SCREEN_WIDTH													= game::MAP_WIDTH + 64;
static constexpr const uint32_t												SCREEN_HEIGHT													= game::MAP_DEPTH + 16;

// Cleanup application resources.
::ftwlib::error_t															ftwapp::cleanup													(::ftwapp::SApplication& applicationInstance)			{ 
	::game::cleanup				(applicationInstance.Game);
	::ftwlib::consoleDestroy	(applicationInstance.ScreenASCII);								
	return 0;
}

// Use this function to setup our game data
::ftwlib::error_t															ftwapp::setup													(::ftwapp::SApplication& applicationInstance)			{ // Accepts an address pointing to an SGame instance
	::ftwlib::consoleCreate(applicationInstance.ScreenASCII, ::SCREEN_WIDTH, ::SCREEN_HEIGHT);
	::srand(0);
	::game::setup(applicationInstance.Game);	// call setup game functions
	return 0;
}

// Use this function to update our game data
::ftwlib::error_t															ftwapp::update													(::ftwapp::SApplication& applicationInstance)			{ // Accepts an address of an SGame instance
	::ftwlib::consolePresent(applicationInstance.ScreenASCII);

	::game::SGame																	& gameInstance													= applicationInstance.Game;																	
	::ftwlib::STimer																& timerInstance													= applicationInstance.Timer;																	
	::game::update(gameInstance, timerInstance.LastTimeMicroseconds);
	timerInstance.Frame();
	return 0;
}

::ftwlib::error_t															ftwapp::render													(::ftwapp::SApplication& applicationInstance)			{
	//::ftwlib::consoleClear(applicationInstance.ScreenASCII);
	::memset(applicationInstance.ScreenASCII.Characters	.begin(), 0, applicationInstance.ScreenASCII.Characters	.size());
	::memset(applicationInstance.ScreenASCII.Colors		.begin(), 0, applicationInstance.ScreenASCII.Colors		.size() * sizeof(uint16_t));
	::game::draw(applicationInstance.Game, applicationInstance.ScreenASCII.Width, applicationInstance.ScreenASCII.Characters.begin(), applicationInstance.ScreenASCII.Colors.begin());
	return 0;
}

int																			main															()														{
	::ftwapp::SApplication															* applicationInstance											= new ::ftwapp::SApplication();	// Create a new instance of our application.
	if( 0 == applicationInstance )
		return -1;	// return error because we couldn't allocate the main instance of our application.

	::ftwapp::setup(*applicationInstance); /// Call setup()
	
	while( true ) { /// Execute code between braces while the condition inside () evaluates to true.
		::ftwapp::update	(*applicationInstance);		// Update frame.
		::ftwapp::render	(*applicationInstance);		// Render frame.
		if(::GetAsyncKeyState(VK_ESCAPE))		/// Check for escape key pressed.
			break;	/// Exit while() loop.
	}

	::ftwapp::cleanup(*applicationInstance);

	delete( applicationInstance );	// Destroy the applcation instance and release its memory.
	return 0; /// Exit from the function returning an (int)eger.
}

int	WINAPI														WinMain								
	(	_In_		::HINSTANCE		// hInstance
	,	_In_opt_	::HINSTANCE		// hPrevInstance
	,	_In_		::LPSTR			// lpCmdLine
	,	_In_		::INT				// nShowCmd
	)
{
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
	return ::ftwlib::failed( 0 > main() ) ? EXIT_FAILURE : EXIT_SUCCESS;	// just redirect to our generic main() function.
}
