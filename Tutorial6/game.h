#include "physics_particle.h"

#include "ftw_error.h"

#include <cstdint>
#include <vector>

#ifndef FRAMEINFO_H
#define FRAMEINFO_H

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
				int32_t															Damage;
				int32_t															RoundsMax;
				double															Speed;
				uint8_t															Image;
	};

	struct SShot {
				int32_t															ShotDescription;
				int32_t															ParticleIndex;
				int32_t															RoundsCurrent;
	};

	struct SShipPoints {
				int32_t															Health;
				int32_t															Shield;
	};

	struct SShipDescription {
				SShipPoints														PointsMax;
				uint8_t															Image;
	};

	struct SShip {
				int32_t															ShipDescription;
				int32_t															SelectedShot;
				int32_t															ParticleIndex;
				SShipPoints														PointsCurrent;
	};

	struct SFrameSeconds {
				double															Total									;
				double															LastFrame								;
				double															LastFrameHalfSquared					;
		// Helper
		inline	void															UpdateFromTime							(double secondsToAdd)						noexcept	{ 
			Total																	+= LastFrame								= secondsToAdd; 
			LastFrameHalfSquared													=  secondsToAdd * secondsToAdd * 0.5;
		}
	};

	struct SFrameMicroseconds {
				uint64_t														Total									;
				uint64_t														LastFrame								;
		// Helper
		inline	void															UpdateFromTime							(uint64_t microsecondsToAdd)				noexcept	{ Total += LastFrame = microsecondsToAdd; }
	};

	struct SFrameInfo {
				uint64_t														FrameNumber								= 0;
				uint64_t														FramesPerSecond							= 0;
				SFrameMicroseconds												Microseconds							= {};
				SFrameSeconds													Seconds									= {};
		
				void															Frame									(uint64_t timeElapsedMicroseconds)						{	// Set last frame time and number.
					++FrameNumber;
					Microseconds	.UpdateFromTime(timeElapsedMicroseconds);
					Seconds			.UpdateFromTime(timeElapsedMicroseconds / 1000000.0);
				}
	};

	template<typename _tEnum>
	struct SParticleInstance {
		_tEnum																	Type								= ~0;
		int32_t																	PhysicsId							= -1;
	};

	struct SGame {
				::game::SParticle2Engine<float>									ParticleEngine						= {};
				::game::SFrameInfo												FrameInfo							= {};
				::std::vector<SParticleInstance<::game::SHIP_TYPE>>				ShipParticleInstances				= {};
				::std::vector<SParticleInstance<::game::SHOT_TYPE>>				ShotParticleInstances				= {};
				::std::vector<SShip>											Ships								= {};
				::std::vector<SShot>											Shots								= {};
	};
	// ------
			::ftwlib::error_t												addShip									(SGame& gameInstance, SHIP_TYPE type);
			::ftwlib::error_t												addShot									(SGame& gameInstance, SHOT_TYPE type);
	// ------
			::ftwlib::error_t												setup									(SGame& gameInstance);
			::ftwlib::error_t												update									(SGame& gameInstance, uint64_t lastTimeMicroseconds);
			::ftwlib::error_t												render									(SGame& gameInstance);
			::ftwlib::error_t												cleanup									(SGame& gameInstance);
}

#endif // FRAMEINFO_H