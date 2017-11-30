#include "ftw_grid_view.h"		// for grid_view<>
#include "ftw_ascii_target.h"	// for SASCIITarget

#ifndef GPFTW_ASCII_SCREEN_H
#define GPFTW_ASCII_SCREEN_H

namespace ftwl 
{
					::ftwl::error_t									asciiDisplayCreate					(uint32_t frontBufferWidth, uint32_t frontBufferHeight);
					::ftwl::error_t									asciiDisplayDestroy					();
					::ftwl::error_t									asciiDisplayPresent					(const ::ftwl::array_view<const uint8_t>& characters, const ::ftwl::array_view<const uint16_t>& colorRefs);
					::ftwl::error_t									asciiDisplayClear					(uint8_t character = ' ', uint16_t colorRef = ::ftwl::ASCII_COLOR_WHITE);
					::ftwl::error_t									asciiDisplaySize					(::ftwl::SCoord2<uint32_t>& size);

					::ftwl::error_t									asciiDisplayResize					(uint32_t frontBufferWidth, uint32_t frontBufferHeight);
					::ftwl::error_t									asciiDisplayPaletteSet				(const ::ftwl::array_view<::ftwl::SColorRGBA>& palette);
					::ftwl::error_t									asciiDisplayPaletteReset			();
					::ftwl::error_t									asciiDisplayTitleSet				(const ::ftwl::array_view<const char>& title);

	template <typename _tUnit>
	static inline	::ftwl::error_t									asciiDisplayCreate					(const ::ftwl::SCoord2<_tUnit>& displaySize)				{ return ::ftwl::asciiDisplayCreate(displaySize.x, displaySize.y); }
	static inline	::ftwl::error_t									asciiDisplayPresent					(const ::ftwl::SASCIITarget& renderTarget)					{
		return ::ftwl::asciiDisplayPresent
			( ::ftwl::array_view<const uint8_t >{renderTarget.Characters	.begin(), renderTarget.Characters	.size()}
			, ::ftwl::array_view<const uint16_t>{renderTarget.Colors		.begin(), renderTarget.Colors		.size()}
			);
	}
}

#endif // GPFTW_ASCII_SCREEN_H
