#include "ftw_array_view.h"	// for array_view<>
#include "ftw_error.h"		// for error_t and failed()

#ifndef GPFTW_ASCII_SCREEN_H
#define GPFTW_ASCII_SCREEN_H

namespace ftwl 
{
	struct SScreenASCII {
		uint32_t									Width					= 0;			
		uint32_t									Height					= 0;
		::ftwl::array_view<uint8_t>				Characters				= {};
		::ftwl::array_view<uint16_t>				Colors					= {};
		::ftwl::array_view<uint32_t>				Palette					= {};
	};

	::ftwl::error_t							consoleCreate			(SScreenASCII& console, int width, int height);
	::ftwl::error_t							consoleDestroy			(SScreenASCII& console);
	::ftwl::error_t							consolePresent			(SScreenASCII& console);
	::ftwl::error_t							consoleClear			(SScreenASCII& console);
}

#endif GPFTW_ASCII_SCREEN_H
