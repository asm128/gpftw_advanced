#include "game.h"

#include "gpftw_ascii_screen.h"

#include "gpftw_timer.h"

template<typename _tValue>	static inline constexpr	const _tValue		max									(const _tValue& a, const _tValue& b)				{ return (a > b) ? a : b; }
template<typename _tValue>	static inline constexpr	const _tValue		min									(const _tValue& a, const _tValue& b)				{ return (a < b) ? a : b; }

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