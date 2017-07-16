#include "game.h"

#include "ftw_ascii_color.h"

				void																		setupShips														(::game::SGame& gameInstance)							{
	gameInstance.DefinitionsShip			[::game::SHIP_TYPE_FASTER		]					= {100, 100, 231	, };
	gameInstance.DefinitionsShip			[::game::SHIP_TYPE_STRONGER		]					= {100, 100, 20		, };
	gameInstance.DefinitionsShip			[::game::SHIP_TYPE_BETTER		]					= {100, 100, 237	, };
	gameInstance.DefinitionsShip			[::game::SHIP_TYPE_GOOD_LOOKING	]					= {100, 100, 227	, };
	gameInstance.DefinitionsShip			[::game::SHIP_TYPE_NICER		]					= {100, 100, 66		, };
}																													
				void																		setupShots														(::game::SGame& gameInstance)							{
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_ROCK			]					= {10, 100, 1,   6	, ::ftwlib::ASCII_COLOR_DARKGREY	};
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_ARROW		]					= {10, 100, 1,  25	, ::ftwlib::ASCII_COLOR_LIGHTGREY	};
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_FIREBALL		]					= {10, 100, 1,  14	, ::ftwlib::ASCII_COLOR_RED			};
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_LASER		]					= {10, 100, 1, 195	, ::ftwlib::ASCII_COLOR_RED			};
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_POISON		]					= {10, 100, 1, 236	, ::ftwlib::ASCII_COLOR_GREEN		};
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_PLASMA		]					= {10, 100, 1, 246	, ::ftwlib::ASCII_COLOR_CYAN		};
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_BOMB			]					= {10, 100, 1, 161	, ::ftwlib::ASCII_COLOR_DARKRED		};
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

	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_ROCK			].Forces.Velocity	= {(float)20, (float)(0)};
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_ARROW		].Forces.Velocity	= {(float)50, (float)(0)};
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_FIREBALL		].Forces.Velocity	= {(float)40, (float)(0)};
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_LASER		].Forces.Velocity	= {(float)70, (float)(0)};
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_POISON		].Forces.Velocity	= {(float)30, (float)(0)};
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_PLASMA		].Forces.Velocity	= {(float)60, (float)(0)};
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_BOMB			].Forces.Velocity	= {(float)10, (float)(0)};
}

static			void																		setupParticleShips												(::game::SGame& gameInstance)							{
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_FASTER		].SetMass			( 2);
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_STRONGER		].SetMass			( 3);
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_BETTER		].SetMass			( 1);
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_GOOD_LOOKING	].SetMass			( 1);
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_NICER		].SetMass			( 1);

	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_FASTER		].Damping			= 1.0f;
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_STRONGER		].Damping			= 1.0f;
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_BETTER		].Damping			= 1.0f;
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_GOOD_LOOKING	].Damping			= 1.0f;
	gameInstance.DefinitionsParticleShip	[::game::SHIP_TYPE_NICER		].Damping			= 1.0f;
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

	addShip(gameInstance, ::game::SHIP_TYPE_GOOD_LOOKING);
	gameInstance.Ships[0].SelectedShot															= ::game::SHOT_TYPE_ROCK; 
	gameInstance.ParticleEngine.Particle[gameInstance.Ships[0].ParticleIndex].Position			= {gameInstance.CombatAreaSizeVisible.x * .25f, gameInstance.CombatAreaSizeVisible.y * .5f}; 
	return 0;
}
