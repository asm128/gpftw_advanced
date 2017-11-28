// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
//
#define NOMINMAX

#include "ftw_ascii_screen.h"
#include "ftw_ascii_color.h"

#include "tilemap.h"

#include <crtdbg.h>      // for _CrtSetDbgFlag()
#include <windows.h>    // for interacting with Windows

static constexpr		const int										SCREEN_WIDTH						= 48	;
static constexpr		const int										SCREEN_HEIGHT						= 32	;

template<typename _tValue>	static inline constexpr	const _tValue		max									(const _tValue& a, const _tValue& b)				{ return (a > b) ? a : b; }
template<typename _tValue>	static inline constexpr	const _tValue		min									(const _tValue& a, const _tValue& b)				{ return (a < b) ? a : b; }

struct SApplication {
	int																		FrameCounter						= 0;	// Declare and initialize a variable of (int)eger type for keeping track of the number of frame since execution began.
	::ftwl::SASCIITarget													ASCIIRenderTarget					= {};
	::ftwl::SPalette														Palette								= 
		{	(uint32_t)::ftwl::ASCII_COLOR_INDEX_0		
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_1 	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_2 	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_3 	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_4 	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_5 	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_6 	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_7 	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_8 	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_9 	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_10	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_11	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_12	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_13	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_14	
		,	(uint32_t)::ftwl::ASCII_COLOR_INDEX_15	
		};

	STileMap																TileMap								= {};
};

// Cleanup application resources.
void																	cleanup								(::SApplication& applicationInstance)				{ 
	::destroyTileMap			(applicationInstance.TileMap);									
	::ftwl::asciiDisplayDestroy	();									
	::ftwl::asciiTargetDestroy	(applicationInstance.ASCIIRenderTarget);									
}

void																	setupRoom							
	(	::ftwl::grid_view<uint32_t>	& asciiMap
	,	uint32_t						offsetX
	,	uint32_t						offsetZ
	,	uint32_t						sizeX
	,	uint32_t						sizeZ
	)
{ 
	for(uint32_t z = offsetZ; z < (offsetZ + sizeZ); ++z)
	for(uint32_t x = offsetX; x < (offsetX + sizeX); ++x) {
		asciiMap[z][x]															= 2;
		asciiMap[z][x]															= 2;
	}

	for(uint32_t z = offsetZ; z < (offsetZ + sizeZ); ++z) {
		asciiMap[z][offsetX				]										= 3;
		asciiMap[z][offsetX + sizeX	- 1	]										= 3;
	}

	for(uint32_t x = offsetX; x < (offsetX + sizeX); ++x) {
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
	{	{' '	, ::ftwl::ASCII_COLOR_BLACK		}
	,	{'~'	, ::ftwl::ASCII_COLOR_DARKBLUE	}
	,	{'.'	, ::ftwl::ASCII_COLOR_GREEN		}
	,	{'#'	, ::ftwl::ASCII_COLOR_DARKRED		}
	,	{'^'	, ::ftwl::ASCII_COLOR_DARKGREEN	}
	,	{'-'	, ::ftwl::ASCII_COLOR_RED			}
	,	{'\\'	, ::ftwl::ASCII_COLOR_RED			}
	,	{'|'	, ::ftwl::ASCII_COLOR_RED			}
	,	{'/'	, ::ftwl::ASCII_COLOR_RED			}
	};

template<typename _tElement, size_t _arraySize>
static constexpr	const uint32_t										size								(const _tElement (&)[_arraySize])					{ return (uint32_t)_arraySize; }

void																	setup								(::SApplication& applicationInstance)				{ 
	::ftwl::asciiTargetCreate	(applicationInstance.ASCIIRenderTarget, ::SCREEN_WIDTH, ::SCREEN_HEIGHT);	
	::ftwl::asciiDisplayCreate	(applicationInstance.ASCIIRenderTarget.Width(), applicationInstance.ASCIIRenderTarget.Height());	
	::initializeTileMap		(applicationInstance.TileMap	, ::SCREEN_WIDTH, ::SCREEN_HEIGHT, tileDescriptions, size(tileDescriptions));	
	::setupMap				(applicationInstance.TileMap	);
}	

void																	update								(::SApplication& applicationInstance)				{ 

	::ftwl::SColorRGBA															oldColor0							= applicationInstance.Palette[::ftwl::ASCII_COLOR_DARKBLUE];
	oldColor0.r																= oldColor0.r;
	oldColor0.g																= oldColor0.g;
	oldColor0.b																= ((applicationInstance.FrameCounter % 32) >= 16) ? oldColor0.b + 4 : oldColor0.b - 4;

	::ftwl::asciiDisplayPresent(applicationInstance.ASCIIRenderTarget);	// Present the current image if any.
	++applicationInstance.FrameCounter;		// Increase our frame counter by 1.
																
}	
void																	draw								(::SApplication& applicationInstance)				{	
	// This function now will draw some coloured symbols in each cell of the ASCII screen.
	::ftwl::SASCIITarget														& asciiTarget						= applicationInstance.ASCIIRenderTarget;
	::STileMap																	& tileMap							= applicationInstance.TileMap;

	// 2d loop
	for(uint32_t z = 0; z < tileMap.ASCIIMap.height	(); ++z)
	for(uint32_t x = 0; x < tileMap.ASCIIMap.width	(); ++x) {
		uint32_t																	tileDescriptionIndex				= tileMap.ASCIIMap[z][x];	// Get the tile description index that points to the tile table record
		
		// Set the target cell with the character and color for to the current tile found in the map.
		asciiTarget.Characters	[z][x]											= tileMap.TileDescriptionTable[tileDescriptionIndex].Character	;
		if('~' == asciiTarget.Characters[z][x])
			asciiTarget.Colors		[z][x]											= (((applicationInstance.FrameCounter % 2) + z + x) % 2) ? ::ftwl::ASCII_COLOR_DARKBLUE | (::ftwl::ASCII_COLOR_BLACK << 4) : ::ftwl::ASCII_COLOR_BLACK | (::ftwl::ASCII_COLOR_DARKBLUE << 4) ;
		else
			asciiTarget.Colors		[z][x]											= tileMap.TileDescriptionTable[tileDescriptionIndex].Color		;
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
	return ::ftwl::failed( 0 > main() ) ? EXIT_FAILURE : EXIT_SUCCESS;	// just redirect to our generic main() function.
}
