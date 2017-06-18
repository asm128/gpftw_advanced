#include <cstdint>

#ifndef GPFTW_ERROR_H
#define GPFTW_ERROR_H

namespace gpftw 
{
	typedef					int32_t					error_t;


	inline constexpr		bool					failed					(error_t errorCode)					{ return errorCode < 0; }
}

#endif // GPFTW_ERROR_H