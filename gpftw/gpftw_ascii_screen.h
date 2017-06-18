#include "gpftw_array_view.h"
#include "gpftw_error.h"

#ifndef GPFTW_ASCII_SCREEN_H
#define GPFTW_ASCII_SCREEN_H

namespace gpftw 
{
	struct SASCIIScreenDetail {

	};

	struct SASCIIScreen {
		uint32_t							Width					= 0;			
		uint32_t							Height					= 0;
		::gpftw::array_view<uint8_t>		Characters				= {};
		::gpftw::array_view<uint16_t>		Colors					= {};
		::gpftw::array_view<uint32_t>		Palette					= {};
	};

	::gpftw::error_t					createConsole			(SASCIIScreen& console, int width, int height);
	::gpftw::error_t					destroyConsole			(SASCIIScreen& console);
	::gpftw::error_t					presentConsole			(SASCIIScreen& console);
	::gpftw::error_t					clearConsole			(SASCIIScreen& console);
}

#endif GPFTW_ASCII_SCREEN_H
