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
			::ftwlib::error_t																game::addShot													(::game::SGame& gameInstance, ::game::SHOT_TYPE type)										{ 
	::game::SShot																					newInstance; 
	newInstance.ParticleIndex																	= gameInstance.ParticleEngine.AddParticle(gameInstance.DefinitionsParticleShot[type]);	
	newInstance.ShotDescription																	= type; 
	newInstance.RoundsCurrent																	= gameInstance.DefinitionsShot[type].RoundsMax;
	uint32_t																						newShotIndex													= (uint32_t)gameInstance.Shots.size();
	gameInstance.Shots.push_back(newInstance); 
	return newShotIndex;
}
			::ftwlib::error_t																updateShips														(::game::SGame& gameInstance)																{
	::std::vector<::game::SShip>																	& shipInstances													= gameInstance.Ships;
	//::std::vector<::game::SShot>																	& shotInstances													= gameInstance.Shots;
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

	for(uint32_t iShip = 1; iShip < shipInstances.size(); ++iShip) {	// 
		int32_t																							particleIndex													= shipInstances[iShip].ParticleIndex;
		::game::SParticle2<float>																		& particleNext													= particleEngine.ParticleNext[particleIndex];
		if( particleNext.Position.x < 0 || particleNext.Position.x >= sizeEffective.x
		 || particleNext.Position.y < 0 || particleNext.Position.y >= sizeEffective.y
		 ) { // Remove the particle instance and related information.
			particleEngine.ParticleState[particleIndex].Unused											= true;
			shipInstances.erase(shipInstances.begin() + iShip);
			--iShip;
		}
		else { 
			static constexpr	const double																gravity															= 9.8;
			::game::SParticle2<float>																		& particleCurrent												= particleEngine.Particle[particleIndex];
			particleCurrent																				= particleEngine.ParticleNext[particleIndex];
		}
	}
	return 0;
}
			::ftwlib::error_t																updateShots														(::game::SGame& gameInstance)																{ 
	//::std::vector<::game::SShip>																	& shipInstances													= gameInstance.Ships;
	::std::vector<::game::SShot>																	& shotInstances													= gameInstance.Shots;
	::game::SParticle2Engine<float>																	& particleEngine												= gameInstance.ParticleEngine;
	const ::ftwlib::SCoord2<int32_t>																sizeEffective													= gameInstance.CombatAreaSizeEffective.Cast<int32_t>();

	for(uint32_t iShot = 0; iShot < shotInstances.size(); ++iShot) {	// 
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
		int32_t																							shotIndex														= ::game::addShot(gameInstance, shipInstances[0].SelectedShot); 
		particleEngine.Particle[shotInstances[shotIndex].ParticleIndex].Position					= particleEngine.Particle[shipInstances[0].ParticleIndex].Position; 
		particleEngine.Particle[shotInstances[shotIndex].ParticleIndex].Forces.Velocity.x			= (float)gameInstance.DefinitionsShot[shotInstances[shotIndex].ShotDescription].Speed; 
	}

	// Move
	bool																							bUp																= ::GetAsyncKeyState('W');
	bool																							bRight															= ::GetAsyncKeyState('D');
	bool																							bDown															= ::GetAsyncKeyState('S');
	bool																							bLeft															= ::GetAsyncKeyState('A');

	::game::SParticle2<float>																		& playerParticle												= particleEngine.Particle[shipInstances[0].ParticleIndex];
	if( bUp		) playerParticle.Forces.Velocity.y													-= lastFrameSeconds * 50;
	if( bRight	) playerParticle.Forces.Velocity.x													+= lastFrameSeconds * 50;
	if( bDown	) playerParticle.Forces.Velocity.y													+= lastFrameSeconds * 50;
	if( bLeft	) playerParticle.Forces.Velocity.x													-= lastFrameSeconds * 50;
	if(!bUp && !bRight && !bDown && !bLeft) {

	}
	particleEngine.Integrate(lastFrameSeconds, frameInfo.Seconds.LastFrameHalfSquared);
	::updateShips(gameInstance);
	::updateShots(gameInstance);
	return 0; 
}
