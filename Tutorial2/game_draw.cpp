#include "game.h"
#include "gpftw_ascii_color.h"

#include <stdio.h>

// This function prints the game map on the console
void																drawASCIIMap						( const ::game::SGame& gameObject, uint32_t targetWidth, uint8_t* targetCharacterGrid, uint16_t* targetColorGrid )		{
	// 
	for( uint32_t z = 0; z < gameObject.Map.Size.y; z++ ) { // iterate over every row
		for( uint32_t x = 0; x < gameObject.Map.Size.x; x++ ) { // iterate over every column for the z row
			uint32_t																linearIndex							= z * targetWidth + x;	// The position where we shuold position our next character
			if( gameObject.Player.Position.x == (int32_t)x && gameObject.Player.Position.y == (int32_t)z ) {
				targetCharacterGrid	[linearIndex]											= 'P'; // draw the player as an ascii character
				targetColorGrid		[linearIndex]											= ::ftwlib::ASCII_COLOR_INDEX_10;
			}
			else if( gameObject.Map.Enemy.Cells[z][x] != INVALID_ENEMY ) {
				if( gameObject.Map.Shots.Cells[z][x] != INVALID_SHOT ) {
					targetCharacterGrid	[linearIndex]											= '@'; // draw the enemy as an ascii character
					targetColorGrid		[linearIndex]											= ::ftwlib::ASCII_COLOR_INDEX_6;
				}
				else {
					targetCharacterGrid	[linearIndex]											= char('E' + gameObject.Map.Enemy.Cells[z][x]); // draw the enemy as an ascii character
					targetColorGrid		[linearIndex]											= ::ftwlib::ASCII_COLOR_INDEX_4;
				}
			}
			else if( gameObject.Map.Shots.Cells[z][x] != INVALID_SHOT ) {
				const ::game::SShot														* shot								= &gameObject.Shots[gameObject.Map.Shots.Cells[z][x]];
				::game::SVector2														dirVector							= ::game::SVector2{1, 0}.Rotate( shot->Direction );

				if( ( dirVector.x < (-GAME_EPSILON) && dirVector.y < (-GAME_EPSILON) )
					|| ( dirVector.x > GAME_EPSILON && dirVector.y > GAME_EPSILON )
					) {
					targetCharacterGrid	[linearIndex]											= '\\'; // draw the shot as an ascii character
					targetColorGrid		[linearIndex]											= ::ftwlib::ASCII_COLOR_INDEX_12;
				}
				else if( ( dirVector.x < (-GAME_EPSILON) && dirVector.y > GAME_EPSILON )
					|| ( dirVector.x > GAME_EPSILON && dirVector.y < (-GAME_EPSILON) )
					) {
					targetCharacterGrid	[linearIndex]											= '/'; // draw the shot as an ascii character
					targetColorGrid		[linearIndex]											= ::ftwlib::ASCII_COLOR_INDEX_12;
				}
				else if( dirVector.x < (-GAME_EPSILON) || dirVector.x > GAME_EPSILON ) {
					targetCharacterGrid[linearIndex]											= '-'; // draw the shot as an ascii character
					targetColorGrid		[linearIndex]											= ::ftwlib::ASCII_COLOR_INDEX_12;
				}
				else {
					targetCharacterGrid[linearIndex]											= '|'; // draw the shot as an ascii character
					targetColorGrid		[linearIndex]											= ::ftwlib::ASCII_COLOR_INDEX_12;
				}
			}
			else {
				targetCharacterGrid[linearIndex]											= char(gameObject.Map.Floor.Cells[z][x] ? gameObject.Map.Floor.Cells[z][x] : ' '); // draw the tile as an ascii character
				targetColorGrid		[linearIndex]											= ::ftwlib::ASCII_COLOR_INDEX_0;
			}
		}
	}
}

// This function prints miscelaneous game info
void																drawASCIIGameInfo					( const ::game::SGame& /*gameObject*/ )													{
	// Calculate another ways of representing the direction
	//::game::SVector2														dirVector							= ::game::SVector2{1, 0}.Rotate(gameObject.Player.Direction);
	//float																	degrees								= (float)(gameObject.Player.Direction / GAME_2PI * 360.0);

	//printf( "- Player health: %i, Mana: %i, Experience: %i\n"
	//		"- Player position: (%i, %i), deltas: (%f, %f)\n"
	//		"- Player direction: (%f, %f)\n- Player angle: %f radians or %f degrees\n"
	//		"- Enemy count: %u, Shot count: %u\n"
	//		"Shoot with Space key. Run by holding LEFT SHIFT while moving.\nMove (P)layer by pressing the arrow keys to prevent being touched by enemies E, F, G and H."
	//	, gameObject.Player.CurrentPoints.HP
	//	, gameObject.Player.CurrentPoints.MP
	//	, gameObject.Player.CurrentPoints.XP
	//	, gameObject.Player.Position.x
	//	, gameObject.Player.Position.y
	//	, gameObject.Player.PositionDeltas.x
	//	, gameObject.Player.PositionDeltas.y
	//	, dirVector.x
	//	, dirVector.y
	//	, gameObject.Player.Direction
	//	, degrees
	//	, (unsigned int)gameObject.Enemy.size()
	//	, (unsigned int)gameObject.Shots.size() 
	//);
}

// Use this function to draw our game data
::ftwlib::error_t															game::draw														(const ::game::SGame& gameObject, uint32_t targetWidth, uint8_t* targetCharacterGrid, uint16_t *targetColorGrid)				{	// Accepts an address of an SGame instance

	::drawASCIIMap( gameObject, targetWidth, targetCharacterGrid, targetColorGrid );

	bool																			playerDead														= gameObject.Player.CurrentPoints.HP <= 0;
	const ::std::string																winText															= gameObject.Enemy.size() == 0 ? "You Win!" 
		: playerDead ? "Game Over!" : ""
		;
	char																			compositeText	[32]											= {};
	uint16_t																		compositeColors	[32]											;
	for(uint32_t i = 0; i < 32; ++i) {
		compositeColors[i]															= ::ftwlib::ASCII_COLOR_INDEX_14 | ((playerDead ? ::ftwlib::ASCII_COLOR_INDEX_12 : ::ftwlib::ASCII_COLOR_INDEX_2) << 4);
	}

	int32_t																			textLength														= ::sprintf_s(compositeText, winText.c_str());
	uint32_t																		offsetCell														= (::game::MAP_DEPTH / 2 * targetWidth) + (::game::MAP_WIDTH / 2 - textLength / 2);
	::memcpy(&targetCharacterGrid	[offsetCell], compositeText		, textLength	);
	::memcpy(&targetColorGrid		[offsetCell], compositeColors	, textLength * 2);
	
	::drawASCIIGameInfo( gameObject );
	return 0;
}