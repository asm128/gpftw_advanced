#include "game.h"

#include <Windows.h>

void																refreshPosFromDeltas			( ::game::SMovingObject& character )															{
	::game::SVector2													& charDeltas						= character.PositionDeltas; // get pointer to deltas
	
	// Now we increase cell units and decrease deltas until the deltas are between 0 and 0.9999999999999999999999
	// Update X coordinate
	int																		deltaX							= (int)charDeltas.x;
	character .Position.x												+= deltaX;
	charDeltas.x														-= deltaX;
	if(charDeltas.x < 0) {
		character.Position.x												-= 1;
		charDeltas.x														+= 1;
	}
	
	// Update Y coordinate
	int																		deltaY							= (int)charDeltas.y;
	character	.Position.y												+= deltaY;
	charDeltas	.y														-= deltaY;
	if(charDeltas.y < 0) {
		character	.Position.y												-= 1;
		charDeltas	.y														+= 1;
	}
}

void																shoot								( ::game::SGame& gameObject, ::game::SVector2 origin, double direction )						{
	::game::SShot															newShot								= {};
	newShot.Damage														= 10; // 10 HP
	newShot.Speed														= 10.0f; // 10 tiles per second
	newShot.PositionDeltas												= origin;
	newShot.Direction													= direction;
	refreshPosFromDeltas( newShot ); // update tile coordinates

	gameObject.Shots.push_back( newShot );
}

// Use this function to update the map tiles
void																updateMap							( ::game::SGame& /*gameObject*/, double /*fLastFrameTime*/ )										{}

// Use this function to update the player
void																updatePlayer						( ::game::SGame& gameObject, double fLastFrameTime  )											{
	// Query key states
	bool																	_keyUP								= ::GetAsyncKeyState('W') ? true : false,
																			_keyDOWN							= ::GetAsyncKeyState('S') ? true : false,
																			_keyLEFT							= ::GetAsyncKeyState('A') ? true : false,
																			_keyRIGHT							= ::GetAsyncKeyState('D') ? true : false;

	::game::SVector2														dir									= {0,0};
	// set target direction from keys pressed
	if(_keyRIGHT)	++dir.x; //dir.x = 1;
	if(_keyLEFT)	--dir.x; //dir.x = -1;
	if(_keyUP)		--dir.y; //dir.y = -1;
	if(_keyDOWN)	++dir.y; //dir.y = 1;

	dir.Normalize();	// normalize the new direction vector (make it a unit length so we can multiply it by speed to get velocity vector).

	bool																	noKeysPressed						= !_keyRIGHT && !_keyLEFT && !_keyUP && !_keyDOWN;
	if( noKeysPressed )
		gameObject.Player.Action										= ::game::ACTION_STAND;
	else {																
		gameObject.Player.Action										= ::game::ACTION_WALK;
		gameObject.Player.Direction										= ::game::SVector2{1, 0}.AngleWith(dir);
		if( dir.y < 0 )
			gameObject.Player.Direction									*= -1; 
	}

	// Increase 50% speed if left shift pressed.
	double																	fSpeed								= ::GetAsyncKeyState(VK_LSHIFT) ? gameObject.Player.Speed * 1.5 : gameObject.Player.Speed;

	if( gameObject.Player.Action == ::game::ACTION_WALK ) {
		dir.Scale(fSpeed*fLastFrameTime);
		// integrate our calculated displacement
		gameObject.Player.PositionDeltas									+= dir;
	}

	// refresh tile coords now that we have accumulated the distance walked this frame
	::refreshPosFromDeltas( gameObject.Player );

	if(::GetAsyncKeyState(VK_SPACE)) {
		::game::SVector2														origin								= { gameObject.Player.Position.x + gameObject.Player.PositionDeltas.x, gameObject.Player.Position.y + gameObject.Player.PositionDeltas.y };
		shoot( gameObject, origin, gameObject.Player.Direction );
	}
}

// Use this function to update the enemies
void																updateEnemies						( ::game::SGame& gameObject, double fLastFrameTime )											{
	uint32_t																mapWidth							= gameObject.Map.Size.x;
	uint32_t																mapDepth							= gameObject.Map.Size.y;

	memset( &gameObject.Map.Enemy.Cells[0][0], INVALID_CHARACTER, sizeof(int) * mapWidth * mapDepth );	// clear enemy layer to refresh the enemy map layer

																									// An iterator is required for std::vector<SCharacter>::erase().
	std::vector<::game::SCharacter>::iterator								iEnemy								= gameObject.Enemy.begin(); // 
	int32_t																	indexEnemy							= 0; // keep track of enemy index
	while( iEnemy != gameObject.Enemy.end() ) {
		::game::SCharacter														& currentEnemy						= gameObject.Enemy[indexEnemy]; // get the address of the current enemy at [iEnemy] index
		::game::SVector2														& enemyDeltas						= currentEnemy.PositionDeltas;

		if( currentEnemy.CurrentPoints.HP <= 0 ) {		// remove enemy if zero health
			gameObject.Player.CurrentPoints.XP									+= iEnemy->MaxPoints.HP;
			iEnemy																= gameObject.Enemy.erase( iEnemy );	// 
			continue; // keep at the current index of the list 
		}

		double																	fEnemySpeed							= currentEnemy.Speed;
		::game::STileCoord2														& playerPosition					= gameObject.Player.Position;
		::game::STileCoord2														& enemyPosition						= currentEnemy.Position;

			 if( playerPosition.x < enemyPosition.x )	enemyDeltas.x			-= (float)(fEnemySpeed * fLastFrameTime);	// decrease x 
		else if( playerPosition.x > enemyPosition.x )	enemyDeltas.x			+= (float)(fEnemySpeed * fLastFrameTime);	// increase x 

			 if( playerPosition.y < enemyPosition.y )	enemyDeltas.y			-= (float)(fEnemySpeed * fLastFrameTime);	// decrease z 
		else if( playerPosition.y > enemyPosition.y )	enemyDeltas.y			+= (float)(fEnemySpeed * fLastFrameTime);	// increase z 

		::refreshPosFromDeltas( currentEnemy );	// refresh tile coordinates now that we have accumulated the distance walked

		if( playerPosition.y == enemyPosition.y 
		 && playerPosition.x == enemyPosition.x 
		 ) { // Decrease player life if enemy position matches player position
			gameObject.Player.CurrentPoints.HP									-= ::ftwl::max( 1, currentEnemy.CurrentPoints.HP/3 );
			enemyPosition.x														= rand() % mapWidth;
			enemyPosition.y														= rand() % mapDepth;	// set a random position for the enemy so it has to walk again in order to hit the player
		}

		gameObject.Map.Enemy.Cells[enemyPosition.y][enemyPosition.x]			= indexEnemy; // assign enemy index to the cell corresponding to this enemy
		++indexEnemy;
		++iEnemy;
	}
}

void																updateShots							( ::game::SGame& gameObject, double fLastFrameTime  )										{
	uint32_t																mapWidth							= (uint32_t)gameObject.Map.Size.x;
	uint32_t																mapDepth							= (uint32_t)gameObject.Map.Size.y;

	memset( &gameObject.Map.Shots.Cells[0][0], INVALID_SHOT, sizeof(int) * mapWidth * mapDepth );	// clear enemy layer to refresh the enemy map layer

	::std::vector<::game::SShot>::iterator									iShot								= gameObject.Shots.begin(); // 
	short 																	indexShot							= 0;
	while( iShot != gameObject.Shots.end() ) {
		::game::SShot															& currentShot						= gameObject.Shots[indexShot]; 
		::game::SVector2														& shotDeltas						= currentShot.PositionDeltas;

		::game::SVector2														dirVector							= ::game::SVector2{1, 0}.Rotate( currentShot.Direction );
		shotDeltas.x														+= float(currentShot.Speed * fLastFrameTime * dirVector.x); // add speed*time*direction to our coordinates 
		shotDeltas.y														+= float(currentShot.Speed * fLastFrameTime * dirVector.y); // add speed*time*direction to our coordinates 

		refreshPosFromDeltas( currentShot );	// refresh cell coordinates now that we have accumulated the distance

		if( currentShot.Position.x < 0 
		 || currentShot.Position.y < 0 
		 || currentShot.Position.x >= (int32_t)mapWidth
		 || currentShot.Position.y >= (int32_t)mapDepth
		 ) { // remove shot if reached the end of the screen
			iShot																= gameObject.Shots.erase( iShot );
			continue;
		}
		//
		if( gameObject.Map.Enemy.Cells[currentShot.Position.y][currentShot.Position.x] != INVALID_CHARACTER ) { // damage enemy and remove shot
			int32_t																	enemyIndex							= gameObject.Map.Enemy.Cells[currentShot.Position.y][currentShot.Position.x];
			::game::SCharacterPoints												& currentEnemyPoints				= gameObject.Enemy[enemyIndex].CurrentPoints;	// get a reference in order to reduce verbosity of our code
			currentEnemyPoints.HP												-= iShot->Damage; // Decrease player life if enemy position matches player position
			gameObject.Player.CurrentPoints.XP									+= iShot->Damage;
			iShot																= gameObject.Shots.erase( iShot ); 
			continue;
		}
		gameObject.Map.Shots.Cells[currentShot.Position.y][currentShot.Position.x]			= indexShot; // assign enemy index to the cell corresponding to this enemy
		++iShot;
		++indexShot;
	}
}

::ftwl::error_t													game::update						(::game::SGame& gameInstance, uint64_t timeElapsedMicroseconds) {
	if( gameInstance.Player.CurrentPoints.HP <= 0 || gameInstance.Enemy.size() == 0 )
		return 0; // return if no enemies or if player HP is 0

	// call update game functions
	double																	timeElapsedSeconds					= timeElapsedMicroseconds / 1000000.0;
	updateMap		( gameInstance, timeElapsedSeconds );
	updatePlayer	( gameInstance, timeElapsedSeconds );
	updateShots		( gameInstance, timeElapsedSeconds );
	updateEnemies	( gameInstance, timeElapsedSeconds ); // update enemies

	return 0;
}
