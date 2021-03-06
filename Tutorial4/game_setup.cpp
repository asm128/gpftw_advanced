#include "game.h"

#include "ftw_size.h"
#include "ftw_ascii_color.h"

// initialize game map
::ftwl::error_t															setupWorldMap													(::game::SGame& gameObject) 														{
	gameObject.Map.Size.x														= ::game::MAP_WIDTH;	// Set a proper width for our map, which has to be less than MAX_MAP_WIDTH.
	gameObject.Map.Size.y														= ::game::MAP_DEPTH;	// Same for map depth.

	for( uint32_t z = 0; z < gameObject.Map.Size.y; ++z ) {		// iterate over every row
		for( uint32_t x = 0; x < gameObject.Map.Size.x; ++x ) {		// iterate over every column for the z row
			gameObject.Map.Floor.Cells[z][x]											= ::game::TILE_TYPE_GRASS;	
			gameObject.Map.Enemy.Cells[z][x]											= ::game::CHARACTER_TYPE_INVALID;
			gameObject.Map.Shots.Cells[z][x]											= ::game::SHOT_TYPE_INVALID;
		}
	}

	// set a wall border
	for( uint32_t x = 0; x < gameObject.Map.Size.x; ++x ) {
		gameObject.Map.Floor.Cells[0][x]											= ::game::TILE_TYPE_WALL;	// set all cells in the first row [0]   
		gameObject.Map.Floor.Cells[gameObject.Map.Size.y - 1][x]					= ::game::TILE_TYPE_WALL;	// set all cells in the last row [depth-1]
	}
	for( uint32_t z = 0; z < gameObject.Map.Size.y; ++z ) {
		gameObject.Map.Floor.Cells[z][0]											= ::game::TILE_TYPE_WALL;	// set all cells for the first column [0]   
		gameObject.Map.Floor.Cells[z][gameObject.Map.Size.x - 1]					= ::game::TILE_TYPE_WALL;	// set all cells for the last column [width-1]
	}

	return 0;
}

// Use this function to setup player at level startup.
::ftwl::error_t															setupWorldPlayer												(::game::SGame& gameObject)															{
	// set some initial configuration to the player character
	::game::SParticle																playerBody														= {};
	playerBody.Position.Tile.x													= 5;
	playerBody.Position.Tile.y													= 5;
	playerBody.Position.Deltas													= {0,0};

	gameObject.Player.DirectionInRadians										= 0.0f;
	gameObject.Player.Speed														= 0.0f;
	gameObject.Player.SpeedMax													= 15.0f;
	gameObject.Player.Particle													= gameObject.ParticleEngine.AddParticle(playerBody);
	gameObject.Player.PointsMax													= 
	gameObject.Player.PointsCurrent												= { 100, 20, 0, 15 }; 
	gameObject.Player.ActionActive												= {};
	::memset(gameObject.Player.ActionDirection, 0, sizeof(::game::SDirection) * ::ftwl::size(gameObject.Player.ActionDirection));
	return 0;
}

// Use this function to setup enemy list at level startup.
::ftwl::error_t															setupWorldEnemies												(::game::SGame& gameObject)															{
	::game::SCharacter																newEnemy														= {};
	for( uint32_t iEnemy = 0; iEnemy < ::game::CHARACTER_TYPE_COUNT; ++iEnemy ) {	
		newEnemy																	= gameObject.Descriptions.Enemy[iEnemy];
		::game::SParticle																enemyBody														= {};
		
		enemyBody.Position															= { ::rand() % (int32_t)gameObject.Map.Size.x, ::rand() % (int32_t)gameObject.Map.Size.y };
		newEnemy.Speed																= iEnemy;
		newEnemy.Particle															= gameObject.ParticleEngine.AddParticle(enemyBody);

		gameObject.Enemy.push_back( newEnemy ); // copy the new enemy as a new element at the end of our enemy list.
	}
	return 0;
}

::ftwl::error_t															setupDescriptionsMap											(::game::SGame& gameObject)															{ 
	static ::game::STileFloor														descriptionsTableTileFloor	[::game::TILE_TYPE_COUNT		]	= {};
	static ::game::STileASCII														imageTableTileFloor			[::game::TILE_TYPE_COUNT		]	= {};
	
#define SET_FLOOR_TILE_RECORD(_tileType, _shotThrough, _transitable, _name)						\
	descriptionsTableTileFloor	[_tileType].ShotThrough							= _shotThrough;	\
	descriptionsTableTileFloor	[_tileType].Transitable							= _transitable;	\
	descriptionsTableTileFloor	[_tileType].Name								= _name;

	SET_FLOOR_TILE_RECORD(::game::TILE_TYPE_WALL	, 0, 0, "Wall"	);
	SET_FLOOR_TILE_RECORD(::game::TILE_TYPE_GRASS	, 1, 1, "Grass"	);
	SET_FLOOR_TILE_RECORD(::game::TILE_TYPE_WATER	, 1, 0, "Water"	);
	SET_FLOOR_TILE_RECORD(::game::TILE_TYPE_LAVA	, 1, 1, "Lava"	);
	gameObject.Descriptions.Floor												= {descriptionsTableTileFloor, ::ftwl::size(descriptionsTableTileFloor)};

	imageTableTileFloor			[::game::TILE_TYPE_GRASS	]					= {'.', ::ftwl::ASCII_COLOR_GREEN		| (::ftwl::ASCII_COLOR_DARKGREEN	<< 4)};
	imageTableTileFloor			[::game::TILE_TYPE_WALL		]					= {'#', ::ftwl::ASCII_COLOR_LIGHTGREY	| (::ftwl::ASCII_COLOR_DARKGREY	<< 4)};
	imageTableTileFloor			[::game::TILE_TYPE_WATER	]					= {'~', ::ftwl::ASCII_COLOR_BLUE		| (::ftwl::ASCII_COLOR_DARKBLUE	<< 4)};
	imageTableTileFloor			[::game::TILE_TYPE_LAVA		]					= {'~', ::ftwl::ASCII_COLOR_YELLOW	| (::ftwl::ASCII_COLOR_RED		<< 4)};
	gameObject.Map.Floor.TileDescriptionTable									= {imageTableTileFloor, ::ftwl::size(imageTableTileFloor)};

	return 0; 
}

::ftwl::error_t															setupDescriptionsShot											(::game::SGame& gameObject)															{ 
	static ::game::SCharacter														descriptionsTableShot		[::game::SHOT_TYPE_COUNT		]	= {};
	static ::game::STileASCII														imageTableShot				[::game::SHOT_TYPE_COUNT		]	= {};

	descriptionsTableShot		[::game::SHOT_TYPE_ARROW	].PointsCurrent.DP	= 10;
	descriptionsTableShot		[::game::SHOT_TYPE_BULLET	].PointsCurrent.DP	= 20;
	descriptionsTableShot		[::game::SHOT_TYPE_DART		].PointsCurrent.DP	= 30;
	descriptionsTableShot		[::game::SHOT_TYPE_FLAME	].PointsCurrent.DP	= 30;
	descriptionsTableShot		[::game::SHOT_TYPE_COLD		].PointsCurrent.DP	= 30;
	descriptionsTableShot		[::game::SHOT_TYPE_ROCK		].PointsCurrent.DP	= 30;
	descriptionsTableShot		[::game::SHOT_TYPE_ROCKET	].PointsCurrent.DP	= 40;
	gameObject.Descriptions.Shot												= {descriptionsTableShot, ::ftwl::size(descriptionsTableShot)};

	imageTableShot				[::game::SHOT_TYPE_ARROW	]					= {'\x1A'	, ::ftwl::ASCII_COLOR_LIGHTGREY	};
	imageTableShot				[::game::SHOT_TYPE_BULLET	]					= {'-'		, ::ftwl::ASCII_COLOR_WHITE		};
	imageTableShot				[::game::SHOT_TYPE_DART		]					= {'-'		, ::ftwl::ASCII_COLOR_GREEN		};
	imageTableShot				[::game::SHOT_TYPE_FLAME	]					= {'"'		, ::ftwl::ASCII_COLOR_RED			};
	imageTableShot				[::game::SHOT_TYPE_COLD		]					= {'o'		, ::ftwl::ASCII_COLOR_CYAN		};
	imageTableShot				[::game::SHOT_TYPE_ROCK		]					= {'o'		, ::ftwl::ASCII_COLOR_DARKGREEN	};
	imageTableShot				[::game::SHOT_TYPE_ROCKET	]					= {'\xf'	, ::ftwl::ASCII_COLOR_DARKCYAN	};
	gameObject.Map.Shots.TileDescriptionTable									= {imageTableShot, ::ftwl::size(imageTableShot)};

	return 0; 
}

::ftwl::error_t															setupDescriptionsEnemies										(::game::SGame& gameObject)															{ 
	static ::game::SParticle														descriptionsTableParticle	[::game::CHARACTER_TYPE_COUNT	]	= {};
	static ::game::SCharacter														descriptionsTableEnemy		[::game::CHARACTER_TYPE_COUNT	]	= {};
	static ::game::STileASCII														imageTableEnemy				[::game::CHARACTER_TYPE_COUNT	]	= {};

#define SET_CHARACTER_RECORD(_charType, _speed, _maxPoints)											\
	descriptionsTableEnemy		[_charType].Speed									= _speed;		\
	descriptionsTableEnemy		[_charType].PointsCurrent							=				\
	descriptionsTableEnemy		[_charType].PointsMax								= _maxPoints;

	SET_CHARACTER_RECORD(::game::CHARACTER_TYPE_SHADOW	, 2, (::game::SCharacterPoints{160, 0, 1, 10}));
	SET_CHARACTER_RECORD(::game::CHARACTER_TYPE_SPEEDY	, 3, (::game::SCharacterPoints{ 80, 0, 1, 10}));
	SET_CHARACTER_RECORD(::game::CHARACTER_TYPE_BASHFUL	, 4, (::game::SCharacterPoints{ 40, 0, 1, 10}));
	SET_CHARACTER_RECORD(::game::CHARACTER_TYPE_POKEY	, 5, (::game::SCharacterPoints{ 20, 0, 1, 10}));
	gameObject.Descriptions.Enemy												= {descriptionsTableEnemy, ::ftwl::size(descriptionsTableEnemy)};

	imageTableEnemy			[::game::CHARACTER_TYPE_SHADOW	]					= {'\x1', ::ftwl::ASCII_COLOR_LIGHTGREY	};
	imageTableEnemy			[::game::CHARACTER_TYPE_SPEEDY	]					= {'\x2', ::ftwl::ASCII_COLOR_WHITE		};
	imageTableEnemy			[::game::CHARACTER_TYPE_BASHFUL	]					= {'\x1', ::ftwl::ASCII_COLOR_GREEN		};
	imageTableEnemy			[::game::CHARACTER_TYPE_POKEY	]					= {'\x2', ::ftwl::ASCII_COLOR_RED			};
	gameObject.Map.Enemy.TileDescriptionTable									= {imageTableEnemy, ::ftwl::size(imageTableEnemy)};

	return 0; 
} // setup enemies in list

::ftwl::error_t															game::cleanup													(::game::SGame& /*gameObject*/)														{ return 0; }
::ftwl::error_t															game::setup														(::game::SGame& gameObject)															{
	::setupDescriptionsMap		(gameObject);
	::setupDescriptionsShot		(gameObject);
	::setupDescriptionsEnemies	(gameObject); // setup enemies in list

	::setupWorldMap				(gameObject);
	::setupWorldPlayer			(gameObject);
	::setupWorldEnemies			(gameObject); // setup enemies in list
	return 0;
}
