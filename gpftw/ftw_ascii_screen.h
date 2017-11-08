#include "ftw_grid_view.h"	// for grid_view<>
#include "ftw_error.h"		// for error_t and failed()

#ifndef GPFTW_ASCII_SCREEN_H
#define GPFTW_ASCII_SCREEN_H

namespace ftwl 
{
	struct SScreenASCII {
		uint32_t									Width					= 0;			
		uint32_t									Height					= 0;
		::ftwl::array_view<uint8_t>					Characters				= {};
		::ftwl::array_view<uint16_t>				Colors					= {};
		::ftwl::array_view<uint32_t>				Palette					= {};

		::ftwl::grid_view<uint8_t>					GridCharacters			= {};
		::ftwl::grid_view<uint16_t>					GridColors				= {};
	};

	::ftwl::error_t								consoleCreate			(SScreenASCII& console, uint32_t width, uint32_t height);
	::ftwl::error_t								consoleDestroy			(SScreenASCII& console);
	::ftwl::error_t								consolePresent			(SScreenASCII& console);
	::ftwl::error_t								consoleClear			(SScreenASCII& console);
	::ftwl::error_t								consoleSetTitle			(const char* title);
}

#endif // GPFTW_ASCII_SCREEN_H
