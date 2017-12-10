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

	static inline				::ftwl::error_t									drawRectangle								(::ftwl::SASCIITarget& asciiTarget, const ::ftwl::SASCIICell& value, const ::ftwl::SRectangle2D<int32_t>& rectangle)	{
		for(int32_t y = ::ftwl::max(0, rectangle.Offset.y), yStop = ::ftwl::min(rectangle.Offset.y + rectangle.Size.y, (int32_t)asciiTarget.Height	()); y < yStop; ++y)
		for(int32_t x = ::ftwl::max(0, rectangle.Offset.x), xStop = ::ftwl::min(rectangle.Offset.x + rectangle.Size.x, (int32_t)asciiTarget.Width	()); x < xStop; ++x) {	
			asciiTarget.Characters	[y][x]												= value.Character;
			asciiTarget.Colors		[y][x]												= value.Color;
		}
		return 0;
	}

	static inline				::ftwl::error_t									drawCircle									(::ftwl::SASCIITarget& asciiTarget, const ::ftwl::SASCIICell& value, const ::ftwl::SCircle2D<int32_t>& circle)			{
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

	static inline				::ftwl::error_t									drawTriangle								(::ftwl::SASCIITarget& asciiTarget, const ::ftwl::SASCIICell& value, const ::ftwl::STriangle2D<int32_t>& triangle)		{
		::ftwl::SCoord2		<int32_t>													areaMin										= {::ftwl::min(::ftwl::min(triangle.A.x, triangle.B.x), triangle.C.x), ::ftwl::min(::ftwl::min(triangle.A.x, triangle.B.x), triangle.C.x)};
		::ftwl::SCoord2		<int32_t>													areaMax										= {::ftwl::max(::ftwl::max(triangle.A.x, triangle.B.x), triangle.C.x), ::ftwl::max(::ftwl::max(triangle.A.x, triangle.B.x), triangle.C.x)};
		::ftwl::SLine2D		<double>													segment0									= {triangle.A.Cast<double>(), triangle.B.Cast<double>()};
		::ftwl::SLine2D		<double>													segment1									= {triangle.B.Cast<double>(), triangle.C.Cast<double>()};
		::ftwl::SLine2D		<double>													segment2									= {triangle.C.Cast<double>(), triangle.A.Cast<double>()};
		::ftwl::SRectangle2D<int32_t>													outerRect									= 
			{	{ ::ftwl::max(0, areaMin.y)
				, ::ftwl::max(0, areaMin.x)
				}
			,	{ ::ftwl::min(areaMax.y, (int32_t)asciiTarget.Height	())
				, ::ftwl::min(areaMax.x, (int32_t)asciiTarget.Width		())
				}
			};	

		for(int32_t y = outerRect.Offset.y, yStop = outerRect.Offset.y + outerRect.Size.y; y < yStop; ++y)
		for(int32_t x = outerRect.Offset.x, xStop = outerRect.Offset.x + outerRect.Size.x; x < xStop; ++x) {	
			::ftwl::SCoord2<int32_t>														cellCurrent									= {x, y};
			const double																	y0											= (segment0.B.y - segment0.A.y) / (y - segment0.A.y);
			const double																	y1											= (segment1.B.y - segment1.A.y) / (y - segment1.A.y);
			const double																	y2											= (segment2.B.y - segment2.A.y) / (y - segment2.A.y);
			bool																			paintCell									= false;	// Here we should set this variable to true or false depending on how many segments we've crossed from the left border of the rectangle
			if(x > segment0.A.x + (segment0.B.x - segment0.A.x) * y0)
				paintCell																	= !paintCell;
			if(x > segment1.A.x + (segment1.B.x - segment1.A.x) * y1)
				paintCell																	= !paintCell;
			if(x > segment2.A.x + (segment2.B.x - segment2.A.x) * y2)
				paintCell																	= !paintCell;
			if(paintCell) {
				asciiTarget.Characters	[y][x]												= value.Character;
				asciiTarget.Colors		[y][x]												= value.Color;
			}
		}
		return 0;
	}

}

#endif // FTW_ASCII_TARGET_H_29874982734
