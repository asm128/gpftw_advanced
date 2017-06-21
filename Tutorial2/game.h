#include "tilemap.h"
#include "mob.h"

#include "ftw_grid.h"

#include <vector>		// for std::vector

#ifndef GAME_H_90237409238740923749023
#define GAME_H_90237409238740923749023
//-------------------------------------------------------------------------

#define TILE_GRASS			0  // define some values to represent our terrain tiles
#define TILE_WALL			1
#define TILE_WATER			2
#define TILE_LAVA			3

#define INVALID_ENEMY		-1
#define INVALID_SHOT		-1

#define GAME_PI				3.141592653589793238462643383279
#define GAME_2PI			(2 * GAME_PI)
#define GAME_EPSILON		0.00001f

namespace game 
{
	static constexpr		const int32_t				MAP_WIDTH								= 48;
	static constexpr		const int32_t				MAP_DEPTH								= 32;

	struct SMap { // The struct is a block of variables to be used to store our map information
		::ftwlib::SCoord2<uint32_t>							Size;		// Declare Width and Depth variables which will hold the active map size
		::game::STileMapASCII<MAP_WIDTH, MAP_DEPTH>			Floor;
		::game::STileMapASCII<MAP_WIDTH, MAP_DEPTH>			Enemy;
		::game::STileMapASCII<MAP_WIDTH, MAP_DEPTH>			Shots;
	};

	struct SFrameInfo {
		uint32_t											FrameNumber								= 0;
		double												LastFrameTime							= 0;
		double												TotalTime								= 0;
	};

	struct SGame { // holds the game data
		SMap												Map; // declare a variable of type SMap
		SCharacter											Player; // Declare a variable of type SCharacter for the player
		SFrameInfo											FrameInfo;
		::std::vector<SCharacter>							Enemy;	// Enemy list
		::std::vector<SShot>								Shots;		// Shot list
	};

	// -- game functions			
	::ftwlib::error_t									setup									(::game::SGame& gameObject);
	::ftwlib::error_t									cleanup									(::game::SGame& gameObject);
	::ftwlib::error_t									update									(::game::SGame& gameObject, uint64_t timeElapsedMicroseconds);
	::ftwlib::error_t									draw									(const ::game::SGame& gameObject, uint32_t targetWidth, uint8_t* targetCharacterGrid, uint16_t* targetColorGrid);	// take the map data and print it on the console
}
//-------------------------------------------------------------------------
#endif // GAME_H_90237409238740923749023