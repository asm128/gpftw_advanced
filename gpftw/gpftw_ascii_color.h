#include <cstdint>	// for uint16_t and uint32_t 

#ifndef GPFTW_ASCII_COLOR_H
#define GPFTW_ASCII_COLOR_H

namespace gpftw 
{
	// This type is used to represent the palette array of the ASCII console. Having : uint16_t here makes ASCII_COLOR_INDEX to have the same memory footprint as an unsigned short.
	enum ASCII_COLOR_INDEX : uint16_t		
		{	ASCII_COLOR_INDEX_0			=	 0	// BLACK				
		,	ASCII_COLOR_INDEX_1 		=	 1 	// DARKBLUE			
		,	ASCII_COLOR_INDEX_2 		=	 2 	// DARKGREEN			
		,	ASCII_COLOR_INDEX_3 		=	 3 	// DARKCYAN			
		,	ASCII_COLOR_INDEX_4 		=	 4 	// DARKRED			
		,	ASCII_COLOR_INDEX_5 		=	 5 	// DARKMAGENTA		
		,	ASCII_COLOR_INDEX_6 		=	 6 	// DARKYELLOW		
		,	ASCII_COLOR_INDEX_7 		=	 7 	// LIGHTGREY 
		,	ASCII_COLOR_INDEX_8 		=	 8 	// DARKGREY			
		,	ASCII_COLOR_INDEX_9 		=	 9 	// BLUE				
		,	ASCII_COLOR_INDEX_10		=	10	// GREEN				
		,	ASCII_COLOR_INDEX_11		=	11	// CYAN				
		,	ASCII_COLOR_INDEX_12		=	12	// RED				
		,	ASCII_COLOR_INDEX_13		=	13	// MAGENTA			
		,	ASCII_COLOR_INDEX_14		=	14	// YELLOW			
		,	ASCII_COLOR_INDEX_15		=	15	// WHITE				
		,	ASCII_COLOR_INDEX_COUNT		=	16	// COUNT
		};

	// 3-Channel color array with a max value of 0xFF each. This means we use 24 bit for each color but as there is no fundamental type of 24-bit in C++ we choose the next larger, in this case an unsigned int of 32-bit size.
	// This particular channel arrangement has the red channel stored in the first 8 bits (the first byte), the green channel in the next 8 bits (the second byte), and the blue channel in the last 8 bits (the third byte) of the channel array. 
	// The remaining 8 bits must be set to 0 and I don't remember if they have an actual meaning for the console system. More complex graphics systems may use those remaining bits to represent transparency (the alpha channel) or other rendering-related information.
	enum ASCII_COLOR : uint32_t		
		{	ASCII_COLOR_BLACK			= 0x000000			
		,	ASCII_COLOR_DARKBLUE		= 0x800000			
		,	ASCII_COLOR_DARKGREEN		= 0x008000			
		,	ASCII_COLOR_DARKCYAN		= 0x808000			
		,	ASCII_COLOR_DARKRED			= 0x000080			
		,	ASCII_COLOR_DARKMAGENTA		= 0x800080			
		,	ASCII_COLOR_DARKYELLOW		= 0x008080			
		,	ASCII_COLOR_LIGHTGREY		= 0xC0C0C0			
		,	ASCII_COLOR_DARKGREY		= 0x808080			
		,	ASCII_COLOR_BLUE			= 0xFF0000			
		,	ASCII_COLOR_GREEN			= 0x00FF00			
		,	ASCII_COLOR_CYAN			= 0xFFFF00			
		,	ASCII_COLOR_RED				= 0x0000FF			
		,	ASCII_COLOR_MAGENTA			= 0xFF00FF			
		,	ASCII_COLOR_YELLOW			= 0x00FFFF			
		,	ASCII_COLOR_WHITE			= 0xFFFFFF			
		};

	// Swaps the background color and the font color.
	inline constexpr	ASCII_COLOR_INDEX			ascii_color_invert		(const ASCII_COLOR_INDEX color)				{ return (ASCII_COLOR_INDEX)(((color & 0xF0) >> 4) | ((color & 0xF) << 4));	}
}

// BLINK	128 
// #define invert_color_ascii(_col) (((_col & 0xF0)>>4) | ((_col & 0xF)<<4))

#endif // GPFTW_ASCII_COLOR_H