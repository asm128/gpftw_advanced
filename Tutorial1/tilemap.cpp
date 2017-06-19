#include "tilemap.h"
#include "gpftw_ascii_color.h"


::gpftw::error_t								initializeTileMap					(STileMap& tileMapToInitialize, uint32_t width, uint32_t height, const STileASCII* tileDescriptions, uint32_t tileDescriptionCount)		{ 
	tileMapToInitialize.ASCIIMap					= {(uint32_t*)::malloc(sizeof(uint32_t)*width*height), width, height}; 
	tileMapToInitialize.TileDescriptionTable		= {&tileDescriptions[0], tileDescriptionCount}; 
	return 0;
}

::gpftw::error_t								destroyTileMap						(STileMap& tileMapToInitialize)											{
	if(tileMapToInitialize.ASCIIMap.size())	
		::free(tileMapToInitialize.ASCIIMap.begin()); 
	
	tileMapToInitialize.ASCIIMap					= {}; 
	return 0;
}
