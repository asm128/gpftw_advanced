#include "game.h"

#include "ftw_particle.h"		// for ::game::SGame
#include "ftw_ascii_screen.h"	// for ::ftwl::SScreenASCII
#include "ftw_timer.h"			// for ::ftwl::STimer

#ifndef APPLICATION_H
#define APPLICATION_H

namespace ftwapp {

	struct SApplication {
		::game::SGame															Game								= {};
		::ftwl::STimer														Timer								= {};
		::ftwl::SScreenASCII													ScreenASCII							= {};
	};

	::ftwl::error_t														setup								(::ftwapp::SApplication& applicationInstance);
	::ftwl::error_t														update								(::ftwapp::SApplication& applicationInstance);
	::ftwl::error_t														render								(::ftwapp::SApplication& applicationInstance);
	::ftwl::error_t														cleanup								(::ftwapp::SApplication& applicationInstance);
}


#endif // APPLICATION_H