#include "game.h"

				void																		setupShips														(::game::SGame& gameInstance)							{
	gameInstance.DefinitionsShip			[::game::SHIP_TYPE_FASTER		]					= {100, 100, 232	};
	gameInstance.DefinitionsShip			[::game::SHIP_TYPE_STRONGER		]					= {100, 100, 21		};
	gameInstance.DefinitionsShip			[::game::SHIP_TYPE_BETTER		]					= {100, 100, 238	};
	gameInstance.DefinitionsShip			[::game::SHIP_TYPE_GOOD_LOOKING	]					= {100, 100, 228	};
	gameInstance.DefinitionsShip			[::game::SHIP_TYPE_NICER		]					= {100, 100, 67		};
}
				void																		setupShots														(::game::SGame& gameInstance)							{
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_ROCK			]					= {10, 100, 1,   7	};
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_ARROW		]					= {10, 100, 1,  26	};
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_FIREBALL		]					= {10, 100, 1,  15	};
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_LASER		]					= {10, 100, 1, 196	};
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_POISON		]					= {10, 100, 1, 237	};
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_PLASMA		]					= {10, 100, 1, 247	};
	gameInstance.DefinitionsShot			[::game::SHOT_TYPE_BOMB			]					= {10, 100, 1, 162	};
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
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_FIREBALL		].Damping			= 1.0f;
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_LASER		].Damping			= 1.0f;
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_POISON		].Damping			= 0.7f;
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_PLASMA		].Damping			= 1.0f;
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_BOMB			].Damping			= 0.6f;

	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_ROCK			].Forces.Velocity	= {(float)2, (float)(0)};
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_ARROW		].Forces.Velocity	= {(float)5, (float)(0)};
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_FIREBALL		].Forces.Velocity	= {(float)4, (float)(0)};
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_LASER		].Forces.Velocity	= {(float)7, (float)(0)};
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_POISON		].Forces.Velocity	= {(float)3, (float)(0)};
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_PLASMA		].Forces.Velocity	= {(float)6, (float)(0)};
	gameInstance.DefinitionsParticleShot	[::game::SHOT_TYPE_BOMB			].Forces.Velocity	= {(float)1, (float)(0)};
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

static inline	void																		setupParticles													(::game::SGame& gameInstance)																{
	::setupParticleShips(gameInstance);
	::setupParticleShots(gameInstance);
}
				::ftwlib::error_t															game::setup														(SGame& gameInstance)																		{
	::setupParticles	(gameInstance);
	::setupShips		(gameInstance);
	::setupShots		(gameInstance);

	addShip(gameInstance, ::game::SHIP_TYPE_GOOD_LOOKING);
	return 0;
}
