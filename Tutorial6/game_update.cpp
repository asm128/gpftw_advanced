#include "game.h"				

#include <Windows.h>	// for GetAsyncKeyState()

			::ftwlib::error_t																game::addShip													(::game::SGame& gameInstance, ::game::SHIP_TYPE type)										{
	::game::SShip																					newInstance; 
	newInstance.ParticleIndex																	= gameInstance.ParticleEngine.AddParticle(gameInstance.DefinitionsParticleShip[type]);	
	gameInstance.ParticleEngine.Particle[newInstance.ParticleIndex].Position					= {10, 10};
	newInstance.ShipDescription																	= type; 
	newInstance.PointsCurrent																	= gameInstance.DefinitionsShip[type].PointsMax;
	uint32_t																						newShipIndex													= gameInstance.Ships.size();
	gameInstance.Ships.push_back(newInstance); 
	return newShipIndex;
}
			::ftwlib::error_t																game::addShot													(::game::SGame& gameInstance, ::game::SHOT_TYPE type)										{ 
	::game::SShot																					newInstance; 
	newInstance.ParticleIndex																	= gameInstance.ParticleEngine.AddParticle(gameInstance.DefinitionsParticleShot[type]);	
	newInstance.ShotDescription																	= type; 
	newInstance.RoundsCurrent																	= gameInstance.DefinitionsShot[type].RoundsMax;
	uint32_t																						newShotIndex													= gameInstance.Shots.size();
	gameInstance.Shots.push_back(newInstance); 
	return newShotIndex;
}
			::ftwlib::error_t																updateShips														(::game::SGame& gameInstance)																{
	::std::vector<::game::SShip>																	& shipInstances													= gameInstance.Ships;
	//::std::vector<::game::SShot>																	& shotInstances													= gameInstance.Shots;
	::game::SParticle2Engine<float>																	& particleEngine												= gameInstance.ParticleEngine;
	const ::ftwlib::SCoord2<int32_t>																sizeEffective													= gameInstance.CombatAreaSizeEffective.Cast<int32_t>();

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
		else { // Apply forces from wind and gravity.
			static constexpr	const double																gravity															= 9.8;
			::game::SParticle2<float>																		& particleCurrent												= particleEngine.Particle[particleIndex];
			particleCurrent																				= particleEngine.ParticleNext[particleIndex];
			//particleCurrent.Forces.AccumulatedForce														= {0, float(gravity * lastFrameSeconds)};
			//particleCurrent.Forces.AccumulatedForce.x													+= float(windDirection + ((rand() % 100) / 100.0 - .5));
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
		else { // Apply forces from wind and gravity.
			static constexpr	const double																gravity															= 9.8;
			::game::SParticle2<float>																		& particleCurrent												= particleEngine.Particle[particleIndex];
			particleCurrent																				= particleEngine.ParticleNext[particleIndex];
			//particleCurrent.Forces.AccumulatedForce														= {0, float(gravity * lastFrameSeconds)};
			//particleCurrent.Forces.AccumulatedForce.x													+= float(windDirection + ((rand() % 100) / 100.0 - .5));
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
	::game::SShip																					& playerShip													= shipInstances[0];
	::game::SParticle2<float>																		& playerParticle												= particleEngine.Particle[playerShip.ParticleIndex];
		 if(::GetAsyncKeyState('1') || ::GetAsyncKeyState(VK_NUMPAD1)) playerShip.SelectedShot = ::game::SHOT_TYPE_ROCK		; 
	else if(::GetAsyncKeyState('2') || ::GetAsyncKeyState(VK_NUMPAD2)) playerShip.SelectedShot = ::game::SHOT_TYPE_ARROW	; 
	else if(::GetAsyncKeyState('3') || ::GetAsyncKeyState(VK_NUMPAD3)) playerShip.SelectedShot = ::game::SHOT_TYPE_FIREBALL	; 
	else if(::GetAsyncKeyState('4') || ::GetAsyncKeyState(VK_NUMPAD4)) playerShip.SelectedShot = ::game::SHOT_TYPE_LASER	; 
	else if(::GetAsyncKeyState('5') || ::GetAsyncKeyState(VK_NUMPAD5)) playerShip.SelectedShot = ::game::SHOT_TYPE_POISON	; 
	else if(::GetAsyncKeyState('6') || ::GetAsyncKeyState(VK_NUMPAD6)) playerShip.SelectedShot = ::game::SHOT_TYPE_PLASMA	; 
	else if(::GetAsyncKeyState('7') || ::GetAsyncKeyState(VK_NUMPAD7)) playerShip.SelectedShot = ::game::SHOT_TYPE_BOMB		; 



	if(::GetAsyncKeyState(VK_SPACE))	{ int32_t shotIndex = ::game::addShot(gameInstance, playerShip.SelectedShot); particleEngine.Particle[shotInstances[shotIndex].ParticleIndex].Position = playerParticle.Position; }
	if(::GetAsyncKeyState('W'))		{ playerParticle.Position.y -= lastFrameSeconds * 10;	}
	if(::GetAsyncKeyState('A'))		{ playerParticle.Position.x -= lastFrameSeconds * 10;	}
	if(::GetAsyncKeyState('S'))		{ playerParticle.Position.y += lastFrameSeconds * 10;	}
	if(::GetAsyncKeyState('D'))		{ playerParticle.Position.x += lastFrameSeconds * 10;	}
	if( !::GetAsyncKeyState('W')
	 && !::GetAsyncKeyState('A')
	 && !::GetAsyncKeyState('S')
	 && !::GetAsyncKeyState('D')
	 )
	{}

	particleEngine.Integrate(lastFrameSeconds, frameInfo.Seconds.LastFrameHalfSquared);
	::updateShips(gameInstance);
	::updateShots(gameInstance);
	return 0; 
}
