#include "ftw_math.h"

#ifndef GPFTW_COORD_H_928374982364923642
#define GPFTW_COORD_H_928374982364923642

namespace ftwlib {

#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	template<typename _tBase>
	struct SCoord2 {
		typedef			SCoord2<_tBase>	TCoord2;
							_tBase		x, y;
		//
		inline constexpr	TCoord2		operator+				(const TCoord2& other)										const	noexcept	{ return {x + other.x, y + other.y};									}
		inline constexpr	TCoord2		operator-				(const TCoord2& other)										const	noexcept	{ return {x - other.x, y - other.y};									}
		inline constexpr	TCoord2		operator*				(double scalar)												const	noexcept	{ return {(_tBase)(x * scalar), (_tBase)(y * scalar)};					}
		inline constexpr	TCoord2		operator/				(double scalar)												const				{ return {(_tBase)(x / scalar), (_tBase)(y / scalar)};					}
		inline constexpr	TCoord2		operator*				(int64_t scalar)											const	noexcept	{ return {(_tBase)(x * scalar), (_tBase)(y * scalar)};					}
		inline constexpr	TCoord2		operator/				(int64_t scalar)											const				{ return {(_tBase)(x / scalar), (_tBase)(y / scalar)};					}
		inline constexpr	TCoord2		operator*				(uint64_t scalar)											const	noexcept	{ return {(_tBase)(x * scalar), (_tBase)(y * scalar)};					}
		inline constexpr	TCoord2		operator/				(uint64_t scalar)											const				{ return {(_tBase)(x / scalar), (_tBase)(y / scalar)};					}
		//
		inline				TCoord2&	operator+=				(const TCoord2& other)												noexcept	{ x += other.x; y += other.y;							return *this;	}
		inline				TCoord2&	operator-=				(const TCoord2& other)												noexcept	{ x -= other.x; y -= other.y;							return *this;	}
		inline				TCoord2&	operator*=				(double scalar)														noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar);	return *this;	}
		inline				TCoord2&	operator/=				(double scalar)																	{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar);	return *this;	}
		inline				TCoord2&	operator*=				(int64_t scalar)													noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar);	return *this;	}
		inline				TCoord2&	operator/=				(int64_t scalar)																{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar);	return *this;	}
		inline				TCoord2&	operator*=				(uint64_t scalar)													noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar);	return *this;	}
		inline				TCoord2&	operator/=				(uint64_t scalar)																{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar);	return *this;	}
		//
		inline constexpr	bool		operator==				(const TCoord2& other)										const	noexcept	{ return x == other.x && y == other.y;									}
		inline constexpr	bool		operator!=				(const TCoord2& other)										const	noexcept	{ return !operator==(other);											}
		inline constexpr	TCoord2		operator-				()															const	noexcept	{ return {x*-1, y*-1};													}
		//
		template<typename _t>
		constexpr inline	SCoord2<_t>	Cast					()															const	noexcept	{ return {(_t)x, (_t)y};																									}
		inline				TCoord2&	Scale					(double scalar)														noexcept	{ return *this *= scalar;																									}
		inline				TCoord2&	Normalize				()																				{ const _tBase sqLen = LengthSquared(); return (sqLen) ? *this /= ::ftwlib::sqrt_safe(sqLen) : *this;						}
		inline constexpr	double		Dot						(const TCoord2& other)										const	noexcept	{ return x * other.x + y * other.y;																							}
		inline constexpr	_tBase		LengthSquared			()															const	noexcept	{ return x * x + y * y;																										}
		inline constexpr	double		Length					()															const				{ const _tBase sqLen = LengthSquared(); return (sqLen) ? ::sqrt(sqLen) : 0;													}
		inline constexpr	double		AngleWith				(const TCoord2& other)										const				{ double lengthsProduct = Length() * other.Length(); return lengthsProduct ? ::acos( Dot(other) / (lengthsProduct) ) : 0;	}
		inline				void		AddScaled				(const TCoord2& vectorToScaleAndAdd, float scale)								{
			x								+= vectorToScaleAndAdd.x * scale;
			y								+= vectorToScaleAndAdd.y * scale;
		}
							TCoord2&	Rotate					(double theta)																	{
			const ::ftwlib::SPairSinCos			pairSinCos				= ::ftwlib::getSinCos(theta);
			const double						px						= x * pairSinCos.Cos - y * pairSinCos.Sin; 

			y								= (_tBase)(x * pairSinCos.Sin + y * pairSinCos.Cos);
			x								= (_tBase)px;
			return *this;
		}
	};	// struct SCoord2

#define NWOL_DEFAULT_OPERATOR_NE(_otherType)	inline constexpr bool operator !=(const _otherType& other )	const	noexcept	{ return !operator==(other); } 
	// Geometric figures and other coord-related POD structs.
	template<typename _tCoord> struct	SRange			{ _tCoord			Offset, Count			; NWOL_DEFAULT_OPERATOR_NE(SRange		<_tCoord>); inline constexpr bool operator==(const	SRange			<_tCoord>	& other) const noexcept { return Offset		== other.Offset		&& Count		== other.Count;							} };
	template<typename _tCoord> struct	SLine2D			{ SCoord2<_tCoord>	A, B					; NWOL_DEFAULT_OPERATOR_NE(SLine2D		<_tCoord>); inline constexpr bool operator==(const	SLine2D			<_tCoord>	& other) const noexcept { return A			== other.A			&& B			== other.B;								} };
	template<typename _tCoord> struct	STriangle2D		{ SCoord2<_tCoord>	A, B, C					; NWOL_DEFAULT_OPERATOR_NE(STriangle2D	<_tCoord>); inline constexpr bool operator==(const	STriangle2D		<_tCoord>	& other) const noexcept { return A			== other.A			&& B			== other.B			&& C == other.C;	} };
	template<typename _tCoord> struct	SRectangle2D	{ SCoord2<_tCoord>	Offset, Size			; NWOL_DEFAULT_OPERATOR_NE(SRectangle2D	<_tCoord>); inline constexpr bool operator==(const	SRectangle2D	<_tCoord>	& other) const noexcept { return Offset		== other.Offset		&& Size			== other.Size;							} };
	template<typename _tCoord> struct	SCircle2D		{ double Radius;	SCoord2<_tCoord> Center	; NWOL_DEFAULT_OPERATOR_NE(SCircle2D	<_tCoord>); inline constexpr bool operator==(const	SCircle2D		<_tCoord>	& other) const noexcept { return Center		== other.Center		&& Radius		== other.Radius;						} };
	template<typename _tCoord> struct	SSphere2D		{ double Radius;	SCoord2<_tCoord> Center	; NWOL_DEFAULT_OPERATOR_NE(SSphere2D	<_tCoord>); inline constexpr bool operator==(const	SSphere2D		<_tCoord>	& other) const noexcept { return Center		== other.Center		&& Radius		== other.Radius;						} };
#pragma pack(pop)



}

#endif // GPFTW_COORD_H_928374982364923642