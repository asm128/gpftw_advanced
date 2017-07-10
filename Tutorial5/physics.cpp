#include "physics.h"

// --------------------------------------------------------------------
static	void														integratePosition
	(	const ::game::SPivot2		& pivotCurrent
	,	const ::game::SForce2		& nextForces
	,	const double				fElapsedTime
	,	const double				fElapsedTimeHalfSquared
	,	::game::SPivot2				& pivotNext
	)
{
	pivotNext															= pivotCurrent;
	pivotNext.Position.Deltas											+= nextForces.Velocity * fElapsedTime;
	pivotNext.Position.Deltas											+= nextForces.Velocity * fElapsedTimeHalfSquared;
	pivotNext.Orientation												+= nextForces.Rotation * fElapsedTime;	// Update angular position.
	pivotNext.Position.RefreshPosFromDeltas();
}
// --------------------------------------------------------------------
static	void														integrateForce
	(	const ::game::SForce2		& forceCurrent
	,	const ::game::SMass2		& massCurrent
	,	const double				fElapsedTime
	,	::game::SVector2			& accumulatedForce
	,	double						& accumulatedTorque
	,	::game::SForce2				& forceNext
	)
{
	forceNext															= forceCurrent;
	forceNext.Acceleration												+= accumulatedForce * massCurrent.InverseMass;			// Calculate linear acceleration from force inputs.
	forceNext.Velocity													+= forceNext.Acceleration * fElapsedTime;				// Adjust velocities Update linear velocity from both acceleration and impulse.
	
	//double																	angularAcceleration									inverseInertiaTensorWorld.Transform(accumulatedTorque);	// Calculate angular acceleration from torque inputs.
	//nextForce.Rotation													+= angularAcceleration * fElapsedTime;				// Update angular velocity from both acceleration and impulse.

	forceNext.Velocity													*= ::pow(massCurrent.LinearDamping	, fElapsedTime);	// Impose drag.
	forceNext.Rotation													*= ::pow(massCurrent.AngularDamping	, fElapsedTime);

	accumulatedTorque													= 0;	// Clear accumulators.
	accumulatedForce													= {};
}

// --------------------------------------------------------------------
		::ftwlib::error_t											game::SRigidBodyEngine::IntegrateForces				(double fElapsedTime)											{
	for(uint32_t iBody = 0, bodyCount = (uint32_t)RigidBodyState.size(); iBody < bodyCount; ++iBody)	
		if(RigidBodyState[iBody].RequiresProcessing()) {
			::integrateForce(RigidBody[iBody].Forces, RigidBody[iBody].Mass, fElapsedTime, AccumulatedForce[iBody], AccumulatedTorque[iBody], RigidBodyNext[iBody].Forces);
		}
	return 0;
}
// --------------------------------------------------------------------
		::ftwlib::error_t											game::SRigidBodyEngine::IntegratePositions			(double fElapsedTime, double fElapsedTimeHalfSquared)			{
	for(uint32_t iBody = 0, bodyCount = (uint32_t)RigidBodyState.size(); iBody < bodyCount; ++iBody)	
		if(RigidBodyState[iBody].RequiresProcessing()) {
			::integratePosition(RigidBody[iBody].Pivot, RigidBodyNext[iBody].Forces, fElapsedTime, fElapsedTimeHalfSquared, RigidBodyNext[iBody].Pivot);
			//RigidBodyState[iBody].OutdatedTransform							= 
			//RigidBodyState[iBody].OutdatedInverseInertiaTensorWorld			= true;	// Normalize the orientation, and update the matrices with the new position and orientation.
		}
	return 0;
}
// --------------------------------------------------------------------
		::ftwlib::error_t											game::SRigidBodyEngine::AddRigidBody				(const SRigidBody2& rigidBodyData)								{
	const uint32_t															bodyCount											= (uint32_t)RigidBodyState.size();
	static constexpr	const ::game::SRigidBodyState						initialBodyState									= {false, true};

	for(uint32_t iBody = 0; iBody < bodyCount; ++iBody) {
		if(false == RigidBodyState[iBody].Unused)
			continue;
		RigidBodyState		[iBody]												= initialBodyState;
		RigidBody			[iBody]												= 
		RigidBodyNext		[iBody]												= rigidBodyData;
		AccumulatedForce	[iBody]												= {};
		AccumulatedTorque	[iBody]												= 0;
		return iBody;
	}

	try {
		RigidBodyState		.push_back(initialBodyState);
		RigidBody			.push_back(rigidBodyData);
		RigidBodyNext		.push_back(rigidBodyData);
		AccumulatedForce	.push_back({});
		AccumulatedTorque	.push_back(0);
	}
	catch(...) {
		AccumulatedTorque	.resize(bodyCount);
		AccumulatedForce	.resize(bodyCount);
		RigidBodyNext		.resize(bodyCount);
		RigidBody			.resize(bodyCount);
		RigidBodyState		.resize(bodyCount);
		return -1;
	}
	return bodyCount;
}

// Increase cell units and decrease deltas until the deltas are between 0 and 0.9999999999999999999999
		void														game::SCellCoord2::RefreshPosFromDeltas				()															{
	::game::SVector2														& charDeltas										= Deltas;	// get pointer to deltas
	::game::STileCoord2														& charTile											= Tile;		// get pointer to deltas

	int32_t																	deltaX												= (int32_t)charDeltas.x;	// Get the integer part of the deltas.
	int32_t																	deltaY												= (int32_t)charDeltas.y;

	charTile	.x														+= deltaX;	// Add the integer part of the deltas to the tile coordinate.
	charTile	.y														+= deltaY;

	charDeltas	.x														-= deltaX;	// Remove the integer part of the deltas from the deltas themselves.
	charDeltas	.y														-= deltaY;

	if(charDeltas.x < 0) {	// Fix X coordinate if X is between 0 and -0.9999999999999999999
		charTile	.x														-= 1;
		charDeltas	.x														+= 1;
	}
	if(charDeltas.y < 0) {	// Fix Y coordinate if Y is between 0 and -0.9999999999999999999
		charTile	.y														-= 1;
		charDeltas	.y														+= 1;
	}
}
