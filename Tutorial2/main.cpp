// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
//
#include "application.h"	// for

#include "ftw_ascii_display.h"

#include <stdio.h>		// for printf()
#include <windows.h>	// for interacting with Windows

static constexpr const uint32_t											SCREEN_WIDTH													= game::MAP_WIDTH + 16;
static constexpr const uint32_t											SCREEN_HEIGHT													= game::MAP_DEPTH + 8;

// Cleanup application resources.
::ftwl::error_t															ftwapp::cleanup													(::ftwapp::SApplication& applicationInstance)			{ 
	::game::cleanup(applicationInstance.Game);
	::ftwl::asciiDisplayDestroy	();								
	::ftwl::asciiTargetDestroy	(applicationInstance.ASCIIRenderTarget);								
	return 0;
}

// Use this function to setup our game data
::ftwl::error_t															ftwapp::setup													(::ftwapp::SApplication& applicationInstance)			{ // Accepts an address pointing to an SGame instance
	::ftwl::asciiTargetCreate(applicationInstance.ASCIIRenderTarget, ::SCREEN_WIDTH, ::SCREEN_HEIGHT);
	::ftwl::asciiDisplayCreate(applicationInstance.ASCIIRenderTarget.Width(), applicationInstance.ASCIIRenderTarget.Height());
	::srand(0);
	// call setup game functions
	::game::setup(applicationInstance.Game);
	return 0;
}

// Use this function to update our game data
::ftwl::error_t															ftwapp::update													(::ftwapp::SApplication& applicationInstance)			{ // Accepts an address of an SGame instance
	::ftwl::asciiDisplayPresent(applicationInstance.ASCIIRenderTarget);

	::game::SGame																	& gameInstance													= applicationInstance.Game;																	
	::ftwl::STimer																& timerInstance													= applicationInstance.Timer;																	
	::game::update(gameInstance, timerInstance.LastTimeMicroseconds);
	timerInstance.Frame();
	return 0;
}

::ftwl::error_t															ftwapp::render													(::ftwapp::SApplication& applicationInstance)			{
	::game::draw(applicationInstance.Game, applicationInstance.ASCIIRenderTarget.Width(), applicationInstance.ASCIIRenderTarget.Characters.begin(), applicationInstance.ASCIIRenderTarget.Colors.begin());
	return 0;
}

int																		main															()														{
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

int	WINAPI																WinMain	
	(	_In_		::HINSTANCE		// hInstance
	,	_In_opt_	::HINSTANCE		// hPrevInstance
	,	_In_		::LPSTR			// lpCmdLine
	,	_In_		::INT				// nShowCmd
	)
{
	::_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
	return ::ftwl::failed( 0 > ::main() ) ? EXIT_FAILURE : EXIT_SUCCESS;	// just redirect to our generic main() function.
}
