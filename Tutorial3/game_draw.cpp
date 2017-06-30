#include "game.h"

#include "ftw_ascii_color.h"	// for ::ftwlib::ASCII_COLOR
#include "ftw_size.h"			// for ::ftwlib::size()

#define GAME_EPSILON		::ftwlib::math_epsilon
#define INVALID_CHARACTER		::game::CHARACTER_TYPE_INVALID
#define INVALID_SHOT		::game::SHOT_TYPE_INVALID

// This function prints the game map on the console
void																drawASCIIMap						( const ::game::SGame& gameObject, uint32_t targetWidth, uint8_t* targetCharacterGrid, uint16_t* targetColorGrid )		{
	const ::ftwlib::array_view<const ::game::STileASCII>					floorDescriptionTable				= gameObject.Map.Floor.TileDescriptionTable;
	const ::ftwlib::array_view<const ::game::STileASCII>					enemyDescriptionTable				= gameObject.Map.Enemy.TileDescriptionTable;
	const ::ftwlib::array_view<const ::game::STileASCII>					shotDescriptionTable				= gameObject.Map.Shots.TileDescriptionTable;

	for( uint32_t z = 0; z < gameObject.Map.Size.y; z++ ) { // iterate over every row
		for( uint32_t x = 0; x < gameObject.Map.Size.x; x++ ) { // iterate over every column for the z row
			::ftwlib::ASCII_COLOR													backgroundColor						=::ftwlib::ASCII_COLOR(floorDescriptionTable[gameObject.Map.Floor.Cells[z][x]].Color & 0xF0);
			uint32_t																linearIndex							= z * targetWidth + x;	// The position where we shuold position our next character
			int32_t																	enemyDescriptionIndex				= gameObject.Map.Enemy.Cells[z][x];
			int32_t																	shotDescriptionIndex				= gameObject.Map.Shots.Cells[z][x];
			if( enemyDescriptionIndex != INVALID_CHARACTER ) {
				if( shotDescriptionIndex != INVALID_SHOT ) {
					targetCharacterGrid	[linearIndex]									= '@'; // draw the enemy as an ascii character
					targetColorGrid		[linearIndex]									= ::ftwlib::ASCII_COLOR_LIGHTGREY | backgroundColor;
				}
				else {
					targetCharacterGrid	[linearIndex]									= enemyDescriptionTable[enemyDescriptionIndex].Character; // draw the enemy as an ascii character
					targetColorGrid		[linearIndex]									= enemyDescriptionTable[enemyDescriptionIndex].Color | backgroundColor;
				}
			}
			else if( shotDescriptionIndex != INVALID_SHOT ) {
				const ::game::SShot														* shot								= &gameObject.Shots[shotDescriptionIndex];
				::game::SVector2														dirVector							= ::game::SVector2{1, 0}.Rotate( shot->Direction );

				targetColorGrid		[linearIndex]									= ::ftwlib::ASCII_COLOR_RED | backgroundColor;
				if( (dirVector.x < (-GAME_EPSILON)	&& dirVector.y < (-GAME_EPSILON))
				 || (dirVector.x >   GAME_EPSILON	&& dirVector.y >   GAME_EPSILON )
				 ) 
					targetCharacterGrid	[linearIndex]									= '\\'; // draw the shot as an ascii character
				else if(( dirVector.x < (-GAME_EPSILON)	&& dirVector.y >   GAME_EPSILON)
				 ||		( dirVector.x >   GAME_EPSILON	&& dirVector.y < (-GAME_EPSILON))
				 )
					targetCharacterGrid	[linearIndex]									= '/'; // draw the shot as an ascii character
				else
					targetCharacterGrid	[linearIndex]									= ( dirVector.x < (-GAME_EPSILON) || dirVector.x > GAME_EPSILON ) ? '-' : '|'; // draw the shot as an ascii character
			}
			else {
				int32_t																	tileIndex							= gameObject.Map.Floor.Cells[z][x];
				targetCharacterGrid	[linearIndex]									= floorDescriptionTable[tileIndex].Character; // draw the enemy as an ascii character
				targetColorGrid		[linearIndex]									= floorDescriptionTable[tileIndex].Color;
			}
		}
	}

	// We draw the player outside the loop.
	::game::STileCoord2														playerPosition						= gameObject.Player.Position;
	uint32_t																linearIndex							= playerPosition.y * targetWidth + playerPosition.x;
	::ftwlib::ASCII_COLOR													backgroundColor						=::ftwlib::ASCII_COLOR(floorDescriptionTable[gameObject.Map.Floor.Cells[playerPosition.y][playerPosition.x]].Color & 0xF0);
	targetCharacterGrid	[linearIndex]									= 'P';	// draw the player as an ascii character
	targetColorGrid		[linearIndex]									= ::ftwlib::ASCII_COLOR_GREEN | backgroundColor;
}

// This function prints miscelaneous game info
void																drawASCIIGameInfo					(const ::game::SGame& gameObject, uint32_t targetWidth, uint8_t* targetCharacterGrid, uint16_t *targetColorGrid)													{
	::game::SVector2														dirVector							= ::game::SVector2{1, 0}.Rotate(gameObject.Player.Direction);	// Calculate other ways of representing the direction
	float																	degrees								= (float)(gameObject.Player.Direction / ::ftwlib::math_2pi * 360.0);

	bool																	playerDead							= gameObject.Player.CurrentPoints.HP <= 0;
	char																	compositeText	[1024]				= {};
	uint16_t																compositeColors	[1024]				;
	uint16_t																backgroundColor						= ::ftwlib::ASCII_COLOR_YELLOW | ((playerDead ? ::ftwlib::ASCII_COLOR_DARKRED : ::ftwlib::ASCII_COLOR_BLUE) << 4);
	for(uint32_t i = 0; i < ::ftwlib::size(compositeColors); ++i) 
		compositeColors[i]													= backgroundColor;

	uint32_t																offsetCell							= (::game::MAP_DEPTH * targetWidth);
	const ::game::SCharacter												& player							= gameObject.Player;	

#define COPY_TO_TARGET()															\
		::memcpy(&targetCharacterGrid[offsetCell], compositeText, textLength); 		\
		::memcpy(&targetColorGrid[offsetCell], compositeColors, textLength * 2); 	\
		offsetCell															+= targetWidth;

	int32_t																	
	textLength	= ::sprintf_s(compositeText, "- Player health: %i, Mana: %i, Experience: %i."							, player.CurrentPoints.HP, player.CurrentPoints.MP, player.CurrentPoints.XP					); COPY_TO_TARGET();
	textLength	= ::sprintf_s(compositeText, "- Player position: (%i, %i), deltas: (%f, %f)."							, player.Position.x, player.Position.y, player.PositionDeltas.x, player.PositionDeltas.y	); COPY_TO_TARGET();
	textLength	= ::sprintf_s(compositeText, "- Player direction: (%f, %f). Player angle: %f radians or %f degrees."	, dirVector.x, dirVector.y, (float)player.Direction, degrees								); COPY_TO_TARGET();
	textLength	= ::sprintf_s(compositeText, "- Enemy count: %u, Shot count: %u."										, (uint32_t)gameObject.Enemy.size(), (uint32_t)gameObject.Shots.size()						); COPY_TO_TARGET();
	textLength	= ::sprintf_s(compositeText, "- Shoot with Space key. Run by holding LEFT SHIFT while moving."																										); COPY_TO_TARGET();
	textLength	= ::sprintf_s(compositeText, "- Move (P)layer by pressing the arrow keys to prevent being touched by enemies E, F, G and H."																		); COPY_TO_TARGET();
}

// Use this function to draw our game data
::ftwlib::error_t													game::draw							(const ::game::SGame& gameObject, uint32_t targetWidth, uint8_t* targetCharacterGrid, uint16_t *targetColorGrid)				{	// Accepts an address of an SGame instance
	::drawASCIIMap( gameObject, targetWidth, targetCharacterGrid, targetColorGrid );

	bool																	playerDead							= gameObject.Player.CurrentPoints.HP <= 0;
	bool																	playerWon							= gameObject.Enemy.size() == 0;
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
