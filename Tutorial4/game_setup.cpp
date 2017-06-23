#include "game.h"

#include "ftw_size.h"
#include "ftw_ascii_color.h"

// initialize game map
::ftwlib::error_t															setupWorldMap													(::game::SGame& gameObject) 														{
	gameObject.Map.Size.x														= game::MAP_WIDTH; // Set a proper width for our map, which has to be less than MAX_MAP_WIDTH
	gameObject.Map.Size.y														= game::MAP_DEPTH; // Same for map depth   

	for( uint32_t z = 0; z < gameObject.Map.Size.y; ++z ) {		// iterate over every row
		for( uint32_t x = 0; x < gameObject.Map.Size.x; ++x ) {		// iterate over every column for the z row
			gameObject.Map.Floor.Cells[z][x]											= ::game::TILE_TYPE_GRASS;	
			gameObject.Map.Enemy.Cells[z][x]											= ::game::CHARACTER_TYPE_INVALID;
			gameObject.Map.Shots.Cells[z][x]											= ::game::SHOT_TYPE_INVALID;
		}
	}

	// set a wall border
	for( uint32_t x = 0; x < gameObject.Map.Size.x; ++x ) {
		gameObject.Map.Floor.Cells[0][x]											= ::game::TILE_TYPE_WALL; // set all cells in the first row [0]   
		gameObject.Map.Floor.Cells[gameObject.Map.Size.y - 1][x]					= ::game::TILE_TYPE_WALL; // set all cells in the last row [depth-1]
	}
	for( uint32_t z = 0; z < gameObject.Map.Size.y; ++z ) {
		gameObject.Map.Floor.Cells[z][0]											= ::game::TILE_TYPE_WALL; // set all cells for the first column [0]   
		gameObject.Map.Floor.Cells[z][gameObject.Map.Size.x - 1]					= ::game::TILE_TYPE_WALL; // set all cells for the last column [width-1]
	}

	return 0;
};

// Use this function to setup player at level startup.
::ftwlib::error_t															setupWorldPlayer												(::game::SGame& gameObject)															{
	// set some initial configuration to the player character
	gameObject.Player.CurrentPoints												= { 100, 20, 0 }; 
	gameObject.Player.Position.x												= 5;
	gameObject.Player.Position.y												= 5;
	gameObject.Player.PositionDeltas											= {0,0};
	gameObject.Player.Direction													= 0.0f;
	gameObject.Player.Speed														= 5.5f;
	gameObject.Player.Action													= ::game::ACTION_STAND;
	return 0;
}

// Use this function to setup enemy list at level startup.
::ftwlib::error_t															setupWorldEnemies												(::game::SGame& gameObject)															{
	::game::SCharacter																newEnemy														= {};
	for( uint32_t iEnemy = 0; iEnemy < ::game::CHARACTER_TYPE_COUNT; ++iEnemy ) {	
		newEnemy																	= gameObject.Descriptions.Enemy[iEnemy];
		newEnemy.Position															= { ::rand() % (int32_t)gameObject.Map.Size.x, ::rand() % (int32_t)gameObject.Map.Size.y };
		gameObject.Enemy.push_back( newEnemy ); // copy the new enemy as a new element at the end of our enemy list.
	}
	return 0;
}

::ftwlib::error_t															setupDescriptionsMap											(::game::SGame& gameObject)														{ 
	static ::game::STileFloor														descriptionsTableTileFloor	[::game::TILE_TYPE_COUNT		]	= {};
	static ::game::STileASCII														imageTableTileFloor			[::game::TILE_TYPE_COUNT		]	= {};
	
#define SET_FLOOR_TILE_RECORD(_tileType, _shotThrough, _transitable, _name)	\
	descriptionsTableTileFloor	[_tileType].ShotThrough		= _shotThrough;	\
	descriptionsTableTileFloor	[_tileType].Transitable		= _transitable;	\
	descriptionsTableTileFloor	[_tileType].Name			= _name;

	SET_FLOOR_TILE_RECORD(::game::TILE_TYPE_WALL	, 0, 0, "Wall"	);
	SET_FLOOR_TILE_RECORD(::game::TILE_TYPE_GRASS	, 1, 1, "Grass"	);
	SET_FLOOR_TILE_RECORD(::game::TILE_TYPE_WATER	, 1, 0, "Water"	);
	SET_FLOOR_TILE_RECORD(::game::TILE_TYPE_LAVA	, 1, 1, "Lava"	);
	descriptionsTableTileFloor[::game::TILE_TYPE_LAVA].Damage					= 10;
	gameObject.Descriptions.Floor												= {descriptionsTableTileFloor, ::ftwlib::size(descriptionsTableTileFloor)};

	imageTableTileFloor			[::game::TILE_TYPE_GRASS	]					= {'.', ::ftwlib::ASCII_COLOR_GREEN		| (::ftwlib::ASCII_COLOR_DARKGREEN	<< 4)};
	imageTableTileFloor			[::game::TILE_TYPE_WALL		]					= {'#', ::ftwlib::ASCII_COLOR_LIGHTGREY	| (::ftwlib::ASCII_COLOR_DARKGREY	<< 4)};
	imageTableTileFloor			[::game::TILE_TYPE_WATER	]					= {'~', ::ftwlib::ASCII_COLOR_BLUE		| (::ftwlib::ASCII_COLOR_DARKBLUE	<< 4)};
	imageTableTileFloor			[::game::TILE_TYPE_LAVA		]					= {'~', ::ftwlib::ASCII_COLOR_YELLOW	| (::ftwlib::ASCII_COLOR_RED		<< 4)};
	gameObject.Map.Floor.TileDescriptionTable									= {imageTableTileFloor, ::ftwlib::size(imageTableTileFloor)};

	return 0; 
}

::ftwlib::error_t															setupDescriptionsShot											(::game::SGame& gameObject)														{ 
	static ::game::SShot															descriptionsTableShot		[::game::SHOT_TYPE_COUNT		]	= {};
	static ::game::STileASCII														imageTableShot				[::game::SHOT_TYPE_COUNT		]	= {};

	descriptionsTableShot		[::game::SHOT_TYPE_ARROW	].Damage			= 10;
	descriptionsTableShot		[::game::SHOT_TYPE_BULLET	].Damage			= 20;
	descriptionsTableShot		[::game::SHOT_TYPE_DART		].Damage			= 30;
	descriptionsTableShot		[::game::SHOT_TYPE_FLAME	].Damage			= 30;
	descriptionsTableShot		[::game::SHOT_TYPE_COLD		].Damage			= 30;
	descriptionsTableShot		[::game::SHOT_TYPE_ROCK		].Damage			= 30;
	descriptionsTableShot		[::game::SHOT_TYPE_ROCKET	].Damage			= 40;
	gameObject.Descriptions.Shot												= {descriptionsTableShot, ::ftwlib::size(descriptionsTableShot)};

	imageTableShot				[::game::SHOT_TYPE_ARROW	]					= {'\x1A'	, ::ftwlib::ASCII_COLOR_LIGHTGREY	};
	imageTableShot				[::game::SHOT_TYPE_BULLET	]					= {'-'		, ::ftwlib::ASCII_COLOR_WHITE		};
	imageTableShot				[::game::SHOT_TYPE_DART		]					= {'-'		, ::ftwlib::ASCII_COLOR_GREEN		};
	imageTableShot				[::game::SHOT_TYPE_FLAME	]					= {'"'		, ::ftwlib::ASCII_COLOR_RED			};
	imageTableShot				[::game::SHOT_TYPE_COLD		]					= {'o'		, ::ftwlib::ASCII_COLOR_CYAN		};
	imageTableShot				[::game::SHOT_TYPE_ROCK		]					= {'o'		, ::ftwlib::ASCII_COLOR_DARKGREEN	};
	imageTableShot				[::game::SHOT_TYPE_ROCKET	]					= {'\xf'	, ::ftwlib::ASCII_COLOR_DARKCYAN	};
	gameObject.Map.Shots.TileDescriptionTable									= {imageTableShot, ::ftwlib::size(imageTableShot)};
	return 0; 
}

::ftwlib::error_t															setupDescriptionsEnemies										(::game::SGame& gameObject)														{ 
	static ::game::SCharacter														descriptionsTableEnemy		[::game::CHARACTER_TYPE_COUNT	]	= {};
	static ::game::STileASCII														imageTableEnemy				[::game::CHARACTER_TYPE_COUNT	]	= {};

	descriptionsTableEnemy	[::game::CHARACTER_TYPE_SHADOW	].Speed				= 2;
	descriptionsTableEnemy	[::game::CHARACTER_TYPE_SHADOW	].CurrentPoints		=
	descriptionsTableEnemy	[::game::CHARACTER_TYPE_SHADOW	].MaxPoints			= {160, 0, 1};

	descriptionsTableEnemy	[::game::CHARACTER_TYPE_SPEEDY	].Speed				= 3;
	descriptionsTableEnemy	[::game::CHARACTER_TYPE_SPEEDY	].CurrentPoints		=
	descriptionsTableEnemy	[::game::CHARACTER_TYPE_SPEEDY	].MaxPoints			= {80, 0, 1};

	descriptionsTableEnemy	[::game::CHARACTER_TYPE_BASHFUL	].Speed				= 4;
	descriptionsTableEnemy	[::game::CHARACTER_TYPE_BASHFUL	].CurrentPoints		=
	descriptionsTableEnemy	[::game::CHARACTER_TYPE_BASHFUL	].MaxPoints			= {40, 0, 1};

	descriptionsTableEnemy	[::game::CHARACTER_TYPE_POKEY	].Speed				= 5;
	descriptionsTableEnemy	[::game::CHARACTER_TYPE_POKEY	].CurrentPoints		=
	descriptionsTableEnemy	[::game::CHARACTER_TYPE_POKEY	].MaxPoints			= {20, 0, 1};
	gameObject.Descriptions.Enemy												= {descriptionsTableEnemy, ::ftwlib::size(descriptionsTableEnemy)};

	imageTableEnemy			[::game::CHARACTER_TYPE_SHADOW	]					= {'\x1', ::ftwlib::ASCII_COLOR_LIGHTGREY	};
	imageTableEnemy			[::game::CHARACTER_TYPE_SPEEDY	]					= {'\x2', ::ftwlib::ASCII_COLOR_WHITE		};
	imageTableEnemy			[::game::CHARACTER_TYPE_BASHFUL	]					= {'\x1', ::ftwlib::ASCII_COLOR_GREEN		};
	imageTableEnemy			[::game::CHARACTER_TYPE_POKEY	]					= {'\x2', ::ftwlib::ASCII_COLOR_RED			};
	gameObject.Map.Enemy.TileDescriptionTable									= imageTableEnemy;
	return 0; 
} // setup enemies in list

::ftwlib::error_t															game::cleanup													(::game::SGame& /*gameObject*/)													{ return 0; }
::ftwlib::error_t															game::setup														(::game::SGame& gameObject)														{
	::setupDescriptionsMap		(gameObject);
	::setupDescriptionsShot		(gameObject);
	::setupDescriptionsEnemies	(gameObject); // setup enemies in list

	::setupWorldMap				(gameObject);
	::setupWorldPlayer			(gameObject);
	::setupWorldEnemies			(gameObject); // setup enemies in list
	return 0;
}

