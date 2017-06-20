#include "gpftw_array_view.h"	// for array_view<>
#include "gpftw_error.h"		// for error_t and failed()

#ifndef GPFTW_ASCII_SCREEN_H
#define GPFTW_ASCII_SCREEN_H

namespace ftwlib 
{
	struct SScreenASCII {
		uint32_t									Width					= 0;			
		uint32_t									Height					= 0;
		::ftwlib::array_view<uint8_t>				Characters				= {};
		::ftwlib::array_view<uint16_t>				Colors					= {};
		::ftwlib::array_view<uint32_t>				Palette					= {};
	};

	::ftwlib::error_t							createConsole			(SScreenASCII& console, int width, int height);
	::ftwlib::error_t							destroyConsole			(SScreenASCII& console);
	::ftwlib::error_t							presentConsole			(SScreenASCII& console);
	::ftwlib::error_t							clearConsole			(SScreenASCII& console);
}

#endif GPFTW_ASCII_SCREEN_H
