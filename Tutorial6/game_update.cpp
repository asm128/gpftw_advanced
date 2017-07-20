#include "game.h"				

#include <Windows.h>	// for GetAsyncKeyState()

			::ftwlib::error_t																game::addEffect													(::game::SGame& gameInstance, EFFECT_TYPE type)												{
	::game::SEffect																					newInstance														= {type,};
	newInstance.ParticleIndex																	= gameInstance.ParticleEngineEffects.AddParticle(gameInstance.DefinitionsParticleEffect[type]);	
	uint32_t																						newEffectIndex													= (uint32_t)gameInstance.Effects.size();
	gameInstance.Effects.push_back(newInstance); 
	return newEffectIndex;
}
			::ftwlib::error_t																game::addShip													(::game::SGame& gameInstance, ::game::SHIP_TYPE type)										{
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
			::ftwlib::error_t																game::addShot													(::game::SGame& gameInstance, ::game::SHOT_TYPE type, int32_t shipIndex)					{ 
	::game::SShot																					newInstance														= {type,}; 
	newInstance.ParticleIndex																	= gameInstance.ParticleEngineGame.AddParticle(gameInstance.DefinitionsParticleShot[type]);	
	newInstance.ShipIndex																		= shipIndex;
	newInstance.RoundsCurrent																	= gameInstance.DefinitionsShot[type].RoundsMax;
	const uint32_t																					newShotIndex													= (uint32_t)gameInstance.Shots.size();
	gameInstance.Shots.push_back(newInstance); 
	return newShotIndex;
}

static		::ftwlib::error_t																updateShips														(::game::SGame& gameInstance)																{
	::std::vector<::game::SShip>																	& shipInstances													= gameInstance.Ships;
	::ftwlib::SParticle2Engine<float>																& particleEngine												= gameInstance.ParticleEngineGame;
	const ::ftwlib::SCoord2<int32_t>																sizeEffective													= gameInstance.CombatAreaSizeEffective.Cast<int32_t>();
	{
		const int32_t																					particleIndex													= shipInstances[0].ParticleIndex;
		::ftwlib::SParticle2<float>																		& particleNext													= particleEngine.ParticleNext[particleIndex];
		if( particleNext.Position.x < 0 || particleNext.Position.x >= sizeEffective.x ) particleNext.Forces.Velocity.x = 0;
		if( particleNext.Position.y < 0 || particleNext.Position.y >= sizeEffective.y ) particleNext.Forces.Velocity.y = 0;

		particleNext.Position.x																		= ::ftwlib::min(::ftwlib::max(particleNext.Position.x, 0.0f), sizeEffective.x - 1.0f);
		particleNext.Position.y																		= ::ftwlib::min(::ftwlib::max(particleNext.Position.y, 0.0f), sizeEffective.y - 1.0f);
		particleEngine.Particle[particleIndex]														= particleNext;
	}

	for(uint32_t iShip = 1; iShip < (uint32_t)shipInstances.size(); ++iShip) {	// Remove ships that are outside the display area.
		::game::SShip																					& shipToTest													= shipInstances[iShip];
		if(shipToTest.Unused)
			continue;
		const int32_t																					particleIndexShip												= shipToTest.ParticleIndex;
		const ::ftwlib::SParticle2<float>																& particleNextShip												= particleEngine.ParticleNext[particleIndexShip];
		if( particleNextShip.Position.x < 0 || particleNextShip.Position.x >= sizeEffective.x
		 || particleNextShip.Position.y < 0 || particleNextShip.Position.y >= sizeEffective.y
		 ) { // Remove the particle instance and related information.
			particleEngine.ParticleState[particleIndexShip].Unused											= true;
			shipToTest.Unused																				= true;
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
			const int32_t																					particleIndexShot												= shotToTest.ParticleIndex;
			const ::ftwlib::SParticle2<float>																& particleShip													= particleEngine.Particle[particleIndexShip];
			const ::ftwlib::SParticle2<float>																& particleShot													= particleEngine.Particle[particleIndexShot];
			const ::ftwlib::SCircle2D<float>																areaShip														= {.5, particleShip.Position};
			const ::ftwlib::SCircle2D<float>																areaShot														= {.5, particleShot.Position};
			if(iShip == (uint32_t)shotToTest.ShipIndex) 
				continue;
			if( (particleShip.Position - particleShot.Position).Length() < (areaShip.Radius + areaShot.Radius)) { // Handle collision - Remove the particle instance and related information.
				shipToTest.PointsCurrent.Health																-=	gameInstance.DefinitionsShot[shotToTest.ShotDescription].Damage;
				particleEngine.ParticleState[particleIndexShot].Unused										= true;
				shotInstances.erase(shotInstances.begin() + iShot);
				--iShot;
			}
		}
		particleEngine.Particle[particleIndexShip]													= particleEngine.ParticleNext[particleIndexShip];
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
		if(shootingShip.Unused || 95 > rand() % 100)
			continue;
		const int32_t																					shotIndex															= ::game::addShot(gameInstance, shootingShip.SelectedShot, iShip); 
		::game::SShot																					& newShot															= shotInstances[shotIndex];
		particleEngine.ParticleNext[newShot.ParticleIndex].Position									= particleEngine.Particle[shootingShip.ParticleIndex].Position; 
		particleEngine.ParticleNext[newShot.ParticleIndex].Forces.Velocity.x						= -(float)gameInstance.DefinitionsShot[newShot.ShotDescription].Speed + particleEngine.Particle[shootingShip.ParticleIndex].Forces.Velocity.x; 
	}
	return 0;
}

template<typename _tInstance>
			::ftwlib::error_t																updateStuff														(::std::vector<_tInstance>& stuffInstances, ::ftwlib::SParticle2Engine<float> & particleEngine, const ::ftwlib::SCoord2<int32_t> sizeEffective) { 
	for(uint32_t iShot = 0; iShot < (uint32_t)stuffInstances.size(); ++iShot) {	// 
		int32_t																							particleIndex													= stuffInstances[iShot].ParticleIndex;
		::ftwlib::SParticle2<float>																		& particleNext													= particleEngine.ParticleNext[particleIndex];
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
static inline	::ftwlib::error_t																updateShots															(::game::SGame& gameInstance)															{ return updateStuff(gameInstance.Shots		, gameInstance.ParticleEngineGame	, gameInstance.CombatAreaSizeEffective.Cast<int32_t>()); }
static inline	::ftwlib::error_t																updateEffects														(::game::SGame& gameInstance)															{ return updateStuff(gameInstance.Effects	, gameInstance.ParticleEngineEffects, gameInstance.CombatAreaSizeEffective.Cast<int32_t>()); }
static 			::ftwlib::error_t																updateSpawners														(::game::SGame& gameInstance)															{ 
	::ftwlib::SFrameInfo																				& frameInfo														= gameInstance.FrameInfo;																	
	::ftwlib::SParticle2Engine<float>																	& particleEngine												= gameInstance.ParticleEngineGame;
	::std::vector<::game::SShip>																		& shipInstances													= gameInstance.Ships;
	for(uint32_t iSpawner = 0, spawnerCount = (uint32_t)gameInstance.Spawners.size(); iSpawner < spawnerCount; ++ iSpawner) {
		::game::SSpawner																				& spawner														= gameInstance.Spawners[iSpawner];
		for(uint32_t iSpawnRecord = 0; iSpawnRecord < spawner.Records.size(); ++iSpawnRecord) {
			if(frameInfo.Seconds.Total >= spawner.Records[iSpawnRecord].TimeSinceGameStarted) {
				int32_t																							shipIndex														= ::game::addShip(gameInstance, spawner.Records[iSpawnRecord].ShipTypeToSpawn);
				::game::SShip																					& newShip														= shipInstances[shipIndex];
				newShip.SelectedShot																		= ::game::SHOT_TYPE_LASER;
				particleEngine.Particle[newShip.ParticleIndex].Position										= spawner.Position.Cast<float>();
				::ftwlib::SCoord2<float>																		distanceFromPlayer												= particleEngine.Particle[shipInstances[0].ParticleIndex].Position - spawner.Position.Cast<float>();
				if(distanceFromPlayer.LengthSquared())
					particleEngine.Particle[newShip.ParticleIndex].Forces.Velocity								= distanceFromPlayer / 2LL;
				spawner.Records.erase(spawner.Records.begin() + iSpawnRecord);
			}
		}
	}
	return 0;
}
			::ftwlib::error_t																game::update													(SGame& gameInstance, uint64_t lastTimeMicroseconds)										{ 
	::ftwlib::SFrameInfo																			& frameInfo														= gameInstance.FrameInfo;																	
	frameInfo.Frame(::ftwlib::min(lastTimeMicroseconds, 150000ULL));
	const float																						lastFrameSeconds												= (float)frameInfo.Seconds.LastFrame;

	::std::vector<::game::SShip>																	& shipInstances													= gameInstance.Ships;
	::std::vector<::game::SShot>																	& shotInstances													= gameInstance.Shots;
	::ftwlib::SParticle2Engine<float>																& particleEngine												= gameInstance.ParticleEngineGame;

	::game::SShip																					& playerShip													= shipInstances[0];
	// SWitch weapon
		 if(::GetAsyncKeyState('1') || ::GetAsyncKeyState(VK_NUMPAD1)) playerShip.SelectedShot = SHOT_TYPE_ROCK		;
	else if(::GetAsyncKeyState('2') || ::GetAsyncKeyState(VK_NUMPAD2)) playerShip.SelectedShot = SHOT_TYPE_ARROW		;
	else if(::GetAsyncKeyState('3') || ::GetAsyncKeyState(VK_NUMPAD3)) playerShip.SelectedShot = SHOT_TYPE_FIREBALL	;
	else if(::GetAsyncKeyState('4') || ::GetAsyncKeyState(VK_NUMPAD4)) playerShip.SelectedShot = SHOT_TYPE_LASER		;
	else if(::GetAsyncKeyState('5') || ::GetAsyncKeyState(VK_NUMPAD5)) playerShip.SelectedShot = SHOT_TYPE_POISON		;
	else if(::GetAsyncKeyState('6') || ::GetAsyncKeyState(VK_NUMPAD6)) playerShip.SelectedShot = SHOT_TYPE_BULLET		;
	else if(::GetAsyncKeyState('7') || ::GetAsyncKeyState(VK_NUMPAD7)) playerShip.SelectedShot = SHOT_TYPE_PLASMA		;
	else if(::GetAsyncKeyState('8') || ::GetAsyncKeyState(VK_NUMPAD8)) playerShip.SelectedShot = SHOT_TYPE_BOMB		;

	if(::GetAsyncKeyState(VK_SPACE)) { // Shoot
		for(uint32_t i = 0; i < 2; ++i ){
			int32_t																							shotIndex														= ::game::addShot(gameInstance, playerShip.SelectedShot, 0); 
			particleEngine.Particle[shotInstances[shotIndex].ParticleIndex].Position					= particleEngine.Particle[playerShip.ParticleIndex].Position + ::ftwlib::SCoord2<float>{(float)i,}; 
			//particleEngine.Particle[shotInstances[shotIndex].ParticleIndex].Forces.Velocity.x			= (float)gameInstance.DefinitionsShot[shotInstances[shotIndex].ShotDescription].Speed + particleEngine.Particle[playerShip.ParticleIndex].Forces.Velocity.x; 
			particleEngine.Particle[shotInstances[shotIndex].ParticleIndex].Forces.Velocity.x			= (float)gameInstance.DefinitionsShot[shotInstances[shotIndex].ShotDescription].Speed; 
			particleEngine.Particle[shotInstances[shotIndex].ParticleIndex].Forces.Velocity.x			+= particleEngine.Particle[playerShip.ParticleIndex].Forces.Velocity.x;
			particleEngine.Particle[shotInstances[shotIndex].ParticleIndex].Forces.Velocity.y			+= particleEngine.Particle[playerShip.ParticleIndex].Forces.Velocity.y * .5f;
		}
	}

	// Move
	bool																							bUp																= ::GetAsyncKeyState('W');
	bool																							bRight															= ::GetAsyncKeyState('D');
	bool																							bDown															= ::GetAsyncKeyState('S');
	bool																							bLeft															= ::GetAsyncKeyState('A');

	::ftwlib::SParticle2<float>																		& playerParticle												= particleEngine.Particle[playerShip.ParticleIndex];
	if( bUp		) playerParticle.Forces.AccumulatedForce.y											-= lastFrameSeconds * 700 * (GetAsyncKeyState(VK_SHIFT) ? 4.0f : 1);
	if( bRight	) playerParticle.Forces.AccumulatedForce.x											+= lastFrameSeconds * 700 * (GetAsyncKeyState(VK_SHIFT) ? 4.0f : 1);
	if( bDown	) playerParticle.Forces.AccumulatedForce.y											+= lastFrameSeconds * 700 * (GetAsyncKeyState(VK_SHIFT) ? 4.0f : 1);
	if( bLeft	) playerParticle.Forces.AccumulatedForce.x											-= lastFrameSeconds * 700 * (GetAsyncKeyState(VK_SHIFT) ? 4.0f : 1);
	if(!bUp && !bRight && !bDown && !bLeft) {
	}
	else {
		playerParticle.Forces.Acceleration															= {}; 
		particleEngine.ParticleState[playerShip.ParticleIndex].Active								= true;
	}

	// -- Spawn a background star.
	int32_t																							newEffectIndex													= ::game::addEffect(gameInstance, ::game::EFFECT_TYPE_STARS);
	gameInstance.ParticleEngineEffects.Particle[gameInstance.Effects[newEffectIndex].ParticleIndex].Position			= {gameInstance.CombatAreaSizeVisible.x - 1.0f, float(rand() % gameInstance.CombatAreaSizeVisible.y)};
	gameInstance.ParticleEngineEffects.Particle[gameInstance.Effects[newEffectIndex].ParticleIndex].Forces.Velocity.x	*= 1 + (rand() % 10) * .1f;

	// -- Apply background force.
	for(uint32_t iParticle = 0, particleCount = (uint32_t)particleEngine.Particle.size(); iParticle < particleCount; ++iParticle)
		particleEngine.Particle[iParticle].Forces.AccumulatedForce.x								-= lastFrameSeconds * 2;

	particleEngine.Integrate(lastFrameSeconds, frameInfo.Seconds.LastFrameHalfSquared);
	gameInstance.ParticleEngineEffects.Integrate(lastFrameSeconds, frameInfo.Seconds.LastFrameHalfSquared);

	::updateShips	(gameInstance);
	::updateShots	(gameInstance);
	::updateEffects	(gameInstance);
	::updateSpawners(gameInstance);
	return 0; 
}
