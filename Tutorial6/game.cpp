#include "game.h"
#include "physics_particle.h"

#include "ftw_coord.h"

static ::game::SShipDescription												shipDefinitions			[::game::SHIP_TYPE_COUNT]						= {};
static ::game::SShotDescription												shotDefinitions			[::game::SHOT_TYPE_COUNT]						= {};
static ::game::SParticle2<float>											shipParticleDefinitions	[::game::SHIP_TYPE_COUNT]						= {};
static ::game::SParticle2<float>											shotParticleDefinitions	[::game::SHOT_TYPE_COUNT]						= {};

				void														setupShips														()														{
	shipDefinitions	[::game::SHIP_TYPE_FASTER		]							= {100, 100, 232	};
	shipDefinitions	[::game::SHIP_TYPE_STRONGER		]							= {100, 100, 21		};
	shipDefinitions	[::game::SHIP_TYPE_BETTER		]							= {100, 100, 238	};
	shipDefinitions	[::game::SHIP_TYPE_GOOD_LOOKING	]							= {100, 100, 228	};
	shipDefinitions	[::game::SHIP_TYPE_NICER		]							= {100, 100, 67		};
}
				void														setupShots														()														{
	shotDefinitions	[::game::SHOT_TYPE_ROCK			]							= {10, 100, 1,   7	};
	shotDefinitions	[::game::SHOT_TYPE_ARROW		]							= {10, 100, 1,  26	};
	shotDefinitions	[::game::SHOT_TYPE_FIREBALL		]							= {10, 100, 1,  15	};
	shotDefinitions	[::game::SHOT_TYPE_LASER		]							= {10, 100, 1, 196	};
	shotDefinitions	[::game::SHOT_TYPE_POISON		]							= {10, 100, 1, 237	};
	shotDefinitions	[::game::SHOT_TYPE_PLASMA		]							= {10, 100, 1, 247	};
	shotDefinitions	[::game::SHOT_TYPE_BOMB			]							= {10, 100, 1, 162	};
}
				void														setupParticleShots												()														{
	shotParticleDefinitions	[::game::SHOT_TYPE_ROCK			].SetMass			( 2);
	shotParticleDefinitions	[::game::SHOT_TYPE_ARROW		].SetMass			(-1);
	shotParticleDefinitions	[::game::SHOT_TYPE_FIREBALL		].SetMass			( 1);
	shotParticleDefinitions	[::game::SHOT_TYPE_LASER		].SetMass			( 1);
	shotParticleDefinitions	[::game::SHOT_TYPE_POISON		].SetMass			( 1);
	shotParticleDefinitions	[::game::SHOT_TYPE_PLASMA		].SetMass			( 1);
	shotParticleDefinitions	[::game::SHOT_TYPE_BOMB			].SetMass			( 1);

	shotParticleDefinitions	[::game::SHOT_TYPE_ROCK			].Damping			= 0.80f;
	shotParticleDefinitions	[::game::SHOT_TYPE_ARROW		].Damping			= 0.80f;
	shotParticleDefinitions	[::game::SHOT_TYPE_FIREBALL		].Damping			= 1.0f;
	shotParticleDefinitions	[::game::SHOT_TYPE_LASER		].Damping			= 1.0f;
	shotParticleDefinitions	[::game::SHOT_TYPE_POISON		].Damping			= 1.0f;
	shotParticleDefinitions	[::game::SHOT_TYPE_PLASMA		].Damping			= 1.0f;
	shotParticleDefinitions	[::game::SHOT_TYPE_BOMB			].Damping			= 1.0f;

	shotParticleDefinitions	[::game::SHOT_TYPE_ROCK			].Forces.Velocity	= {};
	shotParticleDefinitions	[::game::SHOT_TYPE_ARROW		].Forces.Velocity	= {(float)0,  (float)(rand() % 40)};
	shotParticleDefinitions	[::game::SHOT_TYPE_FIREBALL		].Forces.Velocity	= {};
	shotParticleDefinitions	[::game::SHOT_TYPE_LASER		].Forces.Velocity	= {};
	shotParticleDefinitions	[::game::SHOT_TYPE_POISON		].Forces.Velocity	= {};
	shotParticleDefinitions	[::game::SHOT_TYPE_PLASMA		].Forces.Velocity	= {};
	shotParticleDefinitions	[::game::SHOT_TYPE_BOMB			].Forces.Velocity	= {(float)0, -(float)(rand() % 20)-20};
}

static			void														setupParticleShips												()														{
	shipParticleDefinitions	[::game::SHIP_TYPE_FASTER		].SetMass			( 2);
	shipParticleDefinitions	[::game::SHIP_TYPE_STRONGER		].SetMass			( 3);
	shipParticleDefinitions	[::game::SHIP_TYPE_BETTER		].SetMass			( 1);
	shipParticleDefinitions	[::game::SHIP_TYPE_GOOD_LOOKING	].SetMass			( 1);
	shipParticleDefinitions	[::game::SHIP_TYPE_NICER		].SetMass			( 1);

	shipParticleDefinitions	[::game::SHIP_TYPE_FASTER		].Damping			= 0.80f;
	shipParticleDefinitions	[::game::SHIP_TYPE_STRONGER		].Damping			= 0.80f;
	shipParticleDefinitions	[::game::SHIP_TYPE_BETTER		].Damping			= 1.0f;
	shipParticleDefinitions	[::game::SHIP_TYPE_GOOD_LOOKING	].Damping			= 1.0f;
	shipParticleDefinitions	[::game::SHIP_TYPE_NICER		].Damping			= 1.0f;

	shipParticleDefinitions	[::game::SHIP_TYPE_FASTER		].Forces.Velocity	= {};
	shipParticleDefinitions	[::game::SHIP_TYPE_STRONGER		].Forces.Velocity	= {(float)0,  (float)(rand() % 40)};
	shipParticleDefinitions	[::game::SHIP_TYPE_BETTER		].Forces.Velocity	= {};
	shipParticleDefinitions	[::game::SHIP_TYPE_GOOD_LOOKING	].Forces.Velocity	= {};
	shipParticleDefinitions	[::game::SHIP_TYPE_NICER		].Forces.Velocity	= {(float)0, -(float)(rand() % 20)-20};
}

static inline	void														setupParticles												()														{
	::setupParticleShips();
	::setupParticleShots();
}
			::ftwlib::error_t												game::addShip												(::game::SGame& gameInstance, ::game::SHIP_TYPE type)	{
	::game::SShip																	newInstance; 
	newInstance.ParticleIndex													= gameInstance.ParticleEngine.AddParticle(shipParticleDefinitions[type]);	
	newInstance.ShipDescription													= type; 
	gameInstance.Ships.push_back(newInstance); 
	return 0;
}
			::ftwlib::error_t												game::addShot												(::game::SGame& gameInstance, ::game::SHOT_TYPE type)	{ 
	::game::SShot																	newInstance; 
	newInstance.ParticleIndex													= gameInstance.ParticleEngine.AddParticle(shotParticleDefinitions[type]);	
	newInstance.ShotDescription													= type; 
	newInstance.RoundsCurrent													= shotDefinitions[type].RoundsMax;
	gameInstance.Shots.push_back(newInstance); 
	return 0; 
}
//				::ftwlib::error_t											game::cleanup												(SGame& gameInstance)									{ return 0; }
				::ftwlib::error_t											game::setup													(SGame& gameInstance)									{
	::setupParticles	();
	::setupShips		();
	::setupShots		();
	return 0;
}
			::ftwlib::error_t												game::update												(SGame& gameInstance, uint64_t lastTimeMicroseconds)	{ 
	::game::SFrameInfo																& frameInfo														= gameInstance.FrameInfo;																	
	frameInfo.Frame(lastTimeMicroseconds);
	const float																		lastFrameSeconds												= (float)frameInfo.Seconds.LastFrame;

	::std::vector<::game::SShip>													& shipInstances													= gameInstance.Ships;
	::std::vector<::game::SShot>													& shotInstances													= gameInstance.Shots;
	::std::vector<SParticleInstance<::game::SHIP_TYPE>>								& shipParticleInstances											= gameInstance.ShipParticleInstances;
	::std::vector<SParticleInstance<::game::SHOT_TYPE>>								& shotParticleInstances											= gameInstance.ShotParticleInstances;
	::game::SParticle2Engine<float>													& particleEngine												= gameInstance.ParticleEngine;
	static float																	windDirection													= 0.1f;

	if(GetAsyncKeyState('1') || GetAsyncKeyState(VK_NUMPAD1)) for(uint32_t i = 0; i < 3; ++i) gameInstance.Ships[0].SelectedShot	= ::game::SHOT_TYPE_ROCK		; 
	if(GetAsyncKeyState('2') || GetAsyncKeyState(VK_NUMPAD2)) for(uint32_t i = 0; i < 3; ++i) gameInstance.Ships[0].SelectedShot	= ::game::SHOT_TYPE_ARROW		; 
	if(GetAsyncKeyState('3') || GetAsyncKeyState(VK_NUMPAD3)) for(uint32_t i = 0; i < 3; ++i) gameInstance.Ships[0].SelectedShot	= ::game::SHOT_TYPE_FIREBALL	; 
	if(GetAsyncKeyState('4') || GetAsyncKeyState(VK_NUMPAD4)) for(uint32_t i = 0; i < 3; ++i) gameInstance.Ships[0].SelectedShot	= ::game::SHOT_TYPE_LASER		; 
	if(GetAsyncKeyState('5') || GetAsyncKeyState(VK_NUMPAD5)) for(uint32_t i = 0; i < 3; ++i) gameInstance.Ships[0].SelectedShot	= ::game::SHOT_TYPE_POISON		; 
	if(GetAsyncKeyState('6') || GetAsyncKeyState(VK_NUMPAD6)) for(uint32_t i = 0; i < 3; ++i) gameInstance.Ships[0].SelectedShot	= ::game::SHOT_TYPE_PLASMA		; 
	if(GetAsyncKeyState('7') || GetAsyncKeyState(VK_NUMPAD7)) for(uint32_t i = 0; i < 3; ++i) gameInstance.Ships[0].SelectedShot	= ::game::SHOT_TYPE_BOMB		; 

	windDirection																= (float)sin(frameInfo.Seconds.Total/5.0f) * .25f;
	particleEngine.Integrate(lastFrameSeconds, frameInfo.Seconds.LastFrameHalfSquared);
	for(uint32_t iShot = 0; iShot < shotInstances.size(); ++iShot) {
		int32_t																			particleIndex													= shotInstances[iShot].ParticleIndex;
		int32_t																			physicsId														= shotParticleInstances[particleIndex].PhysicsId;
		::game::SParticle2<float>														& particleNext													= particleEngine.ParticleNext[physicsId];
		if( particleNext.Position.x < 0 || particleNext.Position.x >= SCREEN_WIDTH
		 || particleNext.Position.y < 0 || particleNext.Position.y >= SCREEN_HEIGHT
		 ) { // Remove the particle instance and related information.
			particleEngine.ParticleState[physicsId].Unused								= true;
			shotParticleInstances	.erase(shotParticleInstances.begin() + particleIndex);
			shotInstances			.erase(shotInstances		.begin() + iShot);
			--iShot;
		}
		else { // Apply forces from wind and gravity.
			static constexpr	const double												gravity															= 9.8;
			::game::SParticle2<float>														& particleCurrent												= particleEngine.Particle[physicsId];
			particleCurrent																= particleEngine.ParticleNext[physicsId];
			particleCurrent.Forces.AccumulatedForce										= {0, float(gravity * lastFrameSeconds)};
			particleCurrent.Forces.AccumulatedForce.x									+= windDirection + float((rand() % 100) / 100.0 - .5);
		}
	}

	for(uint32_t iShip = 0; iShip < shipInstances.size(); ++iShip) {
		int32_t																			particleIndex													= shipInstances[iShip].ParticleIndex;
		int32_t																			physicsId														= shipParticleInstances[particleIndex].PhysicsId;
		::game::SParticle2<float>														& particleNext													= particleEngine.ParticleNext[physicsId];
		if( particleNext.Position.x < 0 || particleNext.Position.x >= SCREEN_WIDTH
		 || particleNext.Position.y < 0 || particleNext.Position.y >= SCREEN_HEIGHT
		 ) { // Remove the particle instance and related information.
			particleEngine.ParticleState[physicsId].Unused								= true;
			shipParticleInstances	.erase(shipParticleInstances.begin() + particleIndex);
			shipInstances			.erase(shipInstances		.begin() + iShip);
			--iShip;
		}
		else { // Apply forces from wind and gravity.
			static constexpr	const double												gravity															= 9.8;
			::game::SParticle2<float>														& particleCurrent												= particleEngine.Particle[physicsId];
			particleCurrent																= particleEngine.ParticleNext[physicsId];
			particleCurrent.Forces.AccumulatedForce										= {0, float(gravity * lastFrameSeconds)};
			particleCurrent.Forces.AccumulatedForce.x									+= windDirection + float((rand() % 100) / 100.0 - .5);
		}
	}
	return 0; 
}
			::ftwlib::error_t												game::render												(SGame& gameInstance)									{ 
	::std::vector<::game::SShip>													& shipInstances													= gameInstance.Ships;
	::std::vector<::game::SShot>													& shotInstances													= gameInstance.Shots;
	::std::vector<SParticleInstance<::game::SHIP_TYPE>>								& shipParticleInstances											= gameInstance.ShipParticleInstances;
	::std::vector<SParticleInstance<::game::SHOT_TYPE>>								& shotParticleInstances											= gameInstance.ShotParticleInstances;
	::game::SParticle2Engine<float>													& particleEngine												= gameInstance.ParticleEngine;

	for(uint32_t iShot = 0; iShot < shotInstances.size(); ++iShot) {
		int32_t																			particleIndex													= shotInstances[iShot].ParticleIndex;
		int32_t																			physicsId														= shotParticleInstances[particleIndex].PhysicsId;
		const ::ftwlib::SCoord2<float>													particlePosition												= gameInstance.ParticleEngine.Particle[physicsId].Position;
		::game::SParticle2<float>														& particle														= particleEngine.Particle[physicsId];
		if( particle.Position.x < 0 || particle.Position.x >= SCREEN_WIDTH
		 || particle.Position.y < 0 || particle.Position.y >= SCREEN_HEIGHT
		 ) continue;
		const int32_t																	linearIndex														= (int32_t)particlePosition.y * screenAscii.Width + (int32_t)particlePosition.x;
		screenAscii.Characters	[linearIndex]										= 1 + shotDefinitions[shotInstances[iShot].ShotDescription].Image;
		//screenAscii.Colors		[linearIndex]	
	}

	for(uint32_t iShip = 0; iShip < shipInstances.size(); ++iShip) {
		int32_t																			particleIndex													= shipInstances[iShip].ParticleIndex;
		int32_t																			physicsId														= shipParticleInstances[particleIndex].PhysicsId;
		const ::ftwlib::SCoord2<float>													particlePosition												= gameInstance.ParticleEngine.Particle[physicsId].Position;
		::game::SParticle2<float>														& particle														= particleEngine.Particle[physicsId];
		if( particle.Position.x < 0 || particle.Position.x >= SCREEN_WIDTH
		 || particle.Position.y < 0 || particle.Position.y >= SCREEN_HEIGHT
		 ) continue;
		const int32_t																	linearIndex														= (int32_t)particlePosition.y * screenAscii.Width + (int32_t)particlePosition.x;
		screenAscii.Characters	[linearIndex]										= 1 + shipDefinitions[shipInstances[iShip].ShipDescription].Image;
		//screenAscii.Colors		[linearIndex]	
	}				
	return 0; 
}

