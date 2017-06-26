#include "tilemap.h"	// for STileMapASCII
#include "character.h"		// for SMovingObject, SCharacter, SShot

#include "ftw_error.h"	// for ::ftwlib::error_t

#include <vector>		// for ::std::vector

#ifndef GAME_H_90237409238740923749023
#define GAME_H_90237409238740923749023
//-------------------------------------------------------------------------

namespace game 
{
#pragma pack (push, 1)
	static constexpr		const int32_t							MAP_WIDTH								= 64;
	static constexpr		const int32_t							MAP_DEPTH								= 32;

	enum TILE_TYPE : uint32_t 
		{	TILE_TYPE_GRASS				= 0  // define some values to represent our terrain tiles
		,	TILE_TYPE_WALL					
		,	TILE_TYPE_WATER					
		,	TILE_TYPE_LAVA					
		,	TILE_TYPE_COUNT
		,	TILE_TYPE_INVALID			= ~0U
		};

	enum CHARACTER_TYPE : uint32_t 
		{	CHARACTER_TYPE_SHADOW		= 0  // define some values to represent our terrain tiles
		,	CHARACTER_TYPE_SPEEDY			 
		,	CHARACTER_TYPE_BASHFUL
		,	CHARACTER_TYPE_POKEY		 
		,	CHARACTER_TYPE_COUNT			
		,	CHARACTER_TYPE_INVALID		= ~0U
		};

	enum SHOT_TYPE : uint32_t 
		{	SHOT_TYPE_ARROW				= 0  // define some values to represent our terrain tiles
		,	SHOT_TYPE_BULLET				 
		,	SHOT_TYPE_DART
		,	SHOT_TYPE_FLAME				 
		,	SHOT_TYPE_COLD
		,	SHOT_TYPE_ROCK	
		,	SHOT_TYPE_ROCKET 
		,	SHOT_TYPE_COUNT				
		,	SHOT_TYPE_INVALID			= ~0U
		};

	struct STileFloor {
		uint64_t														ShotThrough				: 1;
		uint64_t														Transitable				: 1;
		uint64_t														Damage					: 8;
		::std::string													Name					= "Unnamed tile";
	};

	struct SFrameInfo {
		uint32_t														FrameNumber								= 0;
		double															LastFrameTime							= 0;
		double															TotalTime								= 0;
	};
#pragma pack (pop)

	struct SMap { // The struct is a block of variables to be used to store our map information
		::ftwlib::SCoord2<uint32_t>										Size;		// Declare Width and Depth variables which will hold the active map size
		::game::STileMapASCII<::game::MAP_WIDTH, ::game::MAP_DEPTH>		Floor;
		::game::STileMapASCII<::game::MAP_WIDTH, ::game::MAP_DEPTH>		Enemy;
		::game::STileMapASCII<::game::MAP_WIDTH, ::game::MAP_DEPTH>		Shots;
	};

	struct SDescriptionTables { // 
		::ftwlib::array_view<const ::game::SCharacter	>				Enemy;		// 
		::ftwlib::array_view<const ::game::SCharacter	>				Shot;		// 
		::ftwlib::array_view<const ::game::STileFloor	>				Floor;		// 
	};

	struct SGame { // holds the game data
		::game::SMap													Map;		// declare a variable of type SMap
		::game::SCharacter												Player;		// Declare a variable of type SCharacter for the player
		::game::SFrameInfo												FrameInfo;
		::std::vector<SCharacter>										Enemy;				// Enemy list
		::std::vector<SCharacter>										Shots;				// Shot list

		SRigidBodyEngine												RigidBodyEngine;	// Rigid body list

		SDescriptionTables												Descriptions;
	};

	// -- game functions			
	::ftwlib::error_t												setup									(::game::SGame& gameObject);
	::ftwlib::error_t												cleanup									(::game::SGame& gameObject);
	::ftwlib::error_t												update									(::game::SGame& gameObject, uint64_t timeElapsedMicroseconds);
	::ftwlib::error_t												draw									(const ::game::SGame& gameObject, uint32_t targetWidth, uint8_t* targetCharacterGrid, uint16_t* targetColorGrid);	// take the map data and print it on the console
}
//-------------------------------------------------------------------------
#endif // GAME_H_90237409238740923749023