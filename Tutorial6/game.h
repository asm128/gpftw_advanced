#include "ftw_particle.h"

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
		,	SHOT_TYPE_BULLET
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

	enum EFFECT_TYPE 
		{	EFFECT_TYPE_DEFAULT  
		,	EFFECT_TYPE_ROCKET
		,	EFFECT_TYPE_STARS
		,	EFFECT_TYPE_COUNT
		};
	struct SSpawnerRecord {
				SHIP_TYPE														ShipTypeToSpawn												;
				double															TimeSinceGameStarted										;
	};

	struct SSpawner	{
				::ftwlib::SCoord2<int32_t>										Position													;
				::std::vector<SSpawnerRecord>									Records														;
	};

	struct SShotDescription {
				int32_t															Damage														;
				int32_t															RoundsMax													;
				double															Speed														;
				uint8_t															Image														;
				uint16_t														Color														;
	};

	struct SShot {
				SHOT_TYPE														ShotDescription												;
				int32_t															ParticleIndex												;
				int32_t															ShipIndex													;
				int32_t															RoundsCurrent												;
	};

	struct SShipPoints {
				int32_t															Health														;
				int32_t															Shield														;
	};

	struct SShipDescription {
				SShipPoints														PointsMax													;
				uint8_t															Image														;
				uint16_t														Color														;
	};

	struct SShip {
				SHIP_TYPE														ShipDescription												;
				SHOT_TYPE														SelectedShot												;
				int32_t															ParticleIndex												;
				SShipPoints														PointsCurrent												;
	};

	struct SEffectDescription {
				uint8_t															Image														;
				uint16_t														Color														;
	};

	struct SEffect {
				EFFECT_TYPE														Type														;
				int32_t															ParticleIndex												;
	};

	struct SGame {
				::ftwlib::SFrameInfo											FrameInfo													= {};	// -- Stores data such as the frame number and the elapsed frame time since the previous update
				::ftwlib::SParticle2Engine<float>									ParticleEngineGame											= {};	// -- Physics stuff
				::ftwlib::SParticle2Engine<float>									ParticleEngineEffects										= {};	// -- Physics stuff

				// -- Game object instances
				::std::vector<SShip>											Ships														= {};
				::std::vector<SShot>											Shots														= {};
				::std::vector<SSpawner>											Spawners													= {};
				::std::vector<SEffect>											Effects														= {};

				// -- Screen information
				::ftwlib::SCoord2<uint32_t>										CombatAreaSizeVisible										= {96	, 50};
				::ftwlib::SCoord2<uint32_t>										CombatAreaSizeEffective										= {100	, 60};

				// -- Description tables - Game objects
				::game::SShipDescription										DefinitionsShip				[::game::SHIP_TYPE_COUNT	]	= {};
				::game::SShotDescription										DefinitionsShot				[::game::SHOT_TYPE_COUNT	]	= {};
				::game::SEffectDescription										DefinitionsEffect			[::game::EFFECT_TYPE_COUNT	]	= {};

				// -- Description tables - Physics
				::ftwlib::SParticle2<float>										DefinitionsParticleShip		[::game::SHIP_TYPE_COUNT	]	= {};
				::ftwlib::SParticle2<float>										DefinitionsParticleShot		[::game::SHOT_TYPE_COUNT	]	= {};
				::ftwlib::SParticle2<float>										DefinitionsParticleEffect	[::game::EFFECT_TYPE_COUNT	]	= {};
	};
	// ------
			::ftwlib::error_t												addShip														(SGame& gameInstance, SHIP_TYPE type);
			::ftwlib::error_t												addShot														(SGame& gameInstance, SHOT_TYPE type, int32_t shipIndex);
			::ftwlib::error_t												addEffect													(SGame& gameInstance, EFFECT_TYPE type);
	// ------
			::ftwlib::error_t												setup														(SGame& gameInstance);
			::ftwlib::error_t												update														(SGame& gameInstance, uint64_t lastTimeMicroseconds);
			::ftwlib::error_t												render														(SGame& gameInstance, ::ftwlib::SScreenASCII& screenASCII);
}

#endif // GAME_H
