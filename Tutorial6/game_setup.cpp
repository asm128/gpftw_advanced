#include "game.h"

#include "ftw_ascii_color.h"

				void																		setupShips														(::game::SGame& gameInstance)							{
	gameInstance.DefinitionsShip			[::game::SHIP_TYPE_FASTER		]					= {100, 100, 232	, ::ftwlib::ASCII_COLOR_WHITE		};
	gameInstance.DefinitionsShip			[::game::SHIP_TYPE_STRONGER		]					= {100, 100, 21		, ::ftwlib::ASCII_COLOR_MAGENTA		};
	gameInstance.DefinitionsShip			[::game::SHIP_TYPE_BETTER		]					= {100, 100, 238	, ::ftwlib::ASCII_COLOR_CYAN		};
	gameInstance.DefinitionsShip			[::game::SHIP_TYPE_GOOD_LOOKING	]					= {100, 100, 228	, ::ftwlib::ASCII_COLOR_YELLOW		};
	gameInstance.DefinitionsShip			[::game::SHIP_TYPE_NICER		]					= {100, 100, 67		, ::ftwlib::ASCII_COLOR_GREEN		};
}																													
				void																		setupShots														(::game::SGame& gameInstance)							{
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_ROCK			]					= {10, 100, 20 * .5f,   7	, ::ftwlib::ASCII_COLOR_DARKGREY	};
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_ARROW		]					= {10, 100, 50 * .5f,  26	, ::ftwlib::ASCII_COLOR_LIGHTGREY	};
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_FIREBALL		]					= {10, 100, 40 * .5f,  15	, ::ftwlib::ASCII_COLOR_RED			};
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_LASER		]					= {10, 100, 70 * .5f, 196	, ::ftwlib::ASCII_COLOR_RED			};
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_POISON		]					= {10, 100, 30 * .5f, 237	, ::ftwlib::ASCII_COLOR_GREEN		};
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_PLASMA		]					= {10, 100, 60 * .5f, 247	, ::ftwlib::ASCII_COLOR_CYAN		};
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_BOMB			]					= {10, 100, 10 * .5f, 162	, ::ftwlib::ASCII_COLOR_DARKRED		};
}
				void																		setupParticleShots												(::game::SGame& gameInstance)							{
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_ROCK			].SetMass			(1);
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_ARROW		].SetMass			(1);
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_FIREBALL		].SetMass			(1);
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_LASER		].SetMass			(1);
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_POISON		].SetMass			(1);
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_PLASMA		].SetMass			(1);
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_BOMB			].SetMass			(1);

	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_ROCK			].Damping			= 0.8f;
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_ARROW		].Damping			= 0.9f;
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_FIREBALL		].Damping			= 0.98f;
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_LASER		].Damping			= 1.0f;
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_POISON		].Damping			= 0.7f;
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_PLASMA		].Damping			= 1.0f;
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_BOMB			].Damping			= 0.6f;
}

static			void																		setupParticleShips												(::game::SGame& gameInstance)							{
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_FASTER		].SetMass			( 2);
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_STRONGER		].SetMass			( 3);
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_BETTER		].SetMass			( 1);
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_GOOD_LOOKING	].SetMass			( 1);
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_NICER		].SetMass			( 1);

	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_FASTER		].Damping			= 0.9f;
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_STRONGER		].Damping			= 0.5f;
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_BETTER		].Damping			= 0.7f;
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_GOOD_LOOKING	].Damping			= 0.4f;
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_NICER		].Damping			= 0.6f;
}

static inline	void																		setupParticles													(::game::SGame& gameInstance)							{
	::setupParticleShips(gameInstance);
	::setupParticleShots(gameInstance);
}
				::ftwlib::error_t															game::setup														(::game::SGame& gameInstance)							{
	//gameInstance																				= {};
	::setupParticles	(gameInstance);
	::setupShips		(gameInstance);
	::setupShots		(gameInstance);

	::game::addShip(gameInstance, ::game::SHIP_TYPE_GOOD_LOOKING);
	gameInstance.Ships[0].SelectedShot															= ::game::SHOT_TYPE_ROCK; 
	gameInstance.ParticleEngine.Particle[gameInstance.Ships[0].ParticleIndex].Position			= {gameInstance.CombatAreaSizeVisible.x * .25f, gameInstance.CombatAreaSizeVisible.y * .5f}; 
	return 0;
}
