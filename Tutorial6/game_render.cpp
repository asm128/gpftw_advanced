#include "game.h"
#include "ftw_ascii_color.h"

template <typename _tElem>
			int32_t																			renderParticle													(::ftwlib::SScreenASCII& screenAscii, const ::ftwlib::SParticle2Engine<_tElem>& particleEngine, int32_t particleIndex, uint8_t image, uint16_t color)				{
	const ::ftwlib::SCoord2<_tElem>																	particlePosition												= particleEngine.Particle[particleIndex].Position;
	const ::ftwlib::SParticle2<_tElem>																& particle														= particleEngine.Particle[particleIndex];
	::ftwlib::SCoord2<int32_t>																		screenPosition													= (particleEngine.Particle[particleIndex].Position + ::ftwlib::SCoord2<_tElem>{.5f, .5f}).Cast<int32_t>();
	screenPosition.y																			/= 2;
	if( particle.Position.x < 0 || screenPosition.x >= (int32_t)screenAscii.Width
	 || particle.Position.y < 0 || screenPosition.y >= (int32_t)screenAscii.Height				
	 ) return 1;
	const int32_t																					linearIndex														= (int32_t)screenPosition.y * screenAscii.Width + (int32_t)screenPosition.x;
	screenAscii.Characters	[linearIndex]														= image;
	screenAscii.Colors		[linearIndex]														= color | (screenAscii.Colors[linearIndex] & 0xF0);
	return 0;
}
			::ftwlib::error_t																game::render													(SGame& gameInstance, ::ftwlib::SScreenASCII& screenAscii)									{ 
	::std::vector<::game::SShip>																	& shipInstances													= gameInstance.Ships;
	::std::vector<::game::SShot>																	& shotInstances													= gameInstance.Shots;
	::std::vector<::game::SEffect>																	& effectInstances												= gameInstance.Effects;
	::ftwlib::SParticle2Engine<float>																	& particleEngine												= gameInstance.ParticleEngineGame;
	for(uint32_t iCell = 0, cellCount = screenAscii.Colors.size(); iCell < cellCount; ++iCell) {
		screenAscii.Colors[iCell]																	= ::ftwlib::ASCII_COLOR_DARKBLUE << 4;
	}

	for(uint32_t iEffect = 0; iEffect < effectInstances.size(); ++iEffect) {
		const SEffect																						& effectInstance												= effectInstances[iEffect];
		const SEffectDescription																			& effectDescription												= gameInstance.DefinitionsEffect[effectInstance.Type];
		::renderParticle(screenAscii, gameInstance.ParticleEngineEffects, effectInstance.ParticleIndex, effectDescription.Image, effectDescription.Color);
	}
	for(uint32_t iShot = 0; iShot < shotInstances.size(); ++iShot) {
		const SShot																						& shotInstance													= shotInstances[iShot];
		const SShotDescription																			& shotDescription												= gameInstance.DefinitionsShot[shotInstance.ShotDescription];
		::renderParticle(screenAscii, particleEngine, shotInstance.ParticleIndex, shotDescription.Image, shotDescription.Color);
	}
	for(uint32_t iShip = 0; iShip < shipInstances.size(); ++iShip) {
		if(shipInstances[iShip].Unused)
			continue;
		const SShip																						& shipInstance													= shipInstances[iShip];
		const SShipDescription																			& shipDescription												= gameInstance.DefinitionsShip[shipInstance.ShipDescription];
		::renderParticle(screenAscii, particleEngine, shipInstance.ParticleIndex, shipDescription.Image, shipDescription.Color);
	}				
	return 0; 
}