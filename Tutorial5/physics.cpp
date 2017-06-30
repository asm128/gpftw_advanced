#include "physics.h"

		::ftwlib::error_t											game::SRigidBodyEngine::CalcNextPositions			(double fElapsedTime)																{
	double																	fElapsedTimeHalfSquared								= fElapsedTime * fElapsedTime * 0.5;
	for(uint32_t iBody = 0, bodyCount = (uint32_t)RigidBodyState.size(); iBody < bodyCount; ++iBody) {
		if(RigidBodyState[iBody].Unused || false == RigidBodyState[iBody].Active)
			continue;
		const ::game::SRigidBody												& bodyCurrent										= RigidBody		[iBody];
		::game::SRigidBody														& bodyNext											= RigidBodyNext	[iBody]		= bodyCurrent;

		bodyNext.Position.Deltas											+= bodyCurrent.Velocity * fElapsedTime;
		bodyNext.Position.Deltas											+= bodyCurrent.Velocity * fElapsedTimeHalfSquared;

		bodyNext.Position.RefreshPosFromDeltas();
	}
	return 0;
}

		::ftwlib::error_t											game::SRigidBodyEngine::AddRigidBody			(const SRigidBody& rigidBodyData)								{
	const uint32_t															bodyCount										= (uint32_t)RigidBodyState.size();
	static constexpr const	::game::SRigidBodyState							bodyState										= {false, true};

	for(uint32_t iBody = 0; iBody < bodyCount; ++iBody) {
		if(false == RigidBodyState[iBody].Unused)
			continue;
		RigidBodyState	[iBody]												= bodyState;
		RigidBody		[iBody]												= 
		RigidBodyNext	[iBody]												= rigidBodyData;
		return iBody;
	}
	try {
		RigidBodyState	.push_back(bodyState);
	}
	catch(...) {
		return -1;
	}
	try {
		RigidBody		.push_back(rigidBodyData);
		RigidBodyNext	.push_back(rigidBodyData);
	}
	catch(...) {
		RigidBodyState	.pop_back();
		return -1;
	}
	return bodyCount;
}

// Increase cell units and decrease deltas until the deltas are between 0 and 0.9999999999999999999999
		void														game::SEntityCoord2::RefreshPosFromDeltas		()																{
	::game::SVector2														& charDeltas									= Deltas; // get pointer to deltas
	::game::STileCoord2														& charTile										= Tile; // get pointer to deltas

	// Get the integer part of the deltas.
	int32_t																	deltaX											= (int)charDeltas.x;
	int32_t																	deltaY											= (int)charDeltas.y;

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
