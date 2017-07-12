#include "game.h"				// for ::game::SGame

#include "ftw_ascii_screen.h"	// for ::ftwlib::SScreenASCII
#include "ftw_timer.h"			// for ::ftwlib::STimer

#ifndef APPLICATION_H
#define APPLICATION_H

namespace ftwapp {
	struct SApplication {
		::ftwlib::SScreenASCII													ScreenASCII							= {};
		int64_t																	FrameCounter						= 0;	// Declare and initialize a variable of (int)eger type for keeping track of the number of frame since execution began.

		::ftwlib::STimer														Timer								= {};
		::game::SGame															Game								= {};
	};

	::ftwlib::error_t														setup								(::ftwapp::SApplication& applicationInstance);
	::ftwlib::error_t														update								(::ftwapp::SApplication& applicationInstance);
	::ftwlib::error_t														render								(::ftwapp::SApplication& applicationInstance);
	::ftwlib::error_t														cleanup								(::ftwapp::SApplication& applicationInstance);
}


#endif // APPLICATION_H