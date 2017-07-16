#include "game.h"				

#include <Windows.h>	// for GetAsyncKeyState()

			::ftwlib::error_t																game::addShip													(::game::SGame& gameInstance, ::game::SHIP_TYPE type)										{
	::game::SShip																					newInstance; 
	newInstance.ParticleIndex																	= gameInstance.ParticleEngine.AddParticle(gameInstance.DefinitionsParticleShip[type]);	
	gameInstance.ParticleEngine.Particle[newInstance.ParticleIndex].Position					= {10, 10};
	newInstance.ShipDescription																	= type; 
	newInstance.PointsCurrent																	= gameInstance.DefinitionsShip[type].PointsMax;
	uint32_t																						newShipIndex													= (uint32_t)gameInstance.Ships.size();
	gameInstance.Ships.push_back(newInstance); 
	return newShipIndex;
}
			::ftwlib::error_t																game::addShot													(::game::SGame& gameInstance, ::game::SHOT_TYPE type, int32_t shipIndex)					{ 
	::game::SShot																					newInstance; 
	newInstance.ParticleIndex																	= gameInstance.ParticleEngine.AddParticle(gameInstance.DefinitionsParticleShot[type]);	
	newInstance.ShotDescription																	= type; 
	newInstance.ShipIndex																		= shipIndex;
	newInstance.RoundsCurrent																	= gameInstance.DefinitionsShot[type].RoundsMax;
	uint32_t																						newShotIndex													= (uint32_t)gameInstance.Shots.size();
	gameInstance.Shots.push_back(newInstance); 
	return newShotIndex;
}
			::ftwlib::error_t																updateShips														(::game::SGame& gameInstance)																{
	::std::vector<::game::SShip>																	& shipInstances													= gameInstance.Ships;
	::game::SParticle2Engine<float>																	& particleEngine												= gameInstance.ParticleEngine;
	const ::ftwlib::SCoord2<int32_t>																sizeEffective													= gameInstance.CombatAreaSizeEffective.Cast<int32_t>();
	{
		int32_t																							particleIndex													= shipInstances[0].ParticleIndex;
		::game::SParticle2<float>																		& particleNext													= particleEngine.ParticleNext[particleIndex];
		if( particleNext.Position.x < 0 || particleNext.Position.x >= sizeEffective.x ) particleNext.Forces.Velocity.x = 0;
		if( particleNext.Position.y < 0 || particleNext.Position.y >= sizeEffective.y ) particleNext.Forces.Velocity.y = 0;

		particleNext.Position.x																		= ::ftwlib::min(::ftwlib::max(particleNext.Position.x, 0.0f), sizeEffective.x - 1.0f);
		particleNext.Position.y																		= ::ftwlib::min(::ftwlib::max(particleNext.Position.y, 0.0f), sizeEffective.y - 1.0f);
		particleEngine.Particle[particleIndex]														= particleNext;
	}

	for(uint32_t iShip = 1; iShip < (uint32_t)shipInstances.size(); ++iShip) {	// 
		int32_t																							particleIndexShip													= shipInstances[iShip].ParticleIndex;
		::game::SParticle2<float>																		& particleNextShip													= particleEngine.ParticleNext[particleIndexShip];
		bool																							updateParticle														= true;
		if( particleNextShip.Position.x < 0 || particleNextShip.Position.x >= sizeEffective.x
		 || particleNextShip.Position.y < 0 || particleNextShip.Position.y >= sizeEffective.y
		 ) { // Remove the particle instance and related information.
			particleEngine.ParticleState[particleIndexShip].Unused											= true;
			shipInstances.erase(shipInstances.begin() + iShip);
			--iShip;
		}
		else { 
			::std::vector<::game::SShot>																	& shotInstances													= gameInstance.Shots;
			for(uint32_t iShot = 0; iShot < (uint32_t)shotInstances.size(); ++iShot) {	// 
				int32_t																							particleIndexShot												= shotInstances[iShot].ParticleIndex;
				::game::SParticle2<float>																		& particleShot													= particleEngine.Particle[particleIndexShot];
				::game::SParticle2<float>																		& particleCurrent												= particleEngine.Particle[particleIndexShip];
				::ftwlib::SCircle2D<float>																		areaShip														= {.5, particleCurrent.Position};
				::ftwlib::SCircle2D<float>																		areaShot														= {.5, particleShot.Position};
				if(iShip == (uint32_t)shotInstances[iShot].ShipIndex) 
					continue;
				if( (particleCurrent.Position - particleShot.Position).Length() < (areaShip.Radius + areaShot.Radius)) { // Handle collision - Remove the particle instance and related information.
					shipInstances[iShip].PointsCurrent.Health															-=	gameInstance.DefinitionsShot[shotInstances[iShot].ShotDescription].Damage;
					particleEngine.ParticleState[particleIndexShot].Unused												= true;
					shotInstances.erase(shotInstances.begin() + iShot);
					--iShot;
				}
			}
			if(updateParticle)
				particleEngine.Particle[particleIndexShip]													= particleEngine.ParticleNext[particleIndexShip];
		}
	}

	for(uint32_t iShip = 1; iShip < (uint32_t)shipInstances.size(); ++iShip) {	// Remove dead ships
		int32_t																							particleIndexShip													= shipInstances[iShip].ParticleIndex;
		if(shipInstances[iShip].PointsCurrent.Health <= 0) {
			particleEngine.ParticleState[particleIndexShip].Unused												= true;
			shipInstances.erase(shipInstances.begin() + iShip);
			--iShip;
		}
	}

	return 0;
}
			::ftwlib::error_t																updateShots														(::game::SGame& gameInstance)																{ 
	//::std::vector<::game::SShip>																	& shipInstances													= gameInstance.Ships;
	::std::vector<::game::SShot>																	& shotInstances													= gameInstance.Shots;
	::game::SParticle2Engine<float>																	& particleEngine												= gameInstance.ParticleEngine;
	const ::ftwlib::SCoord2<int32_t>																sizeEffective													= gameInstance.CombatAreaSizeEffective.Cast<int32_t>();

	for(uint32_t iShot = 0; iShot < (uint32_t)shotInstances.size(); ++iShot) {	// 
		int32_t																							particleIndex													= shotInstances[iShot].ParticleIndex;
		::game::SParticle2<float>																		& particleNext													= particleEngine.ParticleNext[particleIndex];
		if( particleNext.Position.x < 0 || particleNext.Position.x >= sizeEffective.x
		 || particleNext.Position.y < 0 || particleNext.Position.y >= sizeEffective.y
		 ) { // Remove the particle instance and related information.
			particleEngine.ParticleState[particleIndex].Unused												= true;
			shotInstances.erase(shotInstances.begin() + iShot);
			--iShot;
		}
		else 
			particleEngine.Particle[particleIndex]														= particleEngine.ParticleNext[particleIndex];
	}
	return 0;
}
			::ftwlib::error_t																updateSpawners														(::game::SGame& gameInstance)															{ 
	::ftwlib::SFrameInfo																			& frameInfo														= gameInstance.FrameInfo;																	
	::game::SParticle2Engine<float>																	& particleEngine												= gameInstance.ParticleEngine;
	::std::vector<::game::SShip>																	& shipInstances													= gameInstance.Ships;
	for(uint32_t iSpawner = 0, spawnerCount = (uint32_t)gameInstance.Spawners.size(); iSpawner < spawnerCount; ++ iSpawner) {
		::game::SSpawner																				& spawner														= gameInstance.Spawners[iSpawner];
		for(uint32_t iSpawnRecord = 0; iSpawnRecord < spawner.Records.size(); ++iSpawnRecord) {
			if(frameInfo.Seconds.Total >= spawner.Records[iSpawnRecord].TimeSinceGameStarted) {
				int32_t																							shipIndex														= ::game::addShip(gameInstance, spawner.Records[iSpawnRecord].ShipTypeToSpawn);
				particleEngine.Particle[shipInstances[shipIndex].ParticleIndex].Position					= spawner.Position.Cast<float>();
				::ftwlib::SCoord2<float>																		distanceFromPlayer												= particleEngine.Particle[shipInstances[0].ParticleIndex].Position - spawner.Position.Cast<float>();
				if(distanceFromPlayer.LengthSquared())
					particleEngine.Particle[shipInstances[shipIndex].ParticleIndex].Forces.Velocity				= distanceFromPlayer / 2LL;
				spawner.Records.erase(spawner.Records.begin() + iSpawnRecord);
			}
		}
	}
	return 0;
}
			::ftwlib::error_t																game::update													(SGame& gameInstance, uint64_t lastTimeMicroseconds)										{ 
	::ftwlib::SFrameInfo																			& frameInfo														= gameInstance.FrameInfo;																	
	frameInfo.Frame(lastTimeMicroseconds);
	const float																						lastFrameSeconds												= (float)frameInfo.Seconds.LastFrame;

	::std::vector<::game::SShip>																	& shipInstances													= gameInstance.Ships;
	::std::vector<::game::SShot>																	& shotInstances													= gameInstance.Shots;
	::game::SParticle2Engine<float>																	& particleEngine												= gameInstance.ParticleEngine;

	// SWitch weapon
		 if(::GetAsyncKeyState('1') || ::GetAsyncKeyState(VK_NUMPAD1)) shipInstances[0].SelectedShot = ::game::SHOT_TYPE_ROCK		;
	else if(::GetAsyncKeyState('2') || ::GetAsyncKeyState(VK_NUMPAD2)) shipInstances[0].SelectedShot = ::game::SHOT_TYPE_ARROW		;
	else if(::GetAsyncKeyState('3') || ::GetAsyncKeyState(VK_NUMPAD3)) shipInstances[0].SelectedShot = ::game::SHOT_TYPE_FIREBALL	;
	else if(::GetAsyncKeyState('4') || ::GetAsyncKeyState(VK_NUMPAD4)) shipInstances[0].SelectedShot = ::game::SHOT_TYPE_LASER		;
	else if(::GetAsyncKeyState('5') || ::GetAsyncKeyState(VK_NUMPAD5)) shipInstances[0].SelectedShot = ::game::SHOT_TYPE_POISON		;
	else if(::GetAsyncKeyState('6') || ::GetAsyncKeyState(VK_NUMPAD6)) shipInstances[0].SelectedShot = ::game::SHOT_TYPE_PLASMA		;
	else if(::GetAsyncKeyState('7') || ::GetAsyncKeyState(VK_NUMPAD7)) shipInstances[0].SelectedShot = ::game::SHOT_TYPE_BOMB		;

	if(::GetAsyncKeyState(VK_SPACE)) { // Shoot
		int32_t																							shotIndex														= ::game::addShot(gameInstance, shipInstances[0].SelectedShot, 0); 
		particleEngine.Particle[shotInstances[shotIndex].ParticleIndex].Position					= particleEngine.Particle[shipInstances[0].ParticleIndex].Position; 
		particleEngine.Particle[shotInstances[shotIndex].ParticleIndex].Forces.Velocity.x			= (float)gameInstance.DefinitionsShot[shotInstances[shotIndex].ShotDescription].Speed; 
	}

	// Move
	bool																							bUp																= ::GetAsyncKeyState('W');
	bool																							bRight															= ::GetAsyncKeyState('D');
	bool																							bDown															= ::GetAsyncKeyState('S');
	bool																							bLeft															= ::GetAsyncKeyState('A');

	::game::SParticle2<float>																		& playerParticle												= particleEngine.Particle[shipInstances[0].ParticleIndex];
	if( bUp		) { playerParticle.Forces.Acceleration = {}; playerParticle.Forces.Velocity.y													-= lastFrameSeconds * 50; }
	if( bRight	) { playerParticle.Forces.Acceleration = {}; playerParticle.Forces.Velocity.x													+= lastFrameSeconds * 50; }
	if( bDown	) { playerParticle.Forces.Acceleration = {}; playerParticle.Forces.Velocity.y													+= lastFrameSeconds * 50; }
	if( bLeft	) { playerParticle.Forces.Acceleration = {}; playerParticle.Forces.Velocity.x													-= lastFrameSeconds * 50; }
	if(!bUp && !bRight && !bDown && !bLeft) {

	}

	// -- Apply background force.
	for(uint32_t iParticle = 0, particleCount = (uint32_t)particleEngine.Particle.size(); iParticle < particleCount; ++iParticle)
		particleEngine.Particle[iParticle].Forces.AccumulatedForce.x								-= lastFrameSeconds;
	particleEngine.Integrate(lastFrameSeconds, frameInfo.Seconds.LastFrameHalfSquared);

	::updateShips	(gameInstance);
	::updateShots	(gameInstance);
	::updateSpawners(gameInstance);
	return 0; 
}
