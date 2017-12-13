#include "game.h"				

#include <Windows.h>	// for GetAsyncKeyState()

			::ftwl::error_t																	game::addEffect													(::game::SGame& gameInstance, EFFECT_TYPE type)												{
	::game::SEffect																					newInstance														= {type,};
	newInstance.ParticleIndex																	= gameInstance.ParticleEngineEffects.AddParticle(gameInstance.DefinitionsParticleEffect[type]);	
	uint32_t																						newEffectIndex													= (uint32_t)gameInstance.Effects.size();
	gameInstance.Effects.push_back(newInstance); 
	return newEffectIndex;
}
			::ftwl::error_t																	game::addShip													(::game::SGame& gameInstance, ::game::SHIP_TYPE type)										{
	::game::SShip																					newInstance														= {type,}; 
	newInstance.ParticleIndex																	= gameInstance.ParticleEngineGame.AddParticle(gameInstance.DefinitionsParticleShip[type]);	
	gameInstance.ParticleEngineGame.Particle[newInstance.ParticleIndex].Position				= {10, 10};
	newInstance.PointsCurrent																	= gameInstance.DefinitionsShip[type].PointsMax;
	newInstance.Unused																			= false;
	for(uint32_t iShip = 0, shipCount = (uint32_t)gameInstance.Ships.size(); iShip < shipCount; ++iShip) {
		::game::SShip																					& instanceToRecycle												= gameInstance.Ships[iShip];
		if(instanceToRecycle.Unused) {
			instanceToRecycle																			= newInstance;
			return iShip;
		}
	}
	uint32_t																						newShipIndex													= (uint32_t)gameInstance.Ships.size();
	gameInstance.Ships.push_back(newInstance); 
	return newShipIndex;
}
			::ftwl::error_t																	game::addShot													(::game::SGame& gameInstance, ::game::SHOT_TYPE type, int32_t shipIndex)					{ 
	::game::SShot																					newInstance														= {type,}; 
	newInstance.ParticleIndex																	= gameInstance.ParticleEngineGame.AddParticle(gameInstance.DefinitionsParticleShot[type]);	
	newInstance.ShipIndex																		= shipIndex;
	//newInstance.RoundsCurrent																	= gameInstance.DefinitionsShot[type].RoundsMax;
	const uint32_t																					newShotIndex													= (uint32_t)gameInstance.Shots.size();
	gameInstance.Shots.push_back(newInstance); 
	return newShotIndex;
}

static		::ftwl::error_t																	updateShips														(::game::SGame& gameInstance)																{
	::std::vector<::game::SShip>																	& shipInstances													= gameInstance.Ships;
	::ftwl::SParticle2Engine<float>																	& particleEngine												= gameInstance.ParticleEngineGame;
	const ::ftwl::SCoord2<int32_t>																	sizeEffective													= gameInstance.CombatAreaSizeEffective.Cast<int32_t>();
	{
		const int32_t																					particleIndex													= shipInstances[0].ParticleIndex;
		::ftwl::SParticle2<float>																		& particleNext													= particleEngine.ParticleNext[particleIndex];
		if( particleNext.Position.x < 0 || particleNext.Position.x >= sizeEffective.x ) particleNext.Forces.Velocity.x = 0;
		if( particleNext.Position.y < 0 || particleNext.Position.y >= sizeEffective.y ) particleNext.Forces.Velocity.y = 0;

		particleNext.Position.x																		= ::ftwl::min(::ftwl::max(particleNext.Position.x, 0.0f), sizeEffective.x - 1.0f);
		particleNext.Position.y																		= ::ftwl::min(::ftwl::max(particleNext.Position.y, 0.0f), sizeEffective.y - 1.0f);
		particleEngine.Particle[particleIndex]														= particleNext;
	}

	for(uint32_t iShip = 1; iShip < (uint32_t)shipInstances.size(); ++iShip) {	// Remove ships that are outside the display area.
		::game::SShip																					& shipToTest													= shipInstances[iShip];
		if(shipToTest.Unused)
			continue;
		const int32_t																					particleIndexShip												= shipToTest.ParticleIndex;
		const ::ftwl::SParticle2<float>																	& particleNextShip												= particleEngine.ParticleNext[particleIndexShip];
		if( particleNextShip.Position.x < 0 || particleNextShip.Position.x >= sizeEffective.x
		 || particleNextShip.Position.y < 0 || particleNextShip.Position.y >= sizeEffective.y
		 ) { // Remove the particle instance and related information.
			particleEngine.ParticleState[particleIndexShip].Unused										= true;
			shipToTest.Unused																			= true;
		}
	}

	::std::vector<::game::SShot>																	& shotInstances													= gameInstance.Shots;
	for(uint32_t iShip = 0; iShip < (uint32_t)shipInstances.size(); ++iShip) {	// Update shots and damage ships.
		::game::SShip																					& shipToTest													= shipInstances[iShip];
		if(shipToTest.Unused)
			continue;
		const int32_t																					particleIndexShip												= shipToTest.ParticleIndex;
		for(uint32_t iShot = 0; iShot < (uint32_t)shotInstances.size(); ++iShot) {	// 
			const ::game::SShot																				& shotToTest													= shotInstances[iShot];
			if(iShip == (uint32_t)shotToTest.ShipIndex) 
				continue;
			const int32_t																					particleIndexShot												= shotToTest.ParticleIndex;
			const ::ftwl::SParticle2<float>																	& particleShip													= particleEngine.Particle[particleIndexShip];
			const ::ftwl::SParticle2<float>																	& particleShot													= particleEngine.Particle[particleIndexShot];
			const ::ftwl::SCircle2D<float>																	areaShip														= {.5, particleShip.Position};
			const ::ftwl::SCircle2D<float>																	areaShot														= {.5, particleShot.Position};
			if((particleShip.Position - particleShot.Position).Length() < (areaShip.Radius + areaShot.Radius)) { // Handle collision - Damage Ship - Remove the particle instance and related information.
				shipToTest.PointsCurrent.Shield																= (int32_t)::ftwl::max(0.0, shipToTest.PointsCurrent.Health - gameInstance.DefinitionsShot[shotToTest.ShotDescription].Damage / 3.0 * 2);
				shipToTest.PointsCurrent.Health																= (int32_t)::ftwl::max(0.0, shipToTest.PointsCurrent.Health - gameInstance.DefinitionsShot[shotToTest.ShotDescription].Damage / 3.0);
				particleEngine.ParticleState[particleIndexShot].Unused										= true;
				shotInstances.erase(shotInstances.begin() + iShot);
				--iShot;
			}
		}
		particleEngine.Particle[particleIndexShip]													= particleEngine.ParticleNext[particleIndexShip];
	}

	for(uint32_t iShot = 0; iShot < (uint32_t)shotInstances.size(); ++iShot) {	// Update shots and damage shots.
		::game::SShot																					& shotToTest													= shotInstances[iShot];
		const int32_t																					particleIndexShot												= shotToTest.ParticleIndex;
		bool																							wasNotDeletedByCollision										= false;
		for(uint32_t iShot2 = 1; iShot2 < (uint32_t)shotInstances.size(); ++iShot2) {	// 
			if(iShot == iShot2)
				continue;
			const ::game::SShot																				& shotToTest2													= shotInstances[iShot2];
			if(shotToTest.ShipIndex == shotToTest2.ShipIndex)
				continue;
			const int32_t																					particleIndexShot2												= shotToTest2.ParticleIndex;
			const ::ftwl::SParticle2<float>																	& particleShot													= particleEngine.Particle[particleIndexShot];
			const ::ftwl::SParticle2<float>																	& particleShot2													= particleEngine.Particle[particleIndexShot2];
			const ::ftwl::SCircle2D<float>																	areaShot														= {.5, particleShot.Position};
			const ::ftwl::SCircle2D<float>																	areaShot2														= {.5, particleShot2.Position};
			if((particleShot.Position - particleShot2.Position).Length() < (areaShot.Radius + areaShot.Radius)) { // Handle collision - Damage Shot - Remove the particle instance and related information.
				particleEngine.ParticleState[particleIndexShot].Unused										= true;
				shotInstances.erase(shotInstances.begin() + iShot);
				shotInstances.erase(shotInstances.begin() + (iShot2 - 1));
				break;
			}
		}
		if(wasNotDeletedByCollision)
			particleEngine.Particle[particleIndexShot]												= particleEngine.ParticleNext[particleIndexShot];
	}

	for(uint32_t iShip = 1; iShip < (uint32_t)shipInstances.size(); ++iShip) {	// Remove dead ships.
		::game::SShip																					& shipToTest														= shipInstances[iShip];
		if(shipToTest.Unused)
			continue;
		const int32_t																					particleIndexShip													= shipToTest.ParticleIndex;
		if(shipToTest.PointsCurrent.Health <= 0) {
			particleEngine.ParticleState[particleIndexShip].Unused										= true;
			shipToTest.Unused																			= true;
		}
	}

	for(uint32_t iShip = 1; iShip < (uint32_t)shipInstances.size(); ++iShip) {	// Make ships shoot.
		const ::game::SShip																				& shootingShip														= shipInstances[iShip];
		if(shootingShip.Unused || 95 > (rand() % 100))
			continue;
		const int32_t																					shotIndex															= ::game::addShot(gameInstance, shootingShip.SelectedShot, iShip); 
		::game::SShot																					& newShot															= shotInstances[shotIndex];
		particleEngine.ParticleNext[newShot.ParticleIndex].Position									= particleEngine.Particle[shootingShip.ParticleIndex].Position; 
		particleEngine.ParticleNext[newShot.ParticleIndex].Forces.Velocity.x						= -(float)gameInstance.DefinitionsShot[newShot.ShotDescription].Speed + particleEngine.Particle[shootingShip.ParticleIndex].Forces.Velocity.x; 
	}
	return 0;
}

template<typename _tInstance>
				::ftwl::error_t																updateStuff															(::std::vector<_tInstance>& stuffInstances, ::ftwl::SParticle2Engine<float> & particleEngine, const ::ftwl::SCoord2<int32_t> sizeEffective) { 
	for(uint32_t iShot = 0; iShot < (uint32_t)stuffInstances.size(); ++iShot) {	// 
		int32_t																							particleIndex														= stuffInstances[iShot].ParticleIndex;
		::ftwl::SParticle2<float>																		& particleNext														= particleEngine.ParticleNext[particleIndex];
		if( particleNext.Position.x < 0 || particleNext.Position.x >= sizeEffective.x
		 || particleNext.Position.y < 0 || particleNext.Position.y >= sizeEffective.y
		 ) { // Remove the particle instance and related information.
			particleEngine.ParticleState[particleIndex].Unused												= true;
			stuffInstances.erase(stuffInstances.begin() + iShot);
			--iShot;
		}
		else 
			particleEngine.Particle[particleIndex]														= particleNext;
	}
	return 0;
}
static inline	::ftwl::error_t																updateShots															(::game::SGame& gameInstance)															{ return updateStuff(gameInstance.Shots		, gameInstance.ParticleEngineGame	, gameInstance.CombatAreaSizeEffective.Cast<int32_t>()); }
static inline	::ftwl::error_t																updateEffects														(::game::SGame& gameInstance)															{ return updateStuff(gameInstance.Effects	, gameInstance.ParticleEngineEffects, gameInstance.CombatAreaSizeEffective.Cast<int32_t>()); }
static 			::ftwl::error_t																updateSpawners														(::game::SGame& gameInstance)															{ 
	::ftwl::SFrameInfo																				& frameInfo														= gameInstance.FrameInfo;																	
	::ftwl::SParticle2Engine<float>																	& particleEngine												= gameInstance.ParticleEngineGame;
	::std::vector<::game::SShip>																	& shipInstances													= gameInstance.Ships;
	for(uint32_t iSpawner = 0, spawnerCount = (uint32_t)gameInstance.Spawners.size(); iSpawner < spawnerCount; ++ iSpawner) {
		::game::SSpawner																				& spawner														= gameInstance.Spawners[iSpawner];
		for(uint32_t iSpawnRecord = 0; iSpawnRecord < spawner.Records.size(); ++iSpawnRecord) {
			if(frameInfo.Seconds.Total >= spawner.Records[iSpawnRecord].TimeSinceGameStarted) {
				int32_t																							shipIndex														= ::game::addShip(gameInstance, spawner.Records[iSpawnRecord].ShipTypeToSpawn);
				::game::SShip																					& newShip														= shipInstances[shipIndex];
				newShip.SelectedShot																		= ::game::SHOT_TYPE_LASER;
				particleEngine.Particle[newShip.ParticleIndex].Position										= spawner.Position.Cast<float>();
				::ftwl::SCoord2<float>																			distanceFromPlayer												= particleEngine.Particle[shipInstances[0].ParticleIndex].Position - spawner.Position.Cast<float>();
				if(distanceFromPlayer.LengthSquared())
					particleEngine.Particle[newShip.ParticleIndex].Forces.Velocity								= distanceFromPlayer / 2LL;
				spawner.Records.erase(spawner.Records.begin() + iSpawnRecord);
			}
		}
	}
	return 0;
}
			::ftwl::error_t																	game::update													(SGame& gameInstance, uint64_t lastTimeMicroseconds)										{ 
	::ftwl::SFrameInfo																				& frameInfo														= gameInstance.FrameInfo;																	
	frameInfo.Frame(::ftwl::min(lastTimeMicroseconds, 150000ULL));
	const float																						lastFrameSeconds												= (float)frameInfo.Seconds.LastFrame;

	::std::vector<::game::SShip>																	& shipInstances													= gameInstance.Ships;
	::std::vector<::game::SShot>																	& shotInstances													= gameInstance.Shots;
	::ftwl::SParticle2Engine<float>																	& particleEngine												= gameInstance.ParticleEngineGame;

	{	// ---------------------------- Handle player input
		::game::SShip																					& playerShip													= shipInstances[0];
		// SWitch weapon
			 if(::GetAsyncKeyState('1') || ::GetAsyncKeyState(VK_NUMPAD1)) { if(!GetAsyncKeyState(VK_CONTROL)) playerShip.SelectedShot = SHOT_TYPE_ROCK		; else { playerShip.ShipDescription = (SHIP_TYPE)(1 % SHIP_TYPE_COUNT); } }
		else if(::GetAsyncKeyState('2') || ::GetAsyncKeyState(VK_NUMPAD2)) { if(!GetAsyncKeyState(VK_CONTROL)) playerShip.SelectedShot = SHOT_TYPE_ARROW	; else { playerShip.ShipDescription = (SHIP_TYPE)(2 % SHIP_TYPE_COUNT); } }
		else if(::GetAsyncKeyState('3') || ::GetAsyncKeyState(VK_NUMPAD3)) { if(!GetAsyncKeyState(VK_CONTROL)) playerShip.SelectedShot = SHOT_TYPE_FIREBALL	; else { playerShip.ShipDescription = (SHIP_TYPE)(3 % SHIP_TYPE_COUNT); } }
		else if(::GetAsyncKeyState('4') || ::GetAsyncKeyState(VK_NUMPAD4)) { if(!GetAsyncKeyState(VK_CONTROL)) playerShip.SelectedShot = SHOT_TYPE_LASER	; else { playerShip.ShipDescription = (SHIP_TYPE)(4 % SHIP_TYPE_COUNT); } }
		else if(::GetAsyncKeyState('5') || ::GetAsyncKeyState(VK_NUMPAD5)) { if(!GetAsyncKeyState(VK_CONTROL)) playerShip.SelectedShot = SHOT_TYPE_POISON	; else { playerShip.ShipDescription = (SHIP_TYPE)(5 % SHIP_TYPE_COUNT); } }
		else if(::GetAsyncKeyState('6') || ::GetAsyncKeyState(VK_NUMPAD6)) { if(!GetAsyncKeyState(VK_CONTROL)) playerShip.SelectedShot = SHOT_TYPE_BULLET	; else { playerShip.ShipDescription = (SHIP_TYPE)(6 % SHIP_TYPE_COUNT); } }
		else if(::GetAsyncKeyState('7') || ::GetAsyncKeyState(VK_NUMPAD7)) { if(!GetAsyncKeyState(VK_CONTROL)) playerShip.SelectedShot = SHOT_TYPE_PLASMA	; else { playerShip.ShipDescription = (SHIP_TYPE)(7 % SHIP_TYPE_COUNT); } }
		else if(::GetAsyncKeyState('8') || ::GetAsyncKeyState(VK_NUMPAD8)) { if(!GetAsyncKeyState(VK_CONTROL)) playerShip.SelectedShot = SHOT_TYPE_BOMB		; else { playerShip.ShipDescription = (SHIP_TYPE)(8 % SHIP_TYPE_COUNT); } }

		if(::GetAsyncKeyState(VK_SPACE)) // Shoot
			for(uint32_t i = 0; i < 2; ++i ) {
				int32_t																							shotIndex														= ::game::addShot(gameInstance, playerShip.SelectedShot, 0); 
				particleEngine.Particle[shotInstances[shotIndex].ParticleIndex].Position					= particleEngine.Particle[playerShip.ParticleIndex].Position + ::ftwl::SCoord2<float>{(float)i,}; 
				//particleEngine.Particle[shotInstances[shotIndex].ParticleIndex].Forces.Velocity.x			= (float)gameInstance.DefinitionsShot[shotInstances[shotIndex].ShotDescription].Speed + particleEngine.Particle[playerShip.ParticleIndex].Forces.Velocity.x; 
				particleEngine.Particle[shotInstances[shotIndex].ParticleIndex].Forces.Velocity.x			= (float)gameInstance.DefinitionsShot[shotInstances[shotIndex].ShotDescription].Speed; 
				particleEngine.Particle[shotInstances[shotIndex].ParticleIndex].Forces.Velocity.x			+= particleEngine.Particle[playerShip.ParticleIndex].Forces.Velocity.x;
				particleEngine.Particle[shotInstances[shotIndex].ParticleIndex].Forces.Velocity.y			+= particleEngine.Particle[playerShip.ParticleIndex].Forces.Velocity.y * .5f;
			}

		// Move
		bool																							bUp																= ::GetAsyncKeyState('W');
		bool																							bRight															= ::GetAsyncKeyState('D');
		bool																							bDown															= ::GetAsyncKeyState('S');
		bool																							bLeft															= ::GetAsyncKeyState('A');

		::ftwl::SParticle2<float>																		& playerParticle												= particleEngine.Particle[playerShip.ParticleIndex];
		if( bUp		) playerParticle.Forces.AccumulatedForce.y											-= lastFrameSeconds * 1000 * (GetAsyncKeyState(VK_SHIFT) ? 5.0f : 1);
		if( bRight	) playerParticle.Forces.AccumulatedForce.x											+= lastFrameSeconds * 1000 * (GetAsyncKeyState(VK_SHIFT) ? 5.0f : 1);
		if( bDown	) playerParticle.Forces.AccumulatedForce.y											+= lastFrameSeconds * 1000 * (GetAsyncKeyState(VK_SHIFT) ? 5.0f : 1);
		if( bLeft	) playerParticle.Forces.AccumulatedForce.x											-= lastFrameSeconds * 1000 * (GetAsyncKeyState(VK_SHIFT) ? 5.0f : 1);
		if(!bUp && !bRight && !bDown && !bLeft) {
		}
		else {
			playerParticle.Forces.Acceleration															= {}; 
			particleEngine.ParticleState[playerShip.ParticleIndex].Active								= true;
		}
	}

	// -- Spawn a background star.
	if(0 == rand() % 10) {
		int32_t																								newEffectIndex													= ::game::addEffect(gameInstance, ::game::EFFECT_TYPE_STARS);
		int32_t																								effectParticleIndex												= gameInstance.Effects[newEffectIndex].ParticleIndex;
		gameInstance.ParticleEngineEffects.Particle[effectParticleIndex].Position						= {gameInstance.CombatAreaSizeEffective.x - 1.0f, float(rand() % gameInstance.CombatAreaSizeEffective.y)};
		gameInstance.ParticleEngineEffects.Particle[effectParticleIndex].Forces.Velocity.x				*= 1 + (rand() % 10) * .1f;
	}

	// -- Apply background force.
	for(uint32_t iParticle = 0, particleCount = (uint32_t)particleEngine.Particle.size(); iParticle < particleCount; ++iParticle)
		if(particleEngine.Particle[iParticle].Damping != 1.0f)
			particleEngine.Particle[iParticle].Forces.AccumulatedForce.x									-= lastFrameSeconds * 2;

	particleEngine.Integrate(lastFrameSeconds, frameInfo.Seconds.LastFrameHalfSquared);
	gameInstance.ParticleEngineEffects.Integrate(lastFrameSeconds, frameInfo.Seconds.LastFrameHalfSquared);

	::updateShips	(gameInstance);
	::updateShots	(gameInstance);
	::updateEffects	(gameInstance);
	::updateSpawners(gameInstance);
	return 0; 
}
