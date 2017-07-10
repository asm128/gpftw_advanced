#include "game_coord.h"
#include "ftw_error.h"	// for ::ftwlib::error_t

#include <vector>

#ifndef PHSYICS_H
#define PHSYICS_H

namespace game 
{
#pragma pack(push, 1)
	struct SPivot2 {
				SCellCoord2														Position								= {};
				double															Orientation								= {};
	};

	struct SForce2 {
				SVector2														Velocity								= {};
				SVector2														Acceleration							= {};
				double															Rotation								= {};
	};

	struct SMass2 {
				double															LinearDamping							= 0.999;
				double															AngularDamping							= 0.999;
				double															InverseMass								= 0.00000000000001;

		inline	void															SetMass									(const double mass)											{ InverseMass = 1.0 / mass;									}
		inline	double															GetMass									()													const	{ return (InverseMass == 0) ? DBL_MAX : 1.0 / InverseMass;	}
		inline	bool															HasFiniteMass							()													const	{ return InverseMass >= 0.0f;								}
	};

	struct SRigidBody2 {
				::game::SMass2													Mass									= {};	// 
				::game::SForce2													Forces									= {};	// A vector representing the speed in a given direction 
				::game::SPivot2													Pivot									= {};	// Coordinates in tile map
	};

	struct SRigidBodyState {
				bool															Unused									: 1;
				bool															Active									: 1;

				inline constexpr	bool										RequiresProcessing						()													const	{ return (false == Unused) && Active; }
	};
#pragma pack(pop)

	struct SRigidBodyEngine {
				::std::vector<SVector2>											AccumulatedForce						= {};
				::std::vector<double>											AccumulatedTorque						= {};
				::std::vector<SRigidBody2>										RigidBody								= {};
				::std::vector<SRigidBody2>										RigidBodyNext							= {};
				::std::vector<SRigidBodyState>									RigidBodyState							= {};
	// --- Methods
				::ftwlib::error_t												IntegrateForces							(double fTimeElapsed);
				::ftwlib::error_t												IntegratePositions						(double fTimeElapsed, double fHalfSquaredTime);
				::ftwlib::error_t												AddRigidBody							(const SRigidBody2& rigidBodyData);
	};

}

#endif // PHSYICS_H
