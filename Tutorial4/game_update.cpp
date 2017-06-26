#include "game.h"

#include <Windows.h>

#define INVALID_CHARACTER		::game::CHARACTER_TYPE_INVALID
#define INVALID_SHOT		::game::SHOT_TYPE_INVALID


void																shoot								( ::game::SGame& gameObject, const ::game::SVector2& origin, double direction, int32_t damage )		{
	::game::SCharacter														newShot								= {};
	newShot.PointsCurrent.DP											= damage; //

	::game::SRigidBody														shotBody							= {};
		
	shotBody.Speed													= 10.0f; // 10 tiles per second
	shotBody.Position.Deltas										= origin;
	shotBody.Direction												= direction;
	shotBody.Position.RefreshPosFromDeltas(); // update tile coordinates
	newShot.RigidBody													= gameObject.RigidBodyEngine.AddRigidBody(shotBody);

	gameObject.Shots.push_back( newShot );
}

// Use this function to update the map tiles
void																updateMap							( ::game::SGame& /*gameObject*/, double /*fLastFrameTime*/ )										{}

// Use this function to update the player
void																updatePlayer						( ::game::SGame& gameObject, double fLastFrameTime  )												{
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

	::game::SCharacter														& playerInstance					= gameObject.Player;
	::game::SRigidBody														& playerBody						= gameObject.RigidBodyEngine.RigidBody[playerInstance.RigidBody];

	bool																	noKeysPressed						= !_keyRIGHT && !_keyLEFT && !_keyUP && !_keyDOWN;
	if( noKeysPressed )
		playerInstance.Action												= ::game::ACTION_STAND;
	else {																		
		playerInstance.Action												= ::game::ACTION_WALK;
		playerBody.Direction												= ::game::SVector2{1, 0}.AngleWith(dir);
		if( dir.y < 0 )
			playerBody.Direction												*= -1; 
	}

	// Increase 50% speed if left shift pressed.
	double																	fSpeed								= ::GetAsyncKeyState(VK_LSHIFT) ? playerBody.Speed * 1.5 : playerBody.Speed;

	::game::SEntityCoord2													& playerPosition						= playerBody.Position;
	::game::STileCoord2														& playerCell							= playerPosition.Tile;
	::game::SVector2														& playerDeltas							= playerPosition.Deltas;
	if( gameObject.Player.Action == ::game::ACTION_WALK ) {
		dir.Scale(fSpeed*fLastFrameTime);
		// integrate our calculated displacement
		playerDeltas														+= dir;
	}

	// refresh tile coords now that we have accumulated the distance walked this frame
	playerPosition.RefreshPosFromDeltas();

	if(::GetAsyncKeyState(VK_SPACE)) {
		::game::SVector2														origin								= { playerCell.x + playerDeltas.x, playerCell.y + playerDeltas.y };
		::shoot( gameObject, origin, playerBody.Direction, playerInstance.PointsCurrent.DP );
	}
}

// Use this function to update the enemies
void																updateEnemies						( ::game::SGame& gameObject, double fLastFrameTime )												{
	uint32_t																mapWidth							= gameObject.Map.Size.x;
	uint32_t																mapDepth							= gameObject.Map.Size.y;

	::memset( &gameObject.Map.Enemy.Cells[0][0], INVALID_CHARACTER, sizeof(int) * mapWidth * mapDepth );	// clear enemy layer to refresh the enemy map layer

	::game::SCharacterPoints												& currentPlayerPoints				= gameObject.Player.PointsCurrent; // get the address of the current enemy at [iEnemy] index
	::game::SRigidBody														& playerBody						= gameObject.RigidBodyEngine.RigidBody[gameObject.Player.RigidBody];
	::game::STileCoord2														& playerCell						= playerBody.Position.Tile;

	int32_t																	indexEnemy							= 0; // keep track of enemy index
	while( indexEnemy < gameObject.Enemy.size() ) {
		::game::SCharacter														& currentEnemy						= gameObject.Enemy[indexEnemy]; // get the address of the current enemy at [iEnemy] index

		if( currentEnemy.PointsCurrent.HP <= 0 ) {		// remove enemy if zero health
			currentPlayerPoints.XP												+= currentEnemy.PointsMax.HP;	// Increase experience depending on enemy health.
			gameObject.Enemy.erase( gameObject.Enemy.begin() + indexEnemy );	// An iterator is required for ::std::vector<SCharacter>::erase(). This nonsense will be fixed in further releases containing our own implementation of a dynamic array.
			continue; // keep at the current index of the list 
		}

		::game::SRigidBody														& enemyBody							= gameObject.RigidBodyEngine.RigidBody[currentEnemy.RigidBody];
		::game::SVector2														& enemyDeltas						= enemyBody.Position.Deltas;
		::game::STileCoord2														& enemyCell							= enemyBody.Position.Tile;
		double																	fEnemySpeed							= enemyBody.Speed;

			 if( playerCell.x < enemyCell.x )	enemyDeltas.x					-= (float)(fEnemySpeed * fLastFrameTime);	// decrease x 
		else if( playerCell.x > enemyCell.x )	enemyDeltas.x					+= (float)(fEnemySpeed * fLastFrameTime);	// increase x 

			 if( playerCell.y < enemyCell.y )	enemyDeltas.y					-= (float)(fEnemySpeed * fLastFrameTime);	// decrease z 
		else if( playerCell.y > enemyCell.y )	enemyDeltas.y					+= (float)(fEnemySpeed * fLastFrameTime);	// increase z 

		enemyBody.Position.RefreshPosFromDeltas();	// refresh tile coordinates now that we have accumulated the distance walked

		if( playerCell.y == enemyCell.y 
		 && playerCell.x == enemyCell.x 
		 ) { // Decrease player life if enemy position matches player position
			currentPlayerPoints.HP												-= ::ftwlib::max( 1, currentEnemy.PointsCurrent.HP/3 );
			enemyCell.x															= rand() % mapWidth;
			enemyCell.y															= rand() % mapDepth;	// set a random position for the enemy so it has to walk again in order to hit the player
		}

		gameObject.Map.Enemy.Cells[enemyCell.y][enemyCell.x]				= indexEnemy; // assign enemy index to the cell corresponding to this enemy
		++indexEnemy;
	}
}

void																updateShots							( ::game::SGame& gameObject, double fLastFrameTime  )												{
	uint32_t																mapWidth							= (uint32_t)gameObject.Map.Size.x;
	uint32_t																mapDepth							= (uint32_t)gameObject.Map.Size.y;

	::memset( gameObject.Map.Shots.Cells.begin(), INVALID_SHOT, sizeof(int) * mapWidth * mapDepth );	// clear enemy layer to refresh the enemy map layer

	short 																	indexShot							= 0;
	while( indexShot < gameObject.Shots.size() ) {
		::game::SCharacter														& currentShot						= gameObject.Shots[indexShot]; 
		::game::SRigidBody														& shotBody							= gameObject.RigidBodyEngine.RigidBody[currentShot.RigidBody];
		::game::SVector2														& shotDeltas						= shotBody.Position.Deltas;
		::game::STileCoord2														& shotCell							= shotBody.Position.Tile;

		::game::SVector2														dirVector							= ::game::SVector2{1, 0}.Rotate( shotBody.Direction );
		shotDeltas.x														+= float(shotBody.Speed * fLastFrameTime * dirVector.x); // add speed*time*direction to our coordinates 
		shotDeltas.y														+= float(shotBody.Speed * fLastFrameTime * dirVector.y); // add speed*time*direction to our coordinates 

		shotBody.Position.RefreshPosFromDeltas();	// refresh cell coordinates now that we have accumulated the distance

		if( shotCell.x < 0 
		 || shotCell.y < 0 
		 || shotCell.x >= (int32_t)mapWidth
		 || shotCell.y >= (int32_t)mapDepth
		 ) { // remove shot if reached the end of the screen
			gameObject.Shots.erase( gameObject.Shots.begin() + indexShot );
			continue;
		}
		//
		if( gameObject.Map.Enemy.Cells[shotCell.y][shotCell.x] != INVALID_CHARACTER ) { // damage enemy and remove shot
			int32_t																	enemyIndex							= gameObject.Map.Enemy.Cells[shotCell.y][shotCell.x];
			::game::SCharacterPoints												& currentEnemyPoints				= gameObject.Enemy[enemyIndex].PointsCurrent;	// get a reference in order to reduce verbosity of our code
			currentEnemyPoints.HP												-= currentShot.PointsCurrent.DP; // Decrease player life if enemy position matches player position
			gameObject.Player.PointsCurrent.XP									+= currentShot.PointsCurrent.DP;
			gameObject.Shots.erase( gameObject.Shots.begin() + indexShot ); 
			continue;
		}
		gameObject.Map.Shots.Cells[shotCell.y][shotCell.x]			= indexShot; // assign enemy index to the cell corresponding to this enemy
		++indexShot;
	}
}

::ftwlib::error_t													game::update						(::game::SGame& gameInstance, uint64_t timeElapsedMicroseconds)										{
	if( gameInstance.Player.PointsCurrent.HP <= 0 || gameInstance.Enemy.size() == 0 )
		return 0; // return if no enemies or if player HP is 0

	// Set last frame time and number.
	++gameInstance.FrameInfo.FrameNumber;
	gameInstance.FrameInfo.LastFrameSeconds								= timeElapsedMicroseconds / 1000000.0;
	gameInstance.FrameInfo.LastFrameMicroseconds						= timeElapsedMicroseconds;
	gameInstance.FrameInfo.TotalTime									+= timeElapsedMicroseconds;

	// call update game functions
	::updateMap		( gameInstance, gameInstance.FrameInfo.LastFrameSeconds );
	::updatePlayer	( gameInstance, gameInstance.FrameInfo.LastFrameSeconds );
	::updateShots	( gameInstance, gameInstance.FrameInfo.LastFrameSeconds );
	::updateEnemies	( gameInstance, gameInstance.FrameInfo.LastFrameSeconds ); // update enemies

	return 0;
}
