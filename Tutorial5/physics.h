#include "ftw_coord.h"
#include "ftw_error.h"

#include <vector>

#ifndef RIGIDBODY_H_2398720938472398
#define RIGIDBODY_H_2398720938472398

namespace game 
{
#pragma pack(push, 1)
	typedef	::ftwlib::SCoord2<float>								SVector2	;
	typedef	::ftwlib::SCoord2<int32_t>								STileCoord2	;

	struct SBodyCoord2 {
				::game::STileCoord2										Tile									= {};	// Coordinates in tile map
				::game::SVector2										Deltas									= {};	// The position inside the tile	as a range between 0.0 and 0.99999998 or something like that, but in general it's thought as between 0.0 and 1.0.

				void													RefreshPosFromDeltas					()						noexcept;
	};

	struct SPivot2 {
				SBodyCoord2												Position								= {};
				double													Orientation								= {};
	};

	struct SForce2 {
				SVector2												Velocity								= {};
				SVector2												Acceleration							= {};
				double													Rotation								= {};
	};

	struct SMass2 {
				double													LinearDamping							= 0.999;
				double													AngularDamping							= 0.999;
				double													InverseMass								= 0.00000000000001;
	};

	struct SRigidBody {
				::game::SMass2											Mass									= {};	// 
				::game::SForce2											Forces									= {};	// A vector representing the speed in a given direction 
				::game::SPivot2											Pivot									= {};	// Coordinates in tile map
	};

	struct SRigidBodyState {
				bool													Unused									: 1;
				bool													Active									: 1;
	};
#pragma pack(pop)

	struct SRigidBodyEngine {
				::std::vector<SVector2>									AccumulatedForce						= {};
				::std::vector<double>									AccumulatedTorque						= {};
				::std::vector<SRigidBody>								RigidBody								= {};
				::std::vector<SRigidBody>								RigidBodyNext							= {};
				::std::vector<SRigidBodyState>							RigidBodyState							= {};
	// --- Methods
				::ftwlib::error_t										IntegrateForces							(double fElapsedTime);
				::ftwlib::error_t										IntegratePositions						(double fElapsedTime, double fHalfSquaredTime);
				::ftwlib::error_t										CalcNextPositions						(double fElapsedTime, double rHalfSquaredTime);
				::ftwlib::error_t										AddRigidBody							(const SRigidBody& rigidBodyData);
	};
}

#endif // RIGIDBODY_H_2398720938472398
