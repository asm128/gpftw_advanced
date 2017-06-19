// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
//
#define NOMINMAX

#include "gpftw_ascii_screen.h"
#include "gpftw_ascii_color.h"

#include "tilemap.h"

#include <stdio.h>      // for printf()
#include <crtdbg.h>      // for printf()
#include <windows.h>    // for interacting with Windows

static constexpr		const int										SCREEN_WIDTH						= 48	;
static constexpr		const int										SCREEN_HEIGHT						= 32	;

template<typename _tValue>	static inline constexpr	const _tValue		max									(const _tValue& a, const _tValue& b)				{ return (a > b) ? a : b; }
template<typename _tValue>	static inline constexpr	const _tValue		min									(const _tValue& a, const _tValue& b)				{ return (a < b) ? a : b; }

struct SApplication {
	::gpftw::SASCIIScreen													ASCIIScreen							= {};
	int																		FrameCounter						= 0;	// Declare and initialize a variable of (int)eger type for keeping track of the number of frame since execution began.

	STileMap																TileMap								= {};
};

// Cleanup application resources.
void																	cleanup								(::SApplication& applicationInstance)				{ 
	::destroyTileMap		(applicationInstance.TileMap);									
	::gpftw::destroyConsole	(applicationInstance.ASCIIScreen);									
}

void																	setupRoom							
	(	::gpftw::grid_view<uint32_t>	& asciiMap
	,	uint32_t						offsetX
	,	uint32_t						offsetZ
	,	uint32_t						sizeX
	,	uint32_t						sizeZ
	)
{ 
	for(uint32_t z = offsetZ; z < (offsetZ+sizeZ); ++z)
	for(uint32_t x = offsetX; x < (offsetX+sizeX); ++x) {
		asciiMap[z][x]															= 2;
		asciiMap[z][x]															= 2;
	}

	for(uint32_t z = offsetZ; z < (offsetZ+sizeZ); ++z) {
		asciiMap[z][offsetX				]										= 3;
		asciiMap[z][offsetX + sizeX	- 1	]										= 3;
	}

	for(uint32_t x = offsetX; x < (offsetX+sizeX); ++x) {
		asciiMap[offsetZ			][x]										= 3;
		asciiMap[offsetZ + sizeZ - 1][x]										= 3;
	}
}

void																	setupMap							(::STileMap& tileMapInstance)						{ 
	uint32_t																	tileMapWidth						= tileMapInstance.ASCIIMap.width	(); 
	uint32_t																	tileMapHeight						= tileMapInstance.ASCIIMap.height	(); 
	for(uint32_t iCell = 0; iCell < tileMapWidth * tileMapHeight; ++iCell) 
		tileMapInstance.ASCIIMap.begin()[iCell]									= 1;


	for(uint32_t z = 0; z < tileMapHeight; ++z) {
		tileMapInstance.ASCIIMap[z][0					]						= 3;
		tileMapInstance.ASCIIMap[z][tileMapWidth - 1	]						= 3;
	}

	for(uint32_t x = 0; x < tileMapWidth; ++x) {
		tileMapInstance.ASCIIMap[0					][x]						= 3;
		tileMapInstance.ASCIIMap[tileMapHeight - 1	][x]						= 3;
	}

	// Draw front towers
	setupRoom(tileMapInstance.ASCIIMap, 2,  6, 6, 5);
	setupRoom(tileMapInstance.ASCIIMap, tileMapWidth - 8,  6, 6, 5);

	// Draw main hall
	setupRoom(tileMapInstance.ASCIIMap, 2, 20, tileMapWidth - 4, 7);
}

static constexpr	const STileASCII									tileDescriptions[]					= 
	{	{' '	, ::gpftw::ASCII_COLOR_INDEX_0}
	,	{'~'	, ::gpftw::ASCII_COLOR_INDEX_1}
	,	{'.'	, ::gpftw::ASCII_COLOR_INDEX_2}
	,	{'#'	, ::gpftw::ASCII_COLOR_INDEX_12}
	,	{'^'	, ::gpftw::ASCII_COLOR_INDEX_10}
	,	{'-'	, ::gpftw::ASCII_COLOR_INDEX_5}
	,	{'\\'	, ::gpftw::ASCII_COLOR_INDEX_5}
	,	{'|'	, ::gpftw::ASCII_COLOR_INDEX_5}
	,	{'/'	, ::gpftw::ASCII_COLOR_INDEX_5}
	,	{'|'	, ::gpftw::ASCII_COLOR_INDEX_5}
	};

template<typename _tElement, size_t _arraySize>
static constexpr	const uint32_t										size								(const _tElement (&)[_arraySize])					{ return (uint32_t)_arraySize; }

void																	setup								(::SApplication& applicationInstance)				{ 
	::gpftw::createConsole	(applicationInstance.ASCIIScreen, ::SCREEN_WIDTH, ::SCREEN_HEIGHT);	
	::initializeTileMap		(applicationInstance.TileMap	, ::SCREEN_WIDTH, ::SCREEN_HEIGHT, tileDescriptions, size(tileDescriptions));	
	::setupMap				(applicationInstance.TileMap	);
}	

void																	update								(::SApplication& applicationInstance)				{ 
	::gpftw::presentConsole(applicationInstance.ASCIIScreen);	// Present the current image if any.

	::gpftw::ASCII_COLOR														oldColor							= (::gpftw::ASCII_COLOR)applicationInstance.ASCIIScreen.Palette[::gpftw::ASCII_COLOR_INDEX_1];
	applicationInstance.ASCIIScreen.Palette[::gpftw::ASCII_COLOR_INDEX_1]	= ((applicationInstance.FrameCounter % 32) >= 16) ? (oldColor & 0x00FFFF) | ((oldColor & 0xFF0000)-0x40000) : (oldColor & 0x00FFFF) | ((oldColor & 0xFF0000)+0x40000);


	++applicationInstance.FrameCounter;																		
}	// Increase our frame counter by 1.

void																	draw								(::SApplication& applicationInstance)				{	
	// This function now will draw some coloured symbols in each cell of the ASCII screen.
	::gpftw::SASCIIScreen														& asciiTarget						= applicationInstance.ASCIIScreen;
	::STileMap																	& tileMap							= applicationInstance.TileMap;

	// 2d loop
	for(uint32_t z = 0; z < tileMap.ASCIIMap.height	(); ++z)
	for(uint32_t x = 0; x < tileMap.ASCIIMap.width	(); ++x) {
		uint32_t																	linearIndex							= z * asciiTarget.Width	+ x;	// calculate the linear index of the screen array from the current x and y of the screen
		uint32_t																	tileDescriptionIndex				= tileMap.ASCIIMap[z][x];	// Get the tile description index that points to the tile table record
		
		// Set the target cell with the character and color for to the current tile found in the map.
		asciiTarget.Characters	[linearIndex]									= tileMap.TileDescriptionTable[tileDescriptionIndex].Character	;
		asciiTarget.Colors		[linearIndex]									= tileMap.TileDescriptionTable[tileDescriptionIndex].Color		;
	}
}

int																		main								()													{
	::SApplication																* applicationInstance				= new ::SApplication();	// Create a new instance of our application.
	if( 0 == applicationInstance )
		return -1;	// return error because we couldn't allocate the main instance of our application.

	::setup(*applicationInstance); /// Call setup()
	
	while( true ) { /// Execute code between braces while the condition inside () evaluates to true.
		::update	(*applicationInstance);		// Update frame.
		::draw		(*applicationInstance);		// Render frame.
		if(::GetAsyncKeyState(VK_ESCAPE))		/// Check for escape key pressed.
			break;	/// Exit while() loop.
	}

	::cleanup(*applicationInstance);

	delete( applicationInstance );	// Destroy the applcation instance and release its memory.
	return 0; /// Exit from the function returning an (int)eger.
}

int	WINAPI																WinMain								
	(	_In_		HINSTANCE		// hInstance
	,	_In_opt_	HINSTANCE		// hPrevInstance
	,	_In_		LPSTR			// lpCmdLine
	,	_In_		INT				// nShowCmd
	)
{
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
	return ::gpftw::failed( 0 > main() ) ? EXIT_FAILURE : EXIT_SUCCESS;	// just redirect to our generic main() function.
}