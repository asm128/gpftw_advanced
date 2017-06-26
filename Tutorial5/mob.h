#include "ftw_coord.h"

#ifndef MOB_H_9238740923874092387423
#define MOB_H_9238740923874092387423

namespace game
{
	typedef				::ftwlib::SCoord2<float>					SVector2		;
	typedef				::ftwlib::SCoord2<int32_t>					STileCoord2		;

	struct SEntityCoord {
							STileCoord2									Tile			;	// Coordinates in tile map
							SVector2									Deltas			;	// The position inside the tile	as a range between 0.0 and 0.99999998 or something like that, but in general it's thought as between 0.0 and 1.0.
	};

	struct SMovingObject {
							SEntityCoord								Position		;	
							double										Direction		;	// rotation angle in radians
							double										Speed			;	// cells/second
	};

	struct SCharacterPoints {
							int32_t										HP				;	// health
							int32_t										MP				;	// mana 
							int32_t										XP				;	// experience/score
							int32_t										DP				;	// damage
	};
}

#endif // MOB_H_9238740923874092387423