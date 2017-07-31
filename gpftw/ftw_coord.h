#include "ftw_math.h"

#ifndef FTW_COORD_H_928374982364923642
#define FTW_COORD_H_928374982364923642

namespace ftwlib {

#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	template<typename _tBase>
	struct SCoord2 {
		typedef													SCoord2<_tBase>			TCoord2;
																_tBase					x, y;
		//
		inline constexpr										TCoord2					operator+				(const TCoord2& other)												const	noexcept	{ return {x + other.x, y + other.y};									}
		inline constexpr										TCoord2					operator-				(const TCoord2& other)												const	noexcept	{ return {x - other.x, y - other.y};									}
		inline constexpr										TCoord2					operator*				(double scalar)														const	noexcept	{ return {(_tBase)(x * scalar), (_tBase)(y * scalar)};					}
		inline constexpr										TCoord2					operator/				(double scalar)														const				{ return {(_tBase)(x / scalar), (_tBase)(y / scalar)};					}
		inline constexpr										TCoord2					operator*				(int64_t scalar)													const	noexcept	{ return {(_tBase)(x * scalar), (_tBase)(y * scalar)};					}
		inline constexpr										TCoord2					operator/				(int64_t scalar)													const				{ return {(_tBase)(x / scalar), (_tBase)(y / scalar)};					}
		inline constexpr										TCoord2					operator*				(uint64_t scalar)													const	noexcept	{ return {(_tBase)(x * scalar), (_tBase)(y * scalar)};					}
		inline constexpr										TCoord2					operator/				(uint64_t scalar)													const				{ return {(_tBase)(x / scalar), (_tBase)(y / scalar)};					}
		//
		inline													TCoord2&				operator+=				(const TCoord2& other)														noexcept	{ x += other.x; y += other.y;							return *this;	}
		inline													TCoord2&				operator-=				(const TCoord2& other)														noexcept	{ x -= other.x; y -= other.y;							return *this;	}
		inline													TCoord2&				operator*=				(double scalar)																noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar);	return *this;	}
		inline													TCoord2&				operator/=				(double scalar)																			{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar);	return *this;	}
		inline													TCoord2&				operator*=				(int64_t scalar)															noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar);	return *this;	}
		inline													TCoord2&				operator/=				(int64_t scalar)																		{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar);	return *this;	}
		inline													TCoord2&				operator*=				(uint64_t scalar)															noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar);	return *this;	}
		inline													TCoord2&				operator/=				(uint64_t scalar)																		{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar);	return *this;	}
		//
		inline constexpr										bool					operator==				(const TCoord2& other)												const	noexcept	{ return x == other.x && y == other.y;									}
		inline constexpr										bool					operator!=				(const TCoord2& other)												const	noexcept	{ return !operator==(other);											}
		inline constexpr										TCoord2					operator-				()																	const	noexcept	{ return {x*-1, y*-1};													}
		//
		template<typename _t>
		constexpr inline										SCoord2<_t>				Cast					()																	const	noexcept	{ return {(_t)x, (_t)y};																									}
		inline													TCoord2&				Scale					(double scalar)																noexcept	{ return *this *= scalar;																									}
		inline													TCoord2&				Normalize				()																						{ const _tBase sqLen = LengthSquared(); return (sqLen) ? *this /= ::ftwlib::sqrt_safe(sqLen) : *this;						}
		inline constexpr										double					Dot						(const TCoord2& other)												const	noexcept	{ return x * other.x + y * other.y;																							}
		inline constexpr										_tBase					LengthSquared			()																	const	noexcept	{ return x * x + y * y;																										}
		inline constexpr										double					Length					()																	const				{ const _tBase sqLen = LengthSquared(); return (sqLen) ? ::sqrt(sqLen) : 0;													}
		inline constexpr										double					AngleWith				(const TCoord2& other)												const				{ double lengthsProduct = Length() * other.Length(); return lengthsProduct ? ::acos( Dot(other) / (lengthsProduct) ) : 0;	}
		inline													void					AddScaled				(const TCoord2& vectorToScaleAndAdd, float scale)										{
			x																				+= vectorToScaleAndAdd.x * scale;
			y																				+= vectorToScaleAndAdd.y * scale;
		}
																TCoord2&				Rotate					(double theta)																			{
			const ::ftwlib::SPairSinCos															pairSinCos				= ::ftwlib::getSinCos(theta);
			const double																		px						= x * pairSinCos.Cos - y * pairSinCos.Sin; 

			y																				= (_tBase)(x * pairSinCos.Sin + y * pairSinCos.Cos);
			x																				= (_tBase)px;
			return *this;
		}
	};	// struct SCoord2

#define NWOL_DEFAULT_OPERATOR_NE(_otherType, ...)	\
		inline constexpr									bool				operator!=				(const _otherType		& other)											const	noexcept	{ return !operator==(other);	}	\
		inline constexpr									bool				operator==				(const _otherType		& other)											const	noexcept	{ return __VA_ARGS__;			}

	// ---- Geometric figures and other coord-related POD structs.
	template<typename _tElement>	struct SRange			{ _tElement				Offset, Count			; NWOL_DEFAULT_OPERATOR_NE(SRange		<_tElement>,	Offset	== other.Offset	&& Count	== other.Count							); };
	template<typename _tElement>	struct SLine2D			{ SCoord2<_tElement>	A, B					; NWOL_DEFAULT_OPERATOR_NE(SLine2D		<_tElement>,	A		== other.A		&& B		== other.B								); };
	template<typename _tElement>	struct STriangle2D		{ SCoord2<_tElement>	A, B, C					; NWOL_DEFAULT_OPERATOR_NE(STriangle2D	<_tElement>,	A		== other.A		&& B		== other.B			&& C == other.C		); };
	template<typename _tElement>	struct SRectangle2D		{ SCoord2<_tElement>	Offset, Size			; NWOL_DEFAULT_OPERATOR_NE(SRectangle2D	<_tElement>,	Offset	== other.Offset	&& Size		== other.Size							); };
	template<typename _tElement>	struct SCircle2D		{ double Radius;	SCoord2<_tElement> Center	; NWOL_DEFAULT_OPERATOR_NE(SCircle2D	<_tElement>,	Center	== other.Center	&& Radius	== other.Radius							); };
	template<typename _tElement>	struct SSphere2D		{ double Radius;	SCoord2<_tElement> Center	; NWOL_DEFAULT_OPERATOR_NE(SSphere2D	<_tElement>,	Center	== other.Center	&& Radius	== other.Radius							); };
#pragma pack(pop)

	// ---- Line
	template<typename _tElement>	static inline constexpr	_tElement				rise					(const SLine2D<_tElement>& line)											noexcept	{ return line.B.y - line.A.y;		}
	template<typename _tElement>	static inline constexpr	_tElement				run						(const SLine2D<_tElement>& line)											noexcept	{ return line.B.x - line.A.x;		}
	template<typename _tElement>	static inline constexpr	_tElement				slope					(const SLine2D<_tElement>& line)											noexcept	{ return rise(line) / run(line);	}

	// ---- Collision
	template<typename _tElement>	static					bool					raySegmentIntersect		(SCoord2<_tElement> r_d, SCoord2<_tElement> r_p, SCoord2<_tElement> s_d, SCoord2<_tElement> s_p)								{
		const double																		t2						= (r_d.x * (s_p.y - r_p.y) + r_d.y * (r_p.x - s_p.x)) / (s_d.x * r_d.y - s_d.y * r_d.x);
		const double																		t1						= (s_p.x + s_d.x * t2 - r_p.x) / r_d.x;	// Plug the value of T2 to get T1
		return (t2 > 0 && 0 < t2 && t2 < 1);
	}

	template<typename _tElement>	static					bool					sphereOverlaps			(const SSphere2D<_tElement> &sphereA, const SSphere2D<_tElement> &sphereB)	noexcept	{
		const double																		distanceSquared			= (sphereA.Center - sphereB.Center).squareMagnitude();
		const double																		radiiSum				= (sphereA.Radius + sphereB.Radius);
		return distanceSquared < (radiiSum * radiiSum);	// check squared distance against squared radius
	}

	// Returns the volume of a sphere. This is used to calculate how to recurse into the bounding volume tree. For a bounding sphere it is a simple calculation.
	template<typename _tElement>	static					double					sphereSize				(const SSphere2D<_tElement> &sphere)										noexcept	{ return 1.3333333333333333 * ::ftwlib::math_pi * sphere.Radius * sphere.Radius * sphere.Radius; }

}

#endif // FTW_COORD_H_928374982364923642