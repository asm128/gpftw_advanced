#include "ftw_coord.h"	

#ifndef GAME_COORD_H
#define GAME_COORD_H

namespace game
{
#pragma pack(push, 1)
	typedef	::ftwlib::SCoord2<float>										SVector2	;
	typedef	::ftwlib::SCoord2<int32_t>										STileCoord2	;

	struct SCellCoord2 {
				STileCoord2														Tile							= {};	// Coordinates in tile map
				SVector2														Deltas							= {};	// The position inside the tile	as a range between 0.0 and 0.99999998 or something like that, but in general it's thought as between 0.0 and 1.0.

				void															RefreshPosFromDeltas			();
	};
#pragma pack(pop)
}

#endif // GAME_COORD_H