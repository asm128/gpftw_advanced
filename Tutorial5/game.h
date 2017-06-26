#include "tilemap.h"	// for STileMapASCII
#include "mob.h"		// for SMovingObject, SCharacter, SShot

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

	enum TILE_TYPE		: uint32_t 
		{	TILE_TYPE_GRASS				= 0  // define some values to represent our terrain tiles
		,	TILE_TYPE_SOIL					
		,	TILE_TYPE_ASPHALT
		,	TILE_TYPE_COBBLESTONE
		,	TILE_TYPE_WATER					
		,	TILE_TYPE_LAVA					
		,	TILE_TYPE_COUNT					
		,	TILE_TYPE_INVALID			= ~0U
		};

	enum CHARACTER_TYPE	: uint32_t 
		{	CHARACTER_TYPE_HERO			= 0  // define some values to represent our character types
		,	CHARACTER_TYPE_GHOSTLY			
		,	CHARACTER_TYPE_QUICKLY			
		,	CHARACTER_TYPE_BASHEY			
		,	CHARACTER_TYPE_SMILEY			
		,	CHARACTER_TYPE_COUNT			
		,	CHARACTER_TYPE_INVALID		= ~0U
		};

	enum SHOT_TYPE		: uint32_t 
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

	enum PROP_TYPE		: uint32_t 
		{	PROP_TYPE_CHEST				= 0  // define some values to represent our terrain tiles
		,	PROP_TYPE_WALL
		,	PROP_TYPE_DOOR
		,	PROP_TYPE_TORCH
		,	PROP_TYPE_FENCE
		,	PROP_TYPE_COUNT					
		,	PROP_TYPE_INVALID			= ~0U
		};

	struct STileFloor {
		uint64_t														ShotThrough								: 1;
		uint64_t														Transitable								: 1;
		uint64_t														Damage									: 8;
		::std::string													Name									= "Unnamed tile";
	};

	struct SFrameInfo {
		uint64_t														LastFrameMicroseconds					= 0;
		uint64_t														TotalMicroseconds						= 0;
		double															LastFrameSeconds						= 0;
		uint64_t														FrameNumber								= 0;
	};

	struct SMap { // The struct is a block of variables to be used to store our map information
		::ftwlib::SCoord2<uint32_t>										Size									= {};		// Declare Width and Depth variables which will hold the active map size
		::game::STileMapASCII<::game::MAP_WIDTH, ::game::MAP_DEPTH>		Floor									= {};
		::game::STileMapASCII<::game::MAP_WIDTH, ::game::MAP_DEPTH>		Prop									= {};
		::game::STileMapASCII<::game::MAP_WIDTH, ::game::MAP_DEPTH>		Character								= {};
		::game::STileMapASCII<::game::MAP_WIDTH, ::game::MAP_DEPTH>		Shot									= {};
	};

	enum ACTION_INPUT : int32_t	// Defines indices for input actions.
		{	ACTION_INPUT_SALUTE				= 0	// 
		,	ACTION_INPUT_WALK_FRONT				// 
		,	ACTION_INPUT_WALK_BACK				// 
		,	ACTION_INPUT_STRAFE_RIGHT			// 
		,	ACTION_INPUT_STRAFE_LEFT			// 
		,	ACTION_INPUT_TURN_RIGHT				// 
		,	ACTION_INPUT_TURN_LEFT				// 
		,	ACTION_INPUT_CLIMB_UP				// 
		,	ACTION_INPUT_CLIMB_DOWN				// 
		,	ACTION_INPUT_FLY_UP					// 
		,	ACTION_INPUT_FLY_DOWN				// 
		,	ACTION_INPUT_SPRINT					// 
		,	ACTION_INPUT_JUMP					// 
		,	ACTION_INPUT_CROUCH					// 
		,	ACTION_INPUT_SNEAK					// 
		,	ACTION_INPUT_SHOOT					// 
		,	ACTION_INPUT_MELEE					// 
		,	ACTION_INPUT_THROW					// 
		,	ACTION_INPUT_RELOAD					// 
		,	ACTION_INPUT_USEITEM				// 
		,	ACTION_INPUT_USEPROP				// 
		,	ACTION_INPUT_WEAPON_NEXT			// 
		,	ACTION_INPUT_WEAPON_PREV			// 
		,	ACTION_INPUT_ITEM_NEXT				// 
		,	ACTION_INPUT_ITEM_PREV				// 
		,	ACTION_INPUT_COUNT					// 
		,	ACTION_INPUT_ITEM_INVALID		= -1			// 
		};

	enum ACTION_TYPE	// this is pretty much the same as "typedef int ACTION_TYPE;"
		{	ACTION_TYPE_WALK	// means the character has to apply movement during update
		,	ACTION_TYPE_STAND	// means the character is stopped and shouldn't walk on frame update
		};

	struct SCharacterPropertyIndices {
		int32_t															Image									= -1;
		int32_t															Body									= -1;
		int32_t															PointsMax								= -1;
		int32_t															PointsCurrent							= -1;
		int32_t															ActionCurrent							= -1;
		int32_t															ActionInput								= -1;
		int32_t															Team									= -1;
		int32_t															Name									= -1;
	};

	struct SCharacterProperty {
		STileASCII														Image									= {};
		SMovingObject													Body									= {};
		SCharacterPoints												PointsMax								= {};
		SCharacterPoints												PointsCurrent							= {};
		ACTION_TYPE														ActionCurrent							= {};
		int32_t															ActionInput								= {};
		int32_t															Team									= -1;
		::std::string													Name									= {};
	};
#pragma pack (pop)

	struct SCharacterPropertyTable {
		::std::vector<STileASCII		>								Image									= {};
		::std::vector<SMovingObject		>								Body									= {};

		::std::vector<SCharacterPoints	>								PointsMax								= {};
		::std::vector<SCharacterPoints	>								PointsCurrent							= {};

		::std::vector<ACTION_TYPE		>								ActionCurrent							= {};
		::std::vector<int32_t			>								ActionInput								= {};

		::std::vector<int32_t			>								Team									= {};
		::std::vector<::std::string		>								Name									= {};

		// This function/method should fit in no more than 8 lines
		::ftwlib::error_t												AddCharacter							(const SCharacterProperty& characterDescription, SCharacterPropertyIndices& characterInstance)			{
			SCharacterPropertyIndices											newCharacterInstance;					// Ghost writes... I'm aware of it.
			try {	// This try/catch block will be removed as soon as we start using our improved containers, considering the standard ones are barely functional and underperformant.
				newCharacterInstance.Image			= (int32_t)Image			.size();	Image			.push_back(characterDescription.Image			);		
				newCharacterInstance.Body			= (int32_t)Body				.size();	Body			.push_back(characterDescription.Body			);		
				newCharacterInstance.PointsMax		= (int32_t)PointsMax		.size();	PointsMax		.push_back(characterDescription.PointsMax		);		
				newCharacterInstance.PointsCurrent	= (int32_t)PointsCurrent	.size();	PointsCurrent	.push_back(characterDescription.PointsCurrent	);		
				newCharacterInstance.ActionCurrent	= (int32_t)ActionCurrent	.size();	ActionCurrent	.push_back(characterDescription.ActionCurrent	);	
				newCharacterInstance.ActionInput	= (int32_t)ActionInput		.size();	ActionInput		.push_back(characterDescription.ActionInput		);
				newCharacterInstance.Team			= (int32_t)Team				.size();	Team			.push_back(characterDescription.Team			);
				newCharacterInstance.Name			= (int32_t)Name				.size();	Name			.push_back(characterDescription.Name			);
			}
			catch(...) {
				return -1;
			}
			// The pathetic design of ::std::vector doesn't have into consideration that push_back() can actually fail and should return an error value to notify about any, otherwise it could return useful information as the index of the pushed element.
			// Instead of explicitly returning a proper value that you can handle with ease, push_back() will ungracefully crash your program if it wasn't able to succeed unless you add an overwhelming amount of error handling code around every single call to push_back().
			characterInstance												= newCharacterInstance;
			return 0;
		}

		inline	::ftwlib::error_t										SetActionInput							(uint32_t characterInstanceId, ACTION_INPUT input)										{ ActionInput[characterInstanceId] |= (1 << input);										}
		inline	::ftwlib::error_t										GetActionInput							(uint32_t characterInstanceId, ACTION_INPUT input, bool& out_InputState)		const	{ out_InputState = 0 != (ActionInput[characterInstanceId] & (1 << input));	return 0;	}
	};

	struct SDescriptionTables { // 
		::ftwlib::array_view<const ::game::SCharacterProperty	>		Character								= {};		// 
		::ftwlib::array_view<const ::game::SCharacterProperty	>		Shot									= {};		// 
		::ftwlib::array_view<const ::game::STileFloor			>		Floor									= {};		// 
	};

	struct SGame { // holds the game data
		::game::SMap													Map										= {};		// declare a variable of type SMap
		::game::SFrameInfo												FrameInfo								= {};

		int32_t															PlayerCharacterInstance					= -1;
		::std::vector<::game::SCharacterPropertyIndices>				CharacterInstances						= {};
		::game::SCharacterPropertyTable									CharacterPropertyTable					= {};
		::game::SDescriptionTables										Descriptions							= {};
	};

	// -- game functions			
	::ftwlib::error_t												setup									(::game::SGame& gameObject);
	::ftwlib::error_t												cleanup									(::game::SGame& gameObject);
	::ftwlib::error_t												update									(::game::SGame& gameObject, uint64_t timeElapsedMicroseconds);
	::ftwlib::error_t												draw									(const ::game::SGame& gameObject, uint32_t targetWidth, uint8_t* targetCharacterGrid, uint16_t* targetColorGrid);	// take the map data and print it on the console
}
//-------------------------------------------------------------------------
#endif // GAME_H_90237409238740923749023