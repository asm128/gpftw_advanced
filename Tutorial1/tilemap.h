#include "gpftw_grid_view.h"
#include "gpftw_error.h"

#ifndef GPFTW_MAP_H
#define GPFTW_MAP_H

struct STileASCII {
						uint8_t									Character;
						uint16_t								Color;
};

struct STileMap {
						::gpftw::grid_view	<uint32_t>			ASCIIMap							= {};
						::gpftw::array_view	<const STileASCII>	TileDescriptionTable				= {};
};

					::gpftw::error_t						initializeTileMap					(STileMap& tileMapToInitialize, uint32_t width, uint32_t height, const STileASCII* tileDescriptions, uint32_t tileDescriptionCount);
					::gpftw::error_t						destroyTileMap						(STileMap& tileMapToInitialize);

#endif // GPFTW_MAP_H