#include <cmath>
#include <cstdint>

#ifndef FTW_MATH_H_0237409283740982740928374
#define FTW_MATH_H_0237409283740982740928374

namespace ftwlib {

#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	template<typename _tNumber>	inline constexpr	double				sqrt_safe				(_tNumber _valueSquared)											noexcept	{ return _valueSquared ? sqrt(_valueSquared) : 0;						}

	struct											SPairSinCos			{ double Sin, Cos; };	

	static inline									SPairSinCos			getSinCos				(double theta)														noexcept	{ return {sin(theta), cos(theta)};										}
#pragma pack(pop)
}

#endif // FTW_MATH_H_0237409283740982740928374
