#include "game.h"

#include "ftw_ascii_color.h"

				void																		setupEffects													(::game::SGame& gameInstance)							{
	gameInstance.DefinitionsEffect			[::game::EFFECT_TYPE_ROCKET			]				= {'.', ::ftwl::ASCII_COLOR_RED		};
	gameInstance.DefinitionsEffect			[::game::EFFECT_TYPE_STARS			]				= {'.', ::ftwl::ASCII_COLOR_WHITE		};
}	
				void																		setupShips														(::game::SGame& gameInstance)							{
	gameInstance.DefinitionsShip			[::game::SHIP_TYPE_FASTER			]				= { 70, 100, 232	, ::ftwl::ASCII_COLOR_WHITE		};
	gameInstance.DefinitionsShip			[::game::SHIP_TYPE_STRONGER			]				= {200, 100, 21		, ::ftwl::ASCII_COLOR_MAGENTA		};
	gameInstance.DefinitionsShip			[::game::SHIP_TYPE_BETTER			]				= {150, 100, 238	, ::ftwl::ASCII_COLOR_CYAN		};
	gameInstance.DefinitionsShip			[::game::SHIP_TYPE_GOOD_LOOKING		]				= { 90, 100, 228	, ::ftwl::ASCII_COLOR_YELLOW		};
	gameInstance.DefinitionsShip			[::game::SHIP_TYPE_NICER			]				= { 80, 100, 67		, ::ftwl::ASCII_COLOR_GREEN		};
	gameInstance.DefinitionsShip			[::game::SHIP_TYPE_BULLY			]				= {170, 100, 'F'	, ::ftwl::ASCII_COLOR_YELLOW		};
	gameInstance.DefinitionsShip			[::game::SHIP_TYPE_STALKER			]				= {250, 100, 'T'	, ::ftwl::ASCII_COLOR_LIGHTGREY	};
	gameInstance.DefinitionsShip			[::game::SHIP_TYPE_POLITE			]				= {100, 100, 'E'	, ::ftwl::ASCII_COLOR_CYAN		};
}																													
				void																		setupShots														(::game::SGame& gameInstance)							{
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_ROCK				]				= {10, 100,  20 * .5f,   7	, ::ftwl::ASCII_COLOR_DARKGREY		};
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_ARROW			]				= {20, 100,  70 * .5f,  26	, ::ftwl::ASCII_COLOR_LIGHTGREY		};
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_FIREBALL			]				= {30, 100,  50 * .5f,  15	, ::ftwl::ASCII_COLOR_RED			};
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_LASER			]				= {40, 100, 150 * .5f, 196	, ::ftwl::ASCII_COLOR_RED			};
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_BULLET			]				= {40, 100, 130 * .5f, 196	, ::ftwl::ASCII_COLOR_LIGHTGREY		};
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_POISON			]				= {50, 100,  30 * .5f, 237	, ::ftwl::ASCII_COLOR_GREEN			};
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_PLASMA			]				= {60, 100, 120 * .5f, 247	, ::ftwl::ASCII_COLOR_CYAN			};
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_BOMB				]				= {70, 100,  10 * .5f, 162	, ::ftwl::ASCII_COLOR_DARKRED		};
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_GRAVITATIONAL	]				= {70, 100,  10 * .5f, 162	, ::ftwl::ASCII_COLOR_DARKMAGENTA	};
}
				void																		setupParticleShots												(::game::SGame& gameInstance)							{
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_ROCK				].SetMass			(1);
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_ARROW			].SetMass			(1);
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_FIREBALL			].SetMass			(1);
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_LASER			].SetMass			(1);
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_BULLET			].SetMass			(1);
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_POISON			].SetMass			(1);
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_PLASMA			].SetMass			(1);
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_BOMB				].SetMass			(1);
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_GRAVITATIONAL	].SetMass			(1);

	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_ROCK				].Damping			= 0.8f;
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_ARROW			].Damping			= 0.9f;
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_FIREBALL			].Damping			= 0.98f;
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_LASER			].Damping			= 1.0f;
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_BULLET			].Damping			= 0.995f;
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_POISON			].Damping			= 0.7f;
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_PLASMA			].Damping			= 1.0f;
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_BOMB				].Damping			= 0.6f;
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_GRAVITATIONAL	].Damping			= 0.6f;
}

static			void																		setupParticleShips												(::game::SGame& gameInstance)							{
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_FASTER			].SetMass		( 3);
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_STRONGER			].SetMass		( 6);
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_BETTER			].SetMass		( 4);
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_GOOD_LOOKING		].SetMass		( 5);
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_NICER			].SetMass		( 2);
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_BULLY			].SetMass		( 7);
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_STALKER			].SetMass		( 8);
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_POLITE			].SetMass		( 9);

	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_FASTER			].Damping		= 0.3f;
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_STRONGER			].Damping		= 0.5f;
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_BETTER			].Damping		= 0.7f;
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_GOOD_LOOKING		].Damping		= 0.4f;
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_NICER			].Damping		= 0.6f;
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_BULLY			].Damping		= 0.2f;
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_STALKER			].Damping		= 0.1f;
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_POLITE			].Damping		= 0.9f;

}

static			void																		setupParticleEffects											(::game::SGame& gameInstance)							{
	gameInstance.DefinitionsParticleEffect	[::game::EFFECT_TYPE_ROCKET		].SetMass			( 1);
	gameInstance.DefinitionsParticleEffect	[::game::EFFECT_TYPE_STARS		].SetMass			( 1);

	gameInstance.DefinitionsParticleEffect	[::game::EFFECT_TYPE_ROCKET		].Damping			= 1.0f;
	gameInstance.DefinitionsParticleEffect	[::game::EFFECT_TYPE_STARS		].Damping			= 1.0f;

	gameInstance.DefinitionsParticleEffect	[::game::EFFECT_TYPE_ROCKET		].Forces.Velocity	= {-20.0f, 0};
	gameInstance.DefinitionsParticleEffect	[::game::EFFECT_TYPE_STARS		].Forces.Velocity	= {-20.0f, 0};
}

static inline	void																		setupParticles													(::game::SGame& gameInstance)							{
	::setupParticleShips	(gameInstance);
	::setupParticleShots	(gameInstance);
	::setupParticleEffects	(gameInstance);
}
				::ftwl::error_t															game::setup														(::game::SGame& gameInstance)							{
	::setupParticles	(gameInstance);
	::setupShips		(gameInstance);
	::setupShots		(gameInstance);
	::setupEffects		(gameInstance);

	gameInstance.Spawners.resize(100);
	for(uint32_t iSpawner = 0, spawnerCount = (uint32_t)gameInstance.Spawners.size(); iSpawner < spawnerCount; ++ iSpawner) {
		::game::SSpawner																				& spawner														= gameInstance.Spawners[iSpawner];
		spawner.Position																			= {(int32_t)gameInstance.CombatAreaSizeVisible.x - 3, (int32_t)((iSpawner * 10) % gameInstance.CombatAreaSizeVisible.y)};
		spawner.Records.resize(100);
		for(uint32_t iSpawnRecord = 0; iSpawnRecord < spawner.Records.size(); ++iSpawnRecord) {
			spawner.Records[iSpawnRecord].ShipTypeToSpawn												= ::game::SHIP_TYPE(iSpawner % ::game::SHIP_TYPE_COUNT);
			spawner.Records[iSpawnRecord].TimeSinceGameStarted											= 5 + (iSpawnRecord * 100 + 1) + (iSpawner);
		}
	}
	::game::addShip(gameInstance, ::game::SHIP_TYPE_FASTER);
	gameInstance.Ships[0].SelectedShot															= ::game::SHOT_TYPE_ROCK; 
	gameInstance.ParticleEngineGame.Particle[gameInstance.Ships[0].ParticleIndex].Position			= {gameInstance.CombatAreaSizeVisible.x * .25f, gameInstance.CombatAreaSizeVisible.y * .5f}; 
	return 0;
}
