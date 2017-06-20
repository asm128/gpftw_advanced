#include "game.h"

// initialize game map
::ftwlib::error_t													setupMap							(::game::SGame& gameObject) 														{
	gameObject.Map.Size.x												= game::MAP_WIDTH; // Set a proper width for our map, which has to be less than MAX_MAP_WIDTH
	gameObject.Map.Size.y												= game::MAP_DEPTH; // Same for map depth   

	for( uint32_t z = 0; z < gameObject.Map.Size.y; ++z ) {		// iterate over every row
		for( uint32_t x = 0; x < gameObject.Map.Size.x; ++x ) {		// iterate over every column for the z row
			gameObject.Map.Floor.Cells[z][x]									= TILE_GRASS;	
			gameObject.Map.Enemy.Cells[z][x]									= INVALID_ENEMY;
			gameObject.Map.Shots.Cells[z][x]									= INVALID_SHOT;
		}
	}

	// set a wall border
	for( uint32_t x = 0; x < gameObject.Map.Size.x; ++x ) {
		gameObject.Map.Floor.Cells[0][x]									= TILE_WALL; // set all cells in the first row [0]   
		gameObject.Map.Floor.Cells[gameObject.Map.Size.y - 1][x]			= TILE_WALL; // set all cells in the last row [depth-1]
	}
	for( uint32_t z = 0; z < gameObject.Map.Size.y; ++z ) {
		gameObject.Map.Floor.Cells[z][0]									= TILE_WALL; // set all cells for the first column [0]   
		gameObject.Map.Floor.Cells[z][gameObject.Map.Size.x - 1]			= TILE_WALL; // set all cells for the last column [width-1]
	}

	return 0;
};

// Use this function to setup player at level startup.
::ftwlib::error_t													setupPlayer							(::game::SGame& gameObject)															{
	// set some initial configuration to the player character
	gameObject.Player.CurrentPoints										= { 100, 20, 0 }; 
	gameObject.Player.Position.x										= 5;
	gameObject.Player.Position.y										= 5;
	gameObject.Player.PositionDeltas									= {0,0};
	gameObject.Player.Direction											= 0.0f;
	gameObject.Player.Speed												= 5.5f;
	gameObject.Player.Action											= ::game::ACTION_STAND;
	return 0;
}

// Use this function to setup enemy list at level startup.
::ftwlib::error_t													setupEnemies						(::game::SGame& gameObject)															{
#define INITIAL_ENEMY_COUNT 5
	for( int iEnemy = 0; iEnemy < INITIAL_ENEMY_COUNT; ++iEnemy ) {
		::game::SCharacter														newEnemy;
		newEnemy.MaxPoints													= { 10 + 10 * iEnemy, 50, 1000000 }; // HP, MP and XP
		newEnemy.CurrentPoints												= { 10 + 10 * iEnemy, 50, 0 };
		newEnemy.Position													= { ::rand() % (int32_t)gameObject.Map.Size.x, ::rand() % (int32_t)gameObject.Map.Size.y };
		newEnemy.PositionDeltas												= {0, 0};
		newEnemy.Direction													= 0.0f;
		newEnemy.Speed														= float(iEnemy); // generate the speed from the enemy index
		newEnemy.Action														= ::game::ACTION_WALK;

		gameObject.Enemy.push_back( newEnemy ); // copy the new enemy as a new element at the end of our enemy list.
	}
	return 0;
}

::ftwlib::error_t													game::cleanup						(::game::SGame& /*gameObject*/)														{ return 0; }
::ftwlib::error_t													game::setup							(::game::SGame& gameObject)															{
	setupMap		(gameObject);
	setupPlayer		(gameObject);
	setupEnemies	(gameObject); // setup enemies in list
	return 0;
}

