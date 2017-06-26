#include "game.h"

#include "ftw_ascii_color.h"	// for ::ftwlib::ASCII_COLOR
#include "ftw_size.h"			// for ::ftwlib::size()

#define GAME_EPSILON		::ftwlib::math_epsilon
#define INVALID_CHARACTER		::game::CHARACTER_TYPE_INVALID
#define INVALID_SHOT		::game::SHOT_TYPE_INVALID

// This function prints the game map on the console
void																drawASCIIMap						( const ::game::SGame& gameObject, uint32_t targetWidth, uint8_t* targetCharacterGrid, uint16_t* targetColorGrid )		{
	const ::ftwlib::array_view<const ::game::STileASCII>					floorDescriptionTable				= gameObject.Map.Floor.TileDescriptionTable;
	const ::ftwlib::array_view<const ::game::STileASCII>					enemyDescriptionTable				= gameObject.Map.Character.TileDescriptionTable;
	const ::ftwlib::array_view<const ::game::STileASCII>					shotDescriptionTable				= gameObject.Map.Shot.TileDescriptionTable;
	
	for( uint32_t z = 0; z < gameObject.Map.Size.y; z++ ) { // iterate over every row
		for( uint32_t x = 0; x < gameObject.Map.Size.x; x++ ) { // iterate over every column for the z row
			::ftwlib::ASCII_COLOR													backgroundColor						=::ftwlib::ASCII_COLOR(floorDescriptionTable[gameObject.Map.Floor.Cells[z][x]].Color & 0xF0);
			uint32_t																linearIndex							= z * targetWidth + x;	// The position where we shuold position our next character
			int32_t																	enemyDescriptionIndex				= gameObject.Map.Character.Cells[z][x];
			int32_t																	shotDescriptionIndex				= gameObject.Map.Shot.Cells[z][x];
			if( enemyDescriptionIndex != INVALID_CHARACTER ) {
				if( shotDescriptionIndex != INVALID_SHOT ) {
					targetCharacterGrid	[linearIndex]									= '@'; // draw the enemy as an ascii character
					targetColorGrid		[linearIndex]									= ::ftwlib::ASCII_COLOR_LIGHTGREY | backgroundColor;
				}
				else {
					const int32_t															tileDescriptionIndex				= gameObject.Map.Character.Cells[z][x];
					targetCharacterGrid	[linearIndex]									= enemyDescriptionTable[tileDescriptionIndex].Character; // draw the enemy as an ascii character
					targetColorGrid		[linearIndex]									= enemyDescriptionTable[tileDescriptionIndex].Color | backgroundColor;
				}
			}
			else if( shotDescriptionIndex != INVALID_SHOT ) {
				const int32_t															tileDescriptionIndex				= gameObject.Map.Shot.Cells[z][x];
				targetCharacterGrid	[linearIndex]									= enemyDescriptionTable[tileDescriptionIndex].Character; // draw the enemy as an ascii character
				targetColorGrid		[linearIndex]									= enemyDescriptionTable[tileDescriptionIndex].Color | backgroundColor;
			}
			else {
				const int32_t															tileDescriptionIndex				= gameObject.Map.Floor.Cells[z][x];
				targetCharacterGrid	[linearIndex]									= floorDescriptionTable[tileDescriptionIndex].Character; // draw the enemy as an ascii character
				targetColorGrid		[linearIndex]									= floorDescriptionTable[tileDescriptionIndex].Color;
			}
		}
	}
}

// This function prints miscelaneous game info
void																drawASCIIGameInfo					(const ::game::SGame& gameObject, uint32_t targetWidth, uint8_t* targetCharacterGrid, uint16_t *targetColorGrid)													{
	int32_t																	playerCharacterIndex				= gameObject.PlayerCharacterInstance;
	if(INVALID_CHARACTER == playerCharacterIndex) {
		return;
	}
		
	const ::game::SCharacterPropertyIndices									& playerCharacterInstance			= gameObject.CharacterInstances						[playerCharacterIndex];

	// Calculate other ways of representing the direction
	const ::game::SMovingObject												& playerBody						= gameObject.CharacterPropertyTable.Body				[playerCharacterInstance.Body];
	::game::SVector2														dirVector							= ::game::SVector2{1, 0}.Rotate(playerBody.Direction);
	float																	degrees								= (float)(playerBody.Direction / ::ftwlib::math_2pi * 360.0);
	// 
	const ::game::SCharacterPoints											& playerPoints						= gameObject.CharacterPropertyTable.PointsCurrent	[playerCharacterInstance.PointsCurrent];
	const ::game::SCharacterPoints											& playerPointsMax					= gameObject.CharacterPropertyTable.PointsMax		[playerCharacterInstance.PointsMax];
	bool																	playerDead							= playerPoints.HP <= 0;
	char																	compositeText	[1024]				= {};
	uint16_t																compositeColors	[1024]				;
	uint16_t																backgroundColor						= ::ftwlib::ASCII_COLOR_YELLOW | ((playerDead ? ::ftwlib::ASCII_COLOR_DARKRED : ::ftwlib::ASCII_COLOR_BLUE) << 4);
	for(uint32_t i = 0; i < ::ftwlib::size(compositeColors); ++i) 
		compositeColors[i]													= backgroundColor;

	uint32_t																offsetCell							= (::game::MAP_DEPTH * targetWidth);

#define COPY_TO_TARGET()																\
		::memcpy(&targetCharacterGrid	[offsetCell], compositeText, textLength); 		\
		::memcpy(&targetColorGrid		[offsetCell], compositeColors, textLength * 2);	\
		offsetCell															+= targetWidth;


	const ::game::SEntityCoord												& playerPosition					= playerBody.Position;
	int32_t																	
	textLength	= ::sprintf_s(compositeText, "- Player health: %i/%u, Mana: %i/%u, Experience: %i, Damage: %i."			, playerPoints.HP, playerPointsMax.HP, playerPoints.MP, playerPointsMax.MP, playerPoints.XP, playerPoints.DP				); COPY_TO_TARGET();
	textLength	= ::sprintf_s(compositeText, "- Player position: (%i, %i), deltas: (%f, %f)."							, playerPosition.Tile.x, playerPosition.Tile.y, playerBody.Position.Deltas.x, playerBody.Position.Deltas.y					); COPY_TO_TARGET();
	textLength	= ::sprintf_s(compositeText, "- Player direction: (%f, %f). Player angle: %f radians or %f degrees."	, dirVector.x, dirVector.y, (float)playerBody.Direction, degrees															); COPY_TO_TARGET();
	textLength	= ::sprintf_s(compositeText, "- Character count: %u, Shot count: %u."									, (uint32_t)gameObject.Map.Character.TileDescriptionTable.size(), (uint32_t)gameObject.Map.Shot.TileDescriptionTable.size()	); COPY_TO_TARGET();
	textLength	= ::sprintf_s(compositeText, "- Shoot with Space key. Run by holding LEFT SHIFT while moving."																																		); COPY_TO_TARGET();
	textLength	= ::sprintf_s(compositeText, "- Move (P)layer by pressing the arrow keys to prevent being touched by enemies E, F, G and H."																										); COPY_TO_TARGET();
}

// Use this function to draw our game data
::ftwlib::error_t													game::draw							(const ::game::SGame& gameObject, uint32_t targetWidth, uint8_t* targetCharacterGrid, uint16_t *targetColorGrid)				{	// Accepts an address of an SGame instance
	::drawASCIIMap( gameObject, targetWidth, targetCharacterGrid, targetColorGrid );

	int32_t																	playerCharacterIndex				= gameObject.PlayerCharacterInstance;
	const ::game::SCharacterPropertyIndices									& playerCharacterInstance			= gameObject.CharacterInstances						[playerCharacterIndex];
	const ::game::SCharacterPoints											& playerPoints						= gameObject.CharacterPropertyTable.PointsCurrent	[playerCharacterInstance.PointsCurrent];
	bool																	playerDead							= playerPoints.HP <= 0;
	bool																	playerWon							= gameObject.Map.Character.TileDescriptionTable.size() == 0;
	const ::std::string														winText								= playerWon ? "You Win!" : playerDead ? "Game Over!" : "";
	char																	compositeText	[32]				= {};
	uint16_t																compositeColors	[32]				;
	uint16_t																backgroundColor						= ::ftwlib::ASCII_COLOR_YELLOW | ((playerDead ? ::ftwlib::ASCII_COLOR_DARKRED : ::ftwlib::ASCII_COLOR_BLUE) << 4);
	for(uint32_t i = 0; i < ::ftwlib::size(compositeColors); ++i) 
		compositeColors[i]													= backgroundColor;

	int32_t																	textLength							= ::sprintf_s(compositeText, winText.c_str());
	uint32_t																offsetCell							= (::game::MAP_DEPTH / 2 * targetWidth) + (::game::MAP_WIDTH / 2 - textLength / 2);
	COPY_TO_TARGET();
	
	::drawASCIIGameInfo( gameObject, targetWidth, targetCharacterGrid, targetColorGrid );
	return 0;
}