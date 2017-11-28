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
}

#endif // FTW_ASCII_TARGET_H_29874982734