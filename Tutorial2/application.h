#include "game.h"

#include "ftw_ascii_screen.h"

#include "ftw_timer.h"

namespace ftwapp {
	struct SApplication {
		::ftwl::SScreenASCII													ScreenASCII							= {};
		int64_t																	FrameCounter						= 0;	// Declare and initialize a variable of (int)eger type for keeping track of the number of frame since execution began.

		::ftwl::STimer														Timer								= {};
		::game::SGame															Game								= {};
	};

	::ftwl::error_t														setup								(::ftwapp::SApplication& applicationInstance);
	::ftwl::error_t														update								(::ftwapp::SApplication& applicationInstance);
	::ftwl::error_t														render								(::ftwapp::SApplication& applicationInstance);
	::ftwl::error_t														cleanup								(::ftwapp::SApplication& applicationInstance);
}
