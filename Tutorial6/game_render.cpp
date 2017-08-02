#include "game.h"
#include "ftw_ascii_color.h"

template <typename _tElem>
			int32_t																			renderParticle													(::ftwl::SScreenASCII& screenAscii, const ::ftwl::SParticle2Engine<_tElem>& particleEngine, int32_t particleIndex, uint8_t image, uint16_t color)				{
	const ::ftwl::SCoord2<_tElem>																	particlePosition												= particleEngine.Particle[particleIndex].Position;
	const ::ftwl::SParticle2<_tElem>																& particle														= particleEngine.Particle[particleIndex];
	::ftwl::SCoord2<int32_t>																		screenPosition													= (particleEngine.Particle[particleIndex].Position + ::ftwl::SCoord2<_tElem>{.5f, .5f}).Cast<int32_t>();
	screenPosition.y																			/= 2;
	if( particle.Position.x < 0 || screenPosition.x >= (int32_t)screenAscii.Width
	 || particle.Position.y < 0 || screenPosition.y >= (int32_t)screenAscii.Height				
	 ) return 1;
	const int32_t																					linearIndex														= (int32_t)screenPosition.y * screenAscii.Width + (int32_t)screenPosition.x;
	screenAscii.Characters	[linearIndex]														= image;
	screenAscii.Colors		[linearIndex]														= color | (screenAscii.Colors[linearIndex] & 0xF0);
	return 0;
}
//			::ftwl::error_t																renderFrameInfo													(::ftwl::SScreenASCII& screenAscii)	{ return 0; }

// This function copies the text to the target screen and sets the background color for the copied characters.
static inline	::ftwl::error_t															printToScreen													(::ftwl::SScreenASCII & screenAscii, const ::ftwl::SCoord2<uint32_t>& offset, const ::ftwl::array_view<const char>& text, const ::ftwl::ASCII_COLOR color) {
	::memcpy(&screenAscii.Characters[offset.y * screenAscii.Width + offset.x], text.begin(), text.size());
	for(uint32_t iCell = 0, cellCount = text.size(); iCell < cellCount; ++iCell)	{ 
		uint32_t																						cellIndex														= offset.y * screenAscii.Width + offset.x + iCell; 
		screenAscii.Colors[cellIndex]																= (::ftwl::ASCII_COLOR)(color | (screenAscii.Colors[cellIndex] & 0xF0)); 
	}
	return 0;
}

template<typename... _argsToPrint>
static			::ftwl::error_t															printfToScreen													(::ftwl::SScreenASCII & screenAscii, const ::ftwl::SCoord2<uint32_t>& offset, const ::ftwl::ASCII_COLOR color, const char* format, _argsToPrint&&... argsToPrint) {
	char																							formatted[64]													= {};
	::sprintf_s(formatted, format, argsToPrint...); 
	::printToScreen(screenAscii, offset, {formatted, (uint32_t)::strlen(formatted)}, color);
	return 0;
}

template<typename _tElem>
static		::ftwl::error_t																renderPlayerInfo												
	(	::ftwl::SScreenASCII				& screenAscii
	,	const ::game::SShip					& shipInstance			
	,	const ::game::SShipDescription		& shipDescription	
	,	const ::game::SShotDescription		& shotDescription
	,	const ::ftwl::SParticle2<_tElem>	& shipParticle
	) 
{
						const ::ftwl::SCoord2<uint32_t>										offset																= {1, screenAscii.Height - 9}; 
	static constexpr	const ::ftwl::ASCII_COLOR												color																= ::ftwl::ASCII_COLOR_YELLOW;
	static constexpr	const char	text0[]	= "Player ship          : %u."		; ::printfToScreen(screenAscii, {offset.x, offset.y + 0U}, color, text0, shipInstance.ShipDescription											);
	static constexpr	const char	text1[]	= "Player health        : %u / %u."	; ::printfToScreen(screenAscii, {offset.x, offset.y + 1U}, color, text1, shipInstance.PointsCurrent.Health, shipDescription.PointsMax.Health	);
	static constexpr	const char	text2[]	= "Player shield        : %u / %u."	; ::printfToScreen(screenAscii, {offset.x, offset.y + 2U}, color, text2, shipInstance.PointsCurrent.Shield, shipDescription.PointsMax.Shield	);
	static constexpr	const char	text3[]	= "Player speed         : %f."		; ::printfToScreen(screenAscii, {offset.x, offset.y + 3U}, color, text3, (float)shipParticle.Forces.Velocity.Length()							);
	static constexpr	const char	text4[]	= "Player weapon        : %u."		; ::printfToScreen(screenAscii, {offset.x, offset.y + 4U}, color, text4, shipInstance.SelectedShot												);
	static constexpr	const char	text5[]	= "Player weapon damage : %u."		; ::printfToScreen(screenAscii, {offset.x, offset.y + 5U}, color, text5, shotDescription.Damage													);
	static constexpr	const char	text6[]	= "Player weapon rounds : %u / %u."	; ::printfToScreen(screenAscii, {offset.x, offset.y + 6U}, color, text6, shipInstance.RoundsCurrent, shotDescription.RoundsMax					);
	static constexpr	const char	text7[]	= "Player weapon speed  : %u."		; ::printfToScreen(screenAscii, {offset.x, offset.y + 7U}, color, text7, shotDescription.Speed													);
	return 0; 
}
//
			::ftwl::error_t																game::render													(SGame& gameInstance, ::ftwl::SScreenASCII& screenAscii)									{ 
	::std::vector<::game::SShip>																	& shipInstances													= gameInstance.Ships;
	::std::vector<::game::SShot>																	& shotInstances													= gameInstance.Shots;
	::std::vector<::game::SEffect>																	& effectInstances												= gameInstance.Effects;
	::ftwl::SParticle2Engine<float>																& particleEngine												= gameInstance.ParticleEngineGame;

	for(uint32_t iCell = 0, cellCount = screenAscii.Colors.size(); iCell < cellCount; ++iCell)	// reset every cell's background color 
		screenAscii.Colors[iCell]																	= ::ftwl::ASCII_COLOR_DARKBLUE << 4;
	{ // Render GUI
		const SShip																						& playerShipInstance											= shipInstances[0];
		const SShotDescription																			& playerShotDescription											= gameInstance.DefinitionsShot[playerShipInstance.SelectedShot];
		const SShipDescription																			& playerShipDescription											= gameInstance.DefinitionsShip[playerShipInstance.ShipDescription];
		const ::ftwl::SParticle2<float>																& playerShipParticle											= particleEngine.Particle[playerShipInstance.ParticleIndex];
		//::renderFrameInfo	(screenAscii, );
		::renderPlayerInfo	(screenAscii, playerShipInstance, playerShipDescription, playerShotDescription, playerShipParticle);
	}
	for(uint32_t iEffect = 0; iEffect < effectInstances.size(); ++iEffect) {	// render effects
		const SEffect																						& effectInstance												= effectInstances[iEffect];
		const SEffectDescription																			& effectDescription												= gameInstance.DefinitionsEffect[effectInstance.Type];
		::renderParticle(screenAscii, gameInstance.ParticleEngineEffects, effectInstance.ParticleIndex, effectDescription.Image, effectDescription.Color);
	}

	for(uint32_t iShot = 0; iShot < shotInstances.size(); ++iShot) {	// render shots
		const SShot																						& shotInstance													= shotInstances[iShot];
		const SShotDescription																			& shotDescription												= gameInstance.DefinitionsShot[shotInstance.ShotDescription];
		::renderParticle(screenAscii, particleEngine, shotInstance.ParticleIndex, shotDescription.Image, shotDescription.Color);
	}

	for(uint32_t iShip = 0; iShip < shipInstances.size(); ++iShip) {	// render ships
		if(shipInstances[iShip].Unused)
			continue;
		const SShip																						& shipInstance													= shipInstances[iShip];
		const SShipDescription																			& shipDescription												= gameInstance.DefinitionsShip[shipInstance.ShipDescription];
		::renderParticle(screenAscii, particleEngine, shipInstance.ParticleIndex, shipDescription.Image, shipDescription.Color);
	}				

	return 0; 
}
