#include "game.h"				

#include <Windows.h>	// for GetAsyncKeyState()

			::ftwlib::error_t																game::addShip													(::game::SGame& gameInstance, ::game::SHIP_TYPE type)										{
	::game::SShip																					newInstance; 
	newInstance.ParticleIndex																	= gameInstance.ParticleEngine.AddParticle(gameInstance.DefinitionsParticleShip[type]);	
	gameInstance.ParticleEngine.Particle[newInstance.ParticleIndex].Position					= {10, 10};
	newInstance.ShipDescription																	= type; 
	newInstance.PointsCurrent																	= gameInstance.DefinitionsShip[type].PointsMax;
	gameInstance.Ships.push_back(newInstance); 
	return 0;
}
			::ftwlib::error_t																game::addShot													(::game::SGame& gameInstance, ::game::SHOT_TYPE type)										{ 
	::game::SShot																					newInstance; 
	newInstance.ParticleIndex																	= gameInstance.ParticleEngine.AddParticle(gameInstance.DefinitionsParticleShot[type]);	
	newInstance.ShotDescription																	= type; 
	newInstance.RoundsCurrent																	= gameInstance.DefinitionsShot[type].RoundsMax;
	gameInstance.Shots.push_back(newInstance); 
	return 0; 
}
			::ftwlib::error_t																updateShips														(::game::SGame& gameInstance)																{
	::std::vector<::game::SShip>																	& shipInstances													= gameInstance.Ships;
	//::std::vector<::game::SShot>																	& shotInstances													= gameInstance.Shots;
	::game::SParticle2Engine<float>																	& particleEngine												= gameInstance.ParticleEngine;
	for(uint32_t iShip = 0; iShip < shipInstances.size(); ++iShip) {	// 
		int32_t																							particleIndex													= shipInstances[iShip].ParticleIndex;
		::game::SParticle2<float>																		& particleNext													= particleEngine.ParticleNext[particleIndex];
		if( particleNext.Position.x < (0 - (int32_t)gameInstance.MarginSize.x) || particleNext.Position.x >= (int32_t)(gameInstance.VisibleSize.x + gameInstance.MarginSize.x)
		 || particleNext.Position.y < (0 - (int32_t)gameInstance.MarginSize.y) || particleNext.Position.y >= (int32_t)(gameInstance.VisibleSize.y + gameInstance.MarginSize.y)
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
	for(uint32_t iShot = 0; iShot < shotInstances.size(); ++iShot) {	// 
		int32_t																							particleIndex													= shotInstances[iShot].ParticleIndex;
		::game::SParticle2<float>																		& particleNext													= particleEngine.ParticleNext[particleIndex];
		if( particleNext.Position.x < (0 - gameInstance.MarginSize.x) || particleNext.Position.x >= (gameInstance.VisibleSize.x + gameInstance.MarginSize.x)
		 || particleNext.Position.y < (0 - gameInstance.MarginSize.y) || particleNext.Position.y >= (gameInstance.VisibleSize.y + gameInstance.MarginSize.y)
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

		 if(GetAsyncKeyState('1') || GetAsyncKeyState(VK_NUMPAD1)) gameInstance.Ships[0].SelectedShot = ::game::SHOT_TYPE_ROCK		; 
	else if(GetAsyncKeyState('2') || GetAsyncKeyState(VK_NUMPAD2)) gameInstance.Ships[0].SelectedShot = ::game::SHOT_TYPE_ARROW		; 
	else if(GetAsyncKeyState('3') || GetAsyncKeyState(VK_NUMPAD3)) gameInstance.Ships[0].SelectedShot = ::game::SHOT_TYPE_FIREBALL	; 
	else if(GetAsyncKeyState('4') || GetAsyncKeyState(VK_NUMPAD4)) gameInstance.Ships[0].SelectedShot = ::game::SHOT_TYPE_LASER		; 
	else if(GetAsyncKeyState('5') || GetAsyncKeyState(VK_NUMPAD5)) gameInstance.Ships[0].SelectedShot = ::game::SHOT_TYPE_POISON	; 
	else if(GetAsyncKeyState('6') || GetAsyncKeyState(VK_NUMPAD6)) gameInstance.Ships[0].SelectedShot = ::game::SHOT_TYPE_PLASMA	; 
	else if(GetAsyncKeyState('7') || GetAsyncKeyState(VK_NUMPAD7)) gameInstance.Ships[0].SelectedShot = ::game::SHOT_TYPE_BOMB		; 

	//const double																					windDirection													= (float)(sin(frameInfo.Seconds.Total / 5.0) * .25);
	gameInstance.ParticleEngine.Integrate(lastFrameSeconds, frameInfo.Seconds.LastFrameHalfSquared);
	updateShips(gameInstance);
	updateShots(gameInstance);
	return 0; 
}
