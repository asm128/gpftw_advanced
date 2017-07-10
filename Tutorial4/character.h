#include "physics.h"

#ifndef MOB_H_9238740923874092387423
#define MOB_H_9238740923874092387423

namespace game
{
#pragma pack(push, 1)
	struct SDirection {	
			uint8_t											Front								: 1;
			uint8_t											Right								: 1;
			uint8_t											Back								: 1;
			uint8_t											Left								: 1;
			uint8_t											Up									: 1;
			uint8_t											Down								: 1;
			uint8_t											Padding								: 2;
		};

	// These enumerations should be kept small and resolve the complexity of each somewhere else. 
	enum ACTION_TYPE : uint16_t
		{	ACTION_TYPE_NONE								= 0
		,	ACTION_TYPE_WALK	
		,	ACTION_TYPE_TURN
		,	ACTION_TYPE_STRAFE	
		,	ACTION_TYPE_JUMP	
		,	ACTION_TYPE_CROUCH	
		,	ACTION_TYPE_SNEAK	
		,	ACTION_TYPE_ATTACK	
		,	ACTION_TYPE_ACTIVATE
		,	ACTION_TYPE_USE		
		,	ACTION_TYPE_THROW	
		,	ACTION_TYPE_SLOT	
		,	ACTION_TYPE_COUNT	
		,	ACTION_TYPE_INVALID								= (0xFFFF & ~0)
		};

	struct SCharacterPoints {
				int32_t										HP									; // Health
				int32_t										MP									; // Mana
				int32_t										XP									; // Experience/score
				int32_t										DP									; // Damage
	};	
		
	struct SCharacter {	// holds character data	
				SCharacterPoints							PointsMax							; // The maximum game points that can be set to a particular character
				SCharacterPoints							PointsCurrent						; // The current value of the character points, updated on game events
				double										Speed								; // Cells/second
				double										SpeedMax							; // Cells/second
				double										DirectionInRadians					; // Rotation angle in radians
				int32_t										Particle							; // For storing position and physics information
				uint32_t									ActionActive						; // Bitfield storing a boolean for each action. This works as long as ACTION_TYPE enumerated values are less than 32 
				SDirection									ActionDirection	[ACTION_TYPE_COUNT]	; // The direction for a given action.
	};
#pragma pack(pop)
}

#endif // MOB_H_9238740923874092387423
