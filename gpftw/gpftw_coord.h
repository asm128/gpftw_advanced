#include <cmath>
#include <cstdint>

#ifndef GPFTW_COORD_H_928374982364923642
#define GPFTW_COORD_H_928374982364923642

namespace ftwlib {
#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	template<typename _tNumber>	inline constexpr	double				sqrt_safe				(_tNumber _valueSquared)											noexcept	{ return _valueSquared ? sqrt(_valueSquared) : 0;						}

	struct											SPairSinCos			{ double Sin, Cos; };	

	static inline									SPairSinCos			getSinCos				(double theta)														noexcept	{ return {sin(theta), cos(theta)};										}

	template<typename _tBase>
	struct SCoord2 {
		typedef			SCoord2<_tBase>	TCoord2;
							_tBase											x, y;
		//
		inline constexpr	TCoord2											operator+				(const TCoord2& other)										const	noexcept	{ return {x + other.x, y + other.y};									}
		inline constexpr	TCoord2											operator-				(const TCoord2& other)										const	noexcept	{ return {x - other.x, y - other.y};									}
		inline constexpr	TCoord2											operator*				(double scalar)												const	noexcept	{ return {(_tBase)(x * scalar), (_tBase)(y * scalar)};					}
		inline constexpr	TCoord2											operator/				(double scalar)												const				{ return {(_tBase)(x / scalar), (_tBase)(y / scalar)};					}
		inline constexpr	TCoord2											operator*				(int64_t scalar)											const	noexcept	{ return {(_tBase)(x * scalar), (_tBase)(y * scalar)};					}
		inline constexpr	TCoord2											operator/				(int64_t scalar)											const				{ return {(_tBase)(x / scalar), (_tBase)(y / scalar)};					}
		inline constexpr	TCoord2											operator*				(uint64_t scalar)											const	noexcept	{ return {(_tBase)(x * scalar), (_tBase)(y * scalar)};					}
		inline constexpr	TCoord2											operator/				(uint64_t scalar)											const				{ return {(_tBase)(x / scalar), (_tBase)(y / scalar)};					}
		//
		inline				TCoord2&										operator+=				(const TCoord2& other)												noexcept	{ x += other.x; y += other.y;							return *this;	}
		inline				TCoord2&										operator-=				(const TCoord2& other)												noexcept	{ x -= other.x; y -= other.y;							return *this;	}
		inline				TCoord2&										operator*=				(double scalar)														noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar);	return *this;	}
		inline				TCoord2&										operator/=				(double scalar)																	{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar);	return *this;	}
		inline				TCoord2&										operator*=				(int64_t scalar)													noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar);	return *this;	}
		inline				TCoord2&										operator/=				(int64_t scalar)																{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar);	return *this;	}
		inline				TCoord2&										operator*=				(uint64_t scalar)													noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar);	return *this;	}
		inline				TCoord2&										operator/=				(uint64_t scalar)																{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar);	return *this;	}
		//
		inline constexpr	bool											operator==				(const TCoord2& other)										const	noexcept	{ return x == other.x && y == other.y;									}
		inline constexpr	bool											operator!=				(const TCoord2& other)										const	noexcept	{ return !operator==(other);											}
		inline constexpr	TCoord2											operator-				()															const	noexcept	{ return {x*-1, y*-1};													}
		//
		template<typename _t>
		constexpr inline	SCoord2<_t>										Cast					()															const	noexcept	{ return {(_t)x, (_t)y};																			}
		inline				TCoord2&										Scale					(double scalar)														noexcept	{ return *this *= scalar;																			}
		inline				TCoord2&										Normalize				()																				{ _tBase sqLen = LengthSquared(); return (sqLen) ? *this /= ::ftwlib::sqrt_safe(sqLen) : *this;		}
		inline constexpr	double											Dot						(const TCoord2& other)										const	noexcept	{ return x * other.x + y * other.y;																	}
		inline constexpr	_tBase											LengthSquared			()															const	noexcept	{ return x * x + y * y;																				}
		inline constexpr	double											Length					()															const				{ return ::sqrt(LengthSquared());																	}
		inline constexpr	double											AngleWith				(const TCoord2& other)										const				{ return ::acos( Dot(other) / (Length() * other.Length()) );										}
		inline				void											AddScaled				(const TCoord2& vectorToScaleAndAdd, float scale)								{
			x																	+= vectorToScaleAndAdd.x * scale;
			y																	+= vectorToScaleAndAdd.y * scale;
		}
							TCoord2&										Rotate					(double theta)																	{
			const ::ftwlib::SPairSinCos												pairSinCos				= ::ftwlib::getSinCos(theta);
			const double															px						= x * pairSinCos.Cos - y * pairSinCos.Sin; 

			y																	= (_tBase)(x * pairSinCos.Sin + y * pairSinCos.Cos);
			x																	= (_tBase)px;
			return *this;
		}
	};	// struct SCoord2
#pragma pack(pop)
}

#endif // GPFTW_COORD_H_928374982364923642