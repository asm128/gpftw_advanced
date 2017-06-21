#include <cmath>
#include <cstdint>

#ifndef FTW_MATH_H_0237409283740982740928374
#define FTW_MATH_H_0237409283740982740928374

namespace ftwlib {

#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	static											const double		math_pi				= 3.1415926535897932384626433832795;	// Pi
	static											const double		math_pi2			= 1.5707963267948966192313216916398;	// Pi/2
	static											const double		math_2pi			= 6.2831853071795864769252867665590;	// 2*Pi
	static											const double		math_pi_180			= math_pi / 180.0;						// Pi/180.0
	static constexpr								const double		math_epsilon		= 0.00001f						  ;

	template<typename _tNumber>	inline constexpr	double				sqrt_safe				(_tNumber _valueSquared)											noexcept	{ return _valueSquared ? sqrt(_valueSquared) : 0;	}

	struct											SPairSinCos			{ double Sin, Cos; };	

	static inline									SPairSinCos			getSinCos				(double theta)														noexcept	{ return {sin(theta), cos(theta)};					}
#pragma pack(pop)
}

#endif // FTW_MATH_H_0237409283740982740928374
