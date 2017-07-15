#include "ftw_grid_view.h"
#include "ftw_error.h"

#ifndef TILEMAP_H
#define TILEMAP_H

struct STileASCII {
						uint8_t									Character;
						uint16_t								Color;
};

struct STileMap {
						::ftwlib::grid_view<uint32_t>			ASCIIMap							= {};
						::ftwlib::array_view<const STileASCII>	TileDescriptionTable				= {};
};

					::ftwlib::error_t						initializeTileMap					(STileMap& tileMapToInitialize, uint32_t width, uint32_t height, const STileASCII* tileDescriptions, uint32_t tileDescriptionCount);
					::ftwlib::error_t						destroyTileMap						(STileMap& tileMapToInitialize);

#endif // TILEMAP_H
