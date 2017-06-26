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
	::ftwlib::destroyConsole	(applicationInstance.ScreenASCII);								
	return 0;
}

// Use this function to setup our game data
::ftwlib::error_t															ftwapp::setup													(::ftwapp::SApplication& applicationInstance)			{ // Accepts an address pointing to an SGame instance
	static uint16_t																	actionKeys[32]													= {};
	actionKeys[::game::ACTION_INPUT_SALUTE			]							= VK_SPACE;
	actionKeys[::game::ACTION_INPUT_WALK_FRONT		]							= 'W';
	actionKeys[::game::ACTION_INPUT_WALK_BACK		]							= 'S';
	actionKeys[::game::ACTION_INPUT_STRAFE_RIGHT	]							= 'D';
	actionKeys[::game::ACTION_INPUT_STRAFE_LEFT		]							= 'A';
	actionKeys[::game::ACTION_INPUT_TURN_RIGHT		]							= VK_RIGHT;
	actionKeys[::game::ACTION_INPUT_TURN_LEFT		]							= VK_LEFT;
	actionKeys[::game::ACTION_INPUT_CLIMB_UP		]							= VK_UP;
	actionKeys[::game::ACTION_INPUT_CLIMB_DOWN		]							= VK_DOWN;
	actionKeys[::game::ACTION_INPUT_FLY_UP			]							= VK_UP;
	actionKeys[::game::ACTION_INPUT_FLY_DOWN		]							= VK_DOWN;
	actionKeys[::game::ACTION_INPUT_SPRINT			]							= VK_SHIFT;
	actionKeys[::game::ACTION_INPUT_JUMP			]							= VK_SPACE;
	actionKeys[::game::ACTION_INPUT_CROUCH			]							= 'C';
	actionKeys[::game::ACTION_INPUT_SNEAK			]							= VK_CONTROL;
	actionKeys[::game::ACTION_INPUT_SHOOT			]							= VK_RETURN;
	actionKeys[::game::ACTION_INPUT_MELEE			]							= 'V';
	actionKeys[::game::ACTION_INPUT_THROW			]							= 'T';
	actionKeys[::game::ACTION_INPUT_RELOAD			]							= 'R';
	actionKeys[::game::ACTION_INPUT_USEITEM			]							= 'F';
	actionKeys[::game::ACTION_INPUT_USEPROP			]							= 'E';
	actionKeys[::game::ACTION_INPUT_WEAPON_NEXT		]							= 'Q';
	actionKeys[::game::ACTION_INPUT_WEAPON_PREV		]							= ' ';
	actionKeys[::game::ACTION_INPUT_ITEM_NEXT		]							= 'Z';
	actionKeys[::game::ACTION_INPUT_ITEM_PREV		]							= ' ';
	applicationInstance.ActionInputPlayer										= {actionKeys, ::game::ACTION_INPUT_COUNT};

	::ftwlib::createConsole(applicationInstance.ScreenASCII, ::SCREEN_WIDTH, ::SCREEN_HEIGHT);
	::srand(0);
	::game::setup(applicationInstance.Game);	// call setup game functions
	return 0;
}

// Use this function to update our game data
::ftwlib::error_t															ftwapp::update													(::ftwapp::SApplication& applicationInstance)			{ // Accepts an address of an SGame instance
	::ftwlib::presentConsole														(applicationInstance.ScreenASCII);

	::game::SGame																	& gameInstance													= applicationInstance.Game;																	
	::ftwlib::STimer																& timerInstance													= applicationInstance.Timer;
	::game::update(gameInstance, timerInstance.LastTimeMicroseconds);
	timerInstance.Frame();
	return 0;
}

::ftwlib::error_t															ftwapp::render													(::ftwapp::SApplication& applicationInstance)			{
	//::ftwlib::clearConsole															(applicationInstance.ScreenASCII);
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
		Sleep(100);
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