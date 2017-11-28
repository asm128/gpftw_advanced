#include "ftw_ascii_target.h"

				::ftwl::error_t									ftwl::asciiTargetCreate							(::ftwl::SASCIITarget& console, uint32_t width, uint32_t height)		{ 
	console.Characters												= {(uint8_t *)::malloc(sizeof(uint8_t ) * width * height), width, height}; 
	console.Colors													= {(uint16_t*)::malloc(sizeof(uint16_t) * width * height), width, height}; 
	return 0; 
}

				::ftwl::error_t									ftwl::asciiTargetDestroy						(::ftwl::SASCIITarget& console)											{ 
	if(console.Characters	.begin()) ::free(console.Characters	.begin());
	if(console.Colors		.begin()) ::free(console.Colors		.begin());
	return 0; 
}

				::ftwl::error_t									ftwl::asciiTargetClear							(::ftwl::SASCIITarget& console, uint8_t character, uint16_t color)		{ 
	uint8_t																* characters									= console.Characters	.begin();
	uint16_t															* colors										= console.Colors		.begin();
	for(uint32_t iCell = 0; iCell < console.Colors.size(); ++iCell) {
		console.Characters	.begin()[iCell]								= character;
		console.Colors		.begin()[iCell]								= color;
	}
	return 0; 
}
