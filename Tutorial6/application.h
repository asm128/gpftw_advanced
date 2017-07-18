#include "game.h"

#include "ftw_particle.h"		// for ::game::SGame
#include "ftw_ascii_screen.h"	// for ::ftwlib::SScreenASCII
#include "ftw_timer.h"			// for ::ftwlib::STimer

#ifndef APPLICATION_H
#define APPLICATION_H

namespace ftwapp {

	struct SApplication {
		::game::SGame															Game								= {};
		::ftwlib::STimer														Timer								= {};
		::ftwlib::SScreenASCII													ScreenASCII							= {};
	};

	::ftwlib::error_t														setup								(::ftwapp::SApplication& applicationInstance);
	::ftwlib::error_t														update								(::ftwapp::SApplication& applicationInstance);
	::ftwlib::error_t														render								(::ftwapp::SApplication& applicationInstance);
	::ftwlib::error_t														cleanup								(::ftwapp::SApplication& applicationInstance);
}


#endif // APPLICATION_H