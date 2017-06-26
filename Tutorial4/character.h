#include "physics.h"

#ifndef MOB_H_9238740923874092387423
#define MOB_H_9238740923874092387423

namespace game
{
	enum ACTION_TYPE	// Define a type to represent the current action of a character.
		{		ACTION_STAND		// means the character has to apply movement during update
		,		ACTION_WALK			// means the character is stopped and shouldn't walk on frame update
		};

	struct SCharacterPoints {
				int32_t										HP				;	// Health
				int32_t										MP				;	// Mana
				int32_t										XP				;	// Experience/score
				int32_t										DP				;	// Damage
	};

	struct SCharacter {	// holds character data
				SCharacterPoints							PointsMax		;	// The maximum game points that can be set to a particular character
				SCharacterPoints							PointsCurrent	;	// The current value of the character points, updated on game events
				int32_t										RigidBody		;	// For storing position and physics information
				ACTION_TYPE									Action			;	// The current action the character is performing
	};
}

#endif // MOB_H_9238740923874092387423