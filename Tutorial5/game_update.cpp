#include "game.h"

#include <Windows.h>

#define INVALID_CHARACTER	::game::CHARACTER_TYPE_INVALID
#define INVALID_SHOT		::game::SHOT_TYPE_INVALID


void																shoot								( ::game::SGame& gameObject, const ::game::SVector2& origin, double direction, int32_t damage )		{
	::game::SCharacter														newShot								= {};
	newShot.PointsCurrent.DP											= damage; //
	newShot.Speed														= 8.0; // 8 tiles per second
	newShot.DirectionInRadians											= direction;

	::game::SRigidBody														shotBody							= {};
	shotBody.Position.Deltas											= origin;
	shotBody.Position.RefreshPosFromDeltas();	// update tile coordinates

	newShot.RigidBody													= gameObject.RigidBodyEngine.AddRigidBody(shotBody);
	gameObject.Shots.push_back( newShot );
}

// Use this function to update the map tiles
void																updateMap							( ::game::SGame& /*gameObject*/, double /*fLastFrameTime*/ )										{}

static constexpr const	::ftwlib::SCoord2<double>					directionFront						= {1, 0};

void																updatePlayerInput					( ::game::SGame& gameObject)																		{
	::game::SCharacter														& playerInstance					= gameObject.Player;
	// --- Query key states
	bool																	_keyUP								= (::GetAsyncKeyState('W') || ::GetAsyncKeyState(VK_UP		)) ? true : false
		,																	_keyDOWN							= (::GetAsyncKeyState('S') || ::GetAsyncKeyState(VK_DOWN	)) ? true : false
		,																	_keyLEFT							= (::GetAsyncKeyState('A') || ::GetAsyncKeyState(VK_LEFT	)) ? true : false
		,																	_keyRIGHT							= (::GetAsyncKeyState('D') || ::GetAsyncKeyState(VK_RIGHT	)) ? true : false
		;

	bool																	noKeysPressed						= !_keyRIGHT && !_keyLEFT && !_keyUP && !_keyDOWN;
	playerInstance.ActionActive											&= ~(1 << ::game::ACTION_TYPE_WALK);
	playerInstance.ActionActive											&= ~(1 << ::game::ACTION_TYPE_TURN);

	if( noKeysPressed )
		return;

	// set target direction from keys pressed
	if(_keyLEFT || _keyRIGHT) {
		playerInstance.ActionActive											|= 1 << ::game::ACTION_TYPE_TURN; 
		playerInstance.ActionDirection[::game::ACTION_TYPE_TURN].Right		= (_keyRIGHT	) ? 1 : 0;
		playerInstance.ActionDirection[::game::ACTION_TYPE_TURN].Left		= (_keyLEFT		) ? 1 : 0;
	}

	if(_keyUP || _keyDOWN) {
		playerInstance.ActionActive											|= 1 << ::game::ACTION_TYPE_WALK; 
		playerInstance.ActionDirection[::game::ACTION_TYPE_WALK].Front		= (_keyUP		) ? 1 : 0; 
		playerInstance.ActionDirection[::game::ACTION_TYPE_WALK].Back		= (_keyDOWN		) ? 1 : 0;
	}
}

template<typename _tValue>	
static inline constexpr	bool										in_range							(const _tValue& value, const _tValue& minValue, const _tValue& maxValue)							{ return value < maxValue && value > minValue; }

void																evaluateAction						( ::game::SCharacter& playerInstance, double fLastFrameTime )										{
	const double															timeScale							= 3;
	
	if(playerInstance.ActionActive & (1 << ::game::ACTION_TYPE_TURN)) {
		if(playerInstance.ActionDirection[::game::ACTION_TYPE_TURN].Right	== true) playerInstance.DirectionInRadians += fLastFrameTime;
		if(playerInstance.ActionDirection[::game::ACTION_TYPE_TURN].Left	== true) playerInstance.DirectionInRadians -= fLastFrameTime;
	}

	if(playerInstance.ActionActive & (1 << ::game::ACTION_TYPE_WALK)) {
		if(playerInstance.ActionDirection[::game::ACTION_TYPE_WALK].Back)
			playerInstance.Speed												-= fLastFrameTime * timeScale;
		else
			playerInstance.Speed												+= fLastFrameTime * timeScale;
	}

	playerInstance.Speed												= ::ftwlib::max(0.0, ::ftwlib::min(playerInstance.Speed, playerInstance.SpeedMax));
}

void																updatePlayer						( ::game::SGame& gameObject, double fLastFrameTime )												{
	::game::SCharacter														& playerInstance					= gameObject.Player;
	::game::SRigidBodyEngine												& bodyEngine						= gameObject.RigidBodyEngine;
	::game::SRigidBody														& playerBody						= bodyEngine.RigidBody		[playerInstance.RigidBody];
	::game::SRigidBody														& playerBodyNext					= bodyEngine.RigidBodyNext	[playerInstance.RigidBody];
	::game::SRigidBodyState													& playerBodyState					= bodyEngine.RigidBodyState	[playerInstance.RigidBody];
	::game::SEntityCoord2													& playerPosition					= playerBody.Position;
	::game::STileCoord2														& playerCell						= playerPosition.Tile;
	::game::SVector2														& playerDeltas						= playerPosition.Deltas;

	::game::STileCoord2														& nextTile							= playerBodyNext.Position.Tile;
	const ::game::SMap														& gameMap							= gameObject.Map;
	nextTile.x															= ::ftwlib::min((uint32_t)::ftwlib::max(playerBodyNext.Position.Tile.x, 1), gameMap.Size.x - 2);
	nextTile.y															= ::ftwlib::min((uint32_t)::ftwlib::max(playerBodyNext.Position.Tile.y, 1), gameMap.Size.y - 2);

	if( gameMap.Enemy.Cells[nextTile.y][nextTile.x] == INVALID_CHARACTER
	 && gameObject.Descriptions.Floor[gameMap.Floor.Cells[nextTile.y][nextTile.x]].Transitable
	 ) {
		playerBody															= playerBodyNext;
		playerBodyState.Active												= true;
	}

	::evaluateAction(playerInstance, fLastFrameTime);

	::game::SVector2														dirVector							= ::game::SVector2{1, 0}.Rotate( playerInstance.DirectionInRadians );
	playerBody.Velocity													= dirVector * playerInstance.Speed;

	if(::GetAsyncKeyState(VK_SPACE)) {
		::game::SVector2														origin								= { playerCell.x + playerDeltas.x, playerCell.y + playerDeltas.y };
		::shoot( gameObject, origin, playerInstance.DirectionInRadians, playerInstance.PointsCurrent.DP );
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

	uint32_t																indexEnemy							= 0; // keep track of enemy index
	while( indexEnemy < (uint32_t)gameObject.Enemy.size() ) {
		::game::SCharacter														& currentEnemy						= gameObject.Enemy[indexEnemy]; // get the address of the current enemy at [iEnemy] index

		if( currentEnemy.PointsCurrent.HP <= 0 ) {		// remove enemy if zero health
			currentPlayerPoints.XP												+= currentEnemy.PointsMax.HP;	// Increase experience depending on enemy health.
			gameObject.Enemy.erase( gameObject.Enemy.begin() + indexEnemy );	// An iterator is required for ::std::vector<SCharacter>::erase(). This nonsense will be fixed in further releases containing our own implementation of a dynamic array.
			continue; // keep at the current index of the list 
		}

		::game::SRigidBody														& enemyBody							= gameObject.RigidBodyEngine.RigidBody[currentEnemy.RigidBody];
		::game::SVector2														& enemyDeltas						= enemyBody.Position.Deltas;
		::game::STileCoord2														& enemyCell							= enemyBody.Position.Tile;
		double																	fEnemySpeed							= currentEnemy.Speed;

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

	uint32_t																indexShot							= 0;
	while( indexShot < (uint32_t)gameObject.Shots.size() ) {
		::game::SCharacter														& currentShot						= gameObject.Shots[indexShot]; 
		::game::SRigidBody														& shotBody							= gameObject.RigidBodyEngine.RigidBody[currentShot.RigidBody];
		::game::SVector2														& shotDeltas						= shotBody.Position.Deltas;
		::game::STileCoord2														& shotCell							= shotBody.Position.Tile;

		::game::SVector2														dirVector							= ::game::SVector2{1, 0}.Rotate( currentShot.DirectionInRadians );
		shotDeltas.x														+= float(currentShot.Speed * fLastFrameTime * dirVector.x); // add speed*time*direction to our coordinates 
		shotDeltas.y														+= float(currentShot.Speed * fLastFrameTime * dirVector.y); // add speed*time*direction to our coordinates 

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
		gameObject.Map.Shots.Cells[shotCell.y][shotCell.x]					= indexShot; // assign enemy index to the cell corresponding to this enemy
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

	gameInstance.RigidBodyEngine.CalcNextPositions(gameInstance.FrameInfo.LastFrameSeconds);

	// call update game functions
	::updateMap		( gameInstance, gameInstance.FrameInfo.LastFrameSeconds );
	::updatePlayer	( gameInstance, gameInstance.FrameInfo.LastFrameSeconds );
	::updateShots	( gameInstance, gameInstance.FrameInfo.LastFrameSeconds );
	::updateEnemies	( gameInstance, gameInstance.FrameInfo.LastFrameSeconds ); // update enemies

	::updatePlayerInput(gameInstance);
	return 0;
}
