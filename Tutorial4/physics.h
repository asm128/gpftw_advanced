#include "ftw_coord.h"
#include "ftw_error.h"

#include <vector>

#ifndef RIGIDBODY_H_2398720938472398
#define RIGIDBODY_H_2398720938472398

namespace game 
{
#pragma pack(push, 1)
	typedef	::ftwlib::SCoord2<float>					SVector2	;
	typedef	::ftwlib::SCoord2<int32_t>					STileCoord2	;

	struct SCellCoord2 {
				STileCoord2									Tile							= {};	// Coordinates in tile map
				SVector2									Deltas							= {};	// The position inside the tile	as a range between 0.0 and 0.99999998 or something like that, but in general it's thought as between 0.0 and 1.0.

				void										RefreshPosFromDeltas			();
	};

	struct SParticle {
				::game::SCellCoord2						Position						= {};	// Coordinates in tile map
				::game::SVector2							Velocity						= {};	// A vector representing the speed in a given direction 
	};

	struct SParticleState {
				bool										Unused							: 1;
				bool										Active							: 1;
	};
#pragma pack(pop)

	struct SParticleEngine {
				::std::vector<SParticle>					Particle						= {};
				::std::vector<SParticle>					ParticleNext					= {};
				::std::vector<SParticleState>				ParticleState					= {};
	// --- Methods
				::ftwlib::error_t							CalcNextPositions				(double fElapsedTime);
				::ftwlib::error_t							AddParticle						(const SParticle& rigidBodyData);
	};
}

#endif // RIGIDBODY_H_2398720938472398
