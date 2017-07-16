#include "game.h"
#include "ftw_ascii_color.h"

			::ftwlib::error_t																game::render													(SGame& gameInstance, ::ftwlib::SScreenASCII& screenAscii)									{ 
	::std::vector<::game::SShip>																	& shipInstances													= gameInstance.Ships;
	::std::vector<::game::SShot>																	& shotInstances													= gameInstance.Shots;
	::game::SParticle2Engine<float>																	& particleEngine												= gameInstance.ParticleEngine;

	for(uint32_t iShot = 0; iShot < shotInstances.size(); ++iShot) {
		int32_t																							particleIndex													= shotInstances[iShot].ParticleIndex;
		const ::ftwlib::SCoord2<float>																	particlePosition												= particleEngine.Particle[particleIndex].Position;
		::game::SParticle2<float>																		& particle														= particleEngine.Particle[particleIndex];
		if( particle.Position.x < 0 || particle.Position.x >= screenAscii.Width
		 || particle.Position.y < 0 || particle.Position.y >= screenAscii.Height
		 ) continue;
		const int32_t																					linearIndex														= (int32_t)particlePosition.y * screenAscii.Width + (int32_t)particlePosition.x;
		screenAscii.Characters	[linearIndex]														= 1 + gameInstance.DefinitionsShot[shotInstances[iShot].ShotDescription].Image;
		screenAscii.Colors		[linearIndex]														= gameInstance.DefinitionsShot[shotInstances[iShot].ShotDescription].Color;
	}

	for(uint32_t iShip = 0; iShip < shipInstances.size(); ++iShip) {
		int32_t																							particleIndex													= shipInstances[iShip].ParticleIndex;
		const ::ftwlib::SCoord2<float>																	particlePosition												= particleEngine.Particle[particleIndex].Position;
		::game::SParticle2<float>																		& particle														= particleEngine.Particle[particleIndex];
		if( particle.Position.x < 0 || particle.Position.x >= screenAscii.Width
		 || particle.Position.y < 0 || particle.Position.y >= screenAscii.Height				
		 ) continue;
		const int32_t																					linearIndex														= (int32_t)particlePosition.y * screenAscii.Width + (int32_t)particlePosition.x;
		screenAscii.Characters	[linearIndex]														= 1 + gameInstance.DefinitionsShip[shipInstances[iShip].ShipDescription].Image;
		screenAscii.Colors		[linearIndex]														= ::ftwlib::ASCII_COLOR_YELLOW;
	}				
	return 0; 
}
