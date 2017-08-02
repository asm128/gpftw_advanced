// Best viewed in 1080p with 88% zoom.
#include "ftw_grid.h"
#include "ftw_error.h"

#ifndef TILEMAP_H
#define TILEMAP_H

namespace game 
{
	struct STileASCII {
							uint8_t													Character;
							uint16_t												Color;
	};
	
	template <size_t _gridWidth, size_t _gridHeight>
	struct STileMapASCII {
		typedef				::ftwl::SGrid<int32_t, _gridWidth, _gridHeight>		TTileIndexGrid;

		static constexpr	const uint32_t											Width						= (uint32_t)_gridWidth;
		static constexpr	const uint32_t											Depth						= (uint32_t)_gridHeight;

							TTileIndexGrid											Cells						= {};
							::ftwl::array_view<const ::game::STileASCII>			TileDescriptionTable		= {};
	};
}

#endif // TILEMAP_H
