#include "physics.h"

		::ftwlib::error_t							game::SRigidBodyEngine::CalcNextPositions		()																{
	for(uint32_t iBody = 0, bodyCount = (uint32_t)Unused.size(); iBody < bodyCount; ++iBody) {
		if(Unused[iBody])
			continue;
		const ::game::SRigidBody							& bodyCurrent									= RigidBody		[iBody];
		::game::SRigidBody									& bodyNext										= RigidBodyNext	[iBody]		= bodyCurrent;

		::game::SVector2									dirVector										= ::game::SVector2{1, 0}.Rotate( bodyCurrent.Direction );
		bodyNext.Position.Deltas						+= dirVector * bodyCurrent.Speed;
		bodyNext.Position.RefreshPosFromDeltas();
	}
	return 0;
}

		::ftwlib::error_t							game::SRigidBodyEngine::AddRigidBody			(const SRigidBody& rigidBodyData)								{
	uint32_t												bodyCount										= (uint32_t)Unused.size();
	for(uint32_t iBody = 0; iBody < bodyCount; ++iBody) {
		if(false == Unused[iBody])
			continue;
		Unused		[iBody]									= false;
		RigidBody	[iBody]									= rigidBodyData;
		return iBody;
	}
	try {
		Unused		.push_back(false		);
		RigidBody	.push_back(rigidBodyData);
	}
	catch(...) {
		return -1;
	}
	return bodyCount;
}

// Increase cell units and decrease deltas until the deltas are between 0 and 0.9999999999999999999999
		void										game::SEntityCoord2::RefreshPosFromDeltas		()																{
	::game::SVector2														& charDeltas					= Deltas; // get pointer to deltas
	::game::STileCoord2														& charTile						= Tile; // get pointer to deltas

	// Get the integer part of the deltas.
	int32_t																	deltaX							= (int)charDeltas.x;
	int32_t																	deltaY							= (int)charDeltas.y;

	// Add the integer part of the deltas to the tile coordinate.
	charTile	.x														+= deltaX;
	charTile	.y														+= deltaY;

	// Remove the integer part of the deltas from the deltas themselves.
	charDeltas	.x														-= deltaX;
	charDeltas	.y														-= deltaY;

	// Fix X coordinate if X is between 0 and -0.9999999999999999999
	if(charDeltas.x < 0) {
		charTile	.x														-= 1;
		charDeltas	.x														+= 1;
	}
	
	// Fix Y coordinate if Y is between 0 and -0.9999999999999999999
	if(charDeltas.y < 0) {
		charTile	.y														-= 1;
		charDeltas	.y														+= 1;
	}
}
