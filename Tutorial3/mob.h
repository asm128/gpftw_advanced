#include "ftw_coord.h"

#ifndef MOB_H_9238740923874092387423
#define MOB_H_9238740923874092387423

namespace game
{
	typedef	::ftwlib::SCoord2<float>					SVector2;
	typedef	::ftwlib::SCoord2<int32_t>					SMapCoord2;

	struct SMovingObject {
				SMapCoord2									Position								= {};	// Coordinates in tile map
				SVector2									PositionDeltas							= {};	// The position inside the tile	as a range between 0.0 and 0.99999998 or something like that, but in general it's thought as between 0.0 and 1.0.
				double										Direction								= {};	// rotation angle in radians
				double										Speed									= {};	// cells/second
	};

	enum ACTION_TYPE		// this is pretty much the same as "typedef int ACTION_TYPE;"
		{		ACTION_WALK		// means the character has to apply movement during update
		,		ACTION_STAND	// means the character is stopped and shouldn't walk on frame update
		};

	struct SCharacterPoints {
				int32_t										HP;	// health, mana and experience/score
				int32_t										MP;
				int32_t										XP;
	};

	// holds character data, inherits from SMovingObject for storing position and physics information
	struct SCharacter : public SMovingObject {
				SCharacterPoints							MaxPoints								;	// the maximum game points that can be set to a particular character
				SCharacterPoints							CurrentPoints							;	// the current value of the character points, updated on game events
				ACTION_TYPE									Action									;
	};

	// holds character data, inherits from SMovingObject for storing position and physics information
	struct SShot : public SMovingObject {
				int32_t										Damage									= 0;	// Value that will be substracted from the character's health points 
	};
}

#endif // MOB_H_9238740923874092387423