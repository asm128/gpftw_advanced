#include "physics_particle.h"

#include "ftw_frameinfo.h"
#include "ftw_ascii_screen.h"	// For ::ftwlib::SScreenASCII

#include <vector>				// For ::std::vector<>

#ifndef GAME_H
#define GAME_H

namespace game 
{
	enum SHOT_TYPE 
		{	SHOT_TYPE_DEFAULT  
		,	SHOT_TYPE_ROCK
		,	SHOT_TYPE_ARROW
		,	SHOT_TYPE_FIREBALL
		,	SHOT_TYPE_LASER
		,	SHOT_TYPE_POISON
		,	SHOT_TYPE_PLASMA
		,	SHOT_TYPE_BOMB
		,	SHOT_TYPE_COUNT
		};

	enum SHIP_TYPE 
		{	SHIP_TYPE_DEFAULT  
		,	SHIP_TYPE_FASTER
		,	SHIP_TYPE_STRONGER
		,	SHIP_TYPE_BETTER
		,	SHIP_TYPE_NICER
		,	SHIP_TYPE_GOOD_LOOKING
		,	SHIP_TYPE_COUNT
		};

	struct SShotDescription {
				int32_t															Damage												;
				int32_t															RoundsMax											;
				double															Speed												;
				uint8_t															Image												;
	};

	struct SShot {
				int32_t															ShotDescription										;
				int32_t															ParticleIndex										;
				int32_t															RoundsCurrent										;
	};

	struct SShipPoints {
				int32_t															Health												;
				int32_t															Shield												;
	};

	struct SShipDescription {
				SShipPoints														PointsMax											;
				uint8_t															Image												;
	};

	struct SShip {
				int32_t															ShipDescription										;
				int32_t															SelectedShot										;
				int32_t															ParticleIndex										;
				SShipPoints														PointsCurrent										;
	};

	struct SGame {
				::ftwlib::SFrameInfo											FrameInfo											= {};	// -- Stores data such as the frame number and the elapsed frame time since the previous update
				::game::SParticle2Engine<float>									ParticleEngine										= {};	// -- Physics stuff

				// -- Game object instances
				::std::vector<SShip>											Ships												= {};
				::std::vector<SShot>											Shots												= {};

				// -- Screen information
				::ftwlib::SCoord2<uint32_t>										VisibleSize											= {96, 50};
				::ftwlib::SCoord2<uint32_t>										MarginSize											= {10, 5};

				// -- Description tables - Game objects
				::game::SShipDescription										DefinitionsShip			[::game::SHIP_TYPE_COUNT]	= {};
				::game::SShotDescription										DefinitionsShot			[::game::SHOT_TYPE_COUNT]	= {};

				// -- Description tables - Physics
				::game::SParticle2<float>										DefinitionsParticleShip	[::game::SHIP_TYPE_COUNT]	= {};
				::game::SParticle2<float>										DefinitionsParticleShot	[::game::SHOT_TYPE_COUNT]	= {};
	};
	// ------
			::ftwlib::error_t												addShip												(SGame& gameInstance, SHIP_TYPE type);
			::ftwlib::error_t												addShot												(SGame& gameInstance, SHOT_TYPE type);
	// ------
			::ftwlib::error_t												setup												(SGame& gameInstance);
			::ftwlib::error_t												update												(SGame& gameInstance, uint64_t lastTimeMicroseconds);
			::ftwlib::error_t												render												(SGame& gameInstance, ::ftwlib::SScreenASCII& screenASCII);
}

#endif // GAME_H