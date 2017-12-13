#include "ftw_ascii_color.h"
#include "ftw_grid_view.h"
#include "ftw_error.h"
#include "ftw_coord.h"
#include "ftw_color.h"
#include <vector>

#ifndef FTW_ASCII_TARGET_H_29874982734
#define FTW_ASCII_TARGET_H_29874982734

namespace ftwl 
{
	typedef					::std::vector<::ftwl::SColorRGBA>					SPalette;

	struct SASCIITarget {
								::ftwl::grid_view<uint8_t>						Characters									= {};
								::ftwl::grid_view<uint16_t>						Colors										= {};

		inline constexpr		uint32_t										Width										()																	const	noexcept	{ return Characters.width	(); }
		inline constexpr		uint32_t										Height										()																	const	noexcept	{ return Characters.height	(); }
	};

							::ftwl::error_t									asciiTargetCreate							(::ftwl::SASCIITarget& target, uint32_t width, uint32_t height);
							::ftwl::error_t									asciiTargetDestroy							(::ftwl::SASCIITarget& target);
							::ftwl::error_t									asciiTargetClear							(::ftwl::SASCIITarget& target, uint8_t character = ' ', uint16_t color = ASCII_COLOR_WHITE);

	template<typename _tUnit>
	static inline			::ftwl::error_t									asciiTargetCreate							(::ftwl::SASCIITarget& target, const ::ftwl::SCoord2<_tUnit>& size)						{ return ::ftwl::asciiTargetCreate(console, (uint32_t)size.x, (uint32_t)size.y); }

	// ------------------------------------------------------ 
	struct SASCIICell {
								uint8_t											Character	;
								uint16_t										Color		;
	};

	static inline			::ftwl::error_t									drawRectangle								(::ftwl::SASCIITarget& asciiTarget, const ::ftwl::SASCIICell& value, const ::ftwl::SRectangle2D<int32_t>& rectangle)	{
		for(int32_t y = ::ftwl::max(0, rectangle.Offset.y), yStop = ::ftwl::min(rectangle.Offset.y + rectangle.Size.y, (int32_t)asciiTarget.Height	()); y < yStop; ++y)
		for(int32_t x = ::ftwl::max(0, rectangle.Offset.x), xStop = ::ftwl::min(rectangle.Offset.x + rectangle.Size.x, (int32_t)asciiTarget.Width	()); x < xStop; ++x) {	
			asciiTarget.Characters	[y][x]												= value.Character;
			asciiTarget.Colors		[y][x]												= value.Color;
		}
		return 0;
	}

	static inline			::ftwl::error_t									drawCircle									(::ftwl::SASCIITarget& asciiTarget, const ::ftwl::SASCIICell& value, const ::ftwl::SCircle2D<int32_t>& circle)			{
		for(int32_t y = ::ftwl::max(0, (int32_t)(circle.Center.y - circle.Radius)), yStop = ::ftwl::min((int32_t)(circle.Center.y + circle.Radius), (int32_t)asciiTarget.Height	()); y < yStop; ++y)
		for(int32_t x = ::ftwl::max(0, (int32_t)(circle.Center.x - circle.Radius)), xStop = ::ftwl::min((int32_t)(circle.Center.x + circle.Radius), (int32_t)asciiTarget.Width	()); x < xStop; ++x) {	
			::ftwl::SCoord2<int32_t>														cellCurrent									= {x, y};
			double																			distance									= (cellCurrent - circle.Center).Length();
			if(distance < circle.Radius) {
				asciiTarget.Characters	[y][x]												= value.Character;
				asciiTarget.Colors		[y][x]												= value.Color;
			}
		}
		return 0;
	}

	// A good article on this kind of triangle rasterization: https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/ 
	static inline			::ftwl::error_t									drawTriangle								(::ftwl::SASCIITarget& asciiTarget, const ::ftwl::SASCIICell& value, const ::ftwl::STriangle2D<int32_t>& triangle)		{
		::ftwl::SCoord2		<int32_t>												areaMin										= {::ftwl::min(::ftwl::min(triangle.A.x, triangle.B.x), triangle.C.x), ::ftwl::min(::ftwl::min(triangle.A.y, triangle.B.y), triangle.C.y)};
		::ftwl::SCoord2		<int32_t>												areaMax										= {::ftwl::max(::ftwl::max(triangle.A.x, triangle.B.x), triangle.C.x), ::ftwl::max(::ftwl::max(triangle.A.y, triangle.B.y), triangle.C.y)};
		for(int32_t y = ::ftwl::max(areaMin.y, 0), yStop = ::ftwl::min(areaMax.y, (int32_t)asciiTarget.Height	()); y < yStop; ++y)
		for(int32_t x = ::ftwl::max(areaMin.x, 0), xStop = ::ftwl::min(areaMax.x, (int32_t)asciiTarget.Width	()); x < xStop; ++x) {	
			const ::ftwl::SCoord2<int32_t>													cellCurrent									= {x, y};
			// Determine barycentric coordinates
			int																				w0											= ::ftwl::orient2d({triangle.A, triangle.B}, cellCurrent);
			int																				w1											= ::ftwl::orient2d({triangle.B, triangle.C}, cellCurrent);
			int																				w2											= ::ftwl::orient2d({triangle.C, triangle.A}, cellCurrent);
			if (w0 >= 0 && w1 >= 0 && w2 >= 0) { // If p is on or inside all edges, render pixel.
				asciiTarget.Characters	[y][x]												= value.Character;
				asciiTarget.Colors		[y][x]												= value.Color;
			}
		}
		return 0;
	}

	// Bresenham's line algorithm
	template<typename _tCoord>
	static					::ftwl::error_t									drawLine									(::ftwl::SASCIITarget& asciiTarget, const ::ftwl::SASCIICell& value, const ::ftwl::SLine2D<_tCoord>& line)				{
		float																		x1											= (float)line.A.x
			,																		y1											= (float)line.A.y
			,																		x2											= (float)line.B.x
			,																		y2											= (float)line.B.y
			;
		const bool																	steep										= (fabs(y2 - y1) > fabs(x2 - x1));
		if(steep){
			::std::swap(x1, y1);
			::std::swap(x2, y2);
		}
		if(x1 > x2) {
			::std::swap(x1, x2);
			::std::swap(y1, y2);
		}
		const float																	dx											= x2 - x1;
		const float																	dy											= fabs(y2 - y1);
		float																		error										= dx / 2.0f;
		const int32_t																ystep										= (y1 < y2) ? 1 : -1;
		int32_t																		y											= (int32_t)y1;
		for(int32_t x = (int32_t)x1, xStop = (int32_t)x2; x < xStop; ++x) {
			if(steep) {
				if(false == ::ftwl::in_range(x, 0, (int32_t)asciiTarget.Height()) || false == ::ftwl::in_range(y, 0, (int32_t)asciiTarget.Width()))
					continue;
				asciiTarget.Characters	[x][y]											= value.Character;
				asciiTarget.Colors		[x][y]											= value.Color;
			}
			else {
				if(false == ::ftwl::in_range(y, 0, (int32_t)asciiTarget.Height()) || false == ::ftwl::in_range(x, 0, (int32_t)asciiTarget.Width()))
					continue;
				asciiTarget.Characters	[y][x]											= value.Character;
				asciiTarget.Colors		[y][x]											= value.Color;
			}
 
			error																-= dy;
			if(error < 0) {
				y																	+= ystep;
				error																+= dx;
			}
		}
		return 0;
	}
}

#endif // FTW_ASCII_TARGET_H_29874982734
