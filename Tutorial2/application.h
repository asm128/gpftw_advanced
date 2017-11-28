#include "game.h"

#include "ftw_ascii_screen.h"

#include "ftw_timer.h"

namespace ftwapp {
	struct SApplication {
		int64_t																	FrameCounter						= 0;	// Declare and initialize a variable of (int)eger type for keeping track of the number of frame since execution began.
		::ftwl::SASCIITarget													ASCIIRenderTarget					= {};
		::ftwl::SPalette														Palette								= 
			{	(uint32_t)::ftwl::ASCII_COLOR_INDEX_0		
			,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_1 	
			,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_2 	
			,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_3 	
			,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_4 	
			,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_5 	
			,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_6 	
			,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_7 	
			,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_8 	
			,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_9 	
			,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_10	
			,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_11	
			,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_12	
			,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_13	
			,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_14	
			,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_15	
			};

		::ftwl::STimer															Timer								= {};
		::game::SGame															Game								= {};
	};

	::ftwl::error_t															setup								(::ftwapp::SApplication& applicationInstance);
	::ftwl::error_t															update								(::ftwapp::SApplication& applicationInstance);
	::ftwl::error_t															render								(::ftwapp::SApplication& applicationInstance);
	::ftwl::error_t															cleanup								(::ftwapp::SApplication& applicationInstance);
}
