#include "ftw_ascii_screen.h"

#include "ftw_ascii_color.h"

#include <cstdio>
#include <Windows.h>

// We need this function because we give its pointer to Windows so it can report to our program when CTRL_C was pressed
::BOOL WINAPI									HandlerRoutine							(_In_ ::DWORD dwCtrlType)											{
	switch(dwCtrlType) {
	case CTRL_C_EVENT:
	default:
		return TRUE;
	}
}

static bool										gpftw_isConsoleCreated					= false;

void											initWindowsConsoleFont					()																	{
	if(false == ::gpftw_isConsoleCreated) // we don't create the console twice.
		return;

	const ::HANDLE										handleConsoleOut						= ::GetStdHandle(STD_OUTPUT_HANDLE);
	::CONSOLE_FONT_INFOEX								infoFont;
	::GetCurrentConsoleFontEx(handleConsoleOut, TRUE, &infoFont);

	infoFont.cbSize									= sizeof(::CONSOLE_FONT_INFOEX);
	infoFont.dwFontSize.X							= 8;
	infoFont.dwFontSize.Y							= 12;
	infoFont.FontFamily								= 0;
	infoFont.FontWeight								= 0;
	infoFont.nFont									= 0;
	::wcscpy_s(infoFont.FaceName, L"Terminal");
	::SetCurrentConsoleFontEx(handleConsoleOut, FALSE, &infoFont);
}

void											initWindowsConsole						()																	{
	if(::gpftw_isConsoleCreated) // we don't create the console twice.
		return;

	::AllocConsole();		// This function tells windows to create a console for our program.
	::AttachConsole(GetCurrentProcessId());	// This function tells windows that we want to receive notifications from this process' console. This is so we can catch the CTRL_C event and prevent the program to terminate in an undesired way.
		
	// The following lines are needed to redirect printf() and other Input/Output functions to the newly created console.
	::FILE*										
	stream											= 0;	::freopen_s(&stream, "CONOUT$", "w+", stdout);
	stream											= 0;	::freopen_s(&stream, "CONIN$", "r+", stdin);

	// With this function we tell to Windows that we want to use the HandlerRoutine function to get the CTRL-C notification. 
	::SetConsoleCtrlHandler(HandlerRoutine, TRUE);	// What this does is to send the pointer to HandlerRoutine as the first parameter and Windows then can call the function through its address.

	::EnableMenuItem (	::GetSystemMenu ( ::GetConsoleWindow(), FALSE)	// We do this to grey out/disable the Cross Button used to close the window.
		,				SC_CLOSE
		,				MF_BYCOMMAND | MF_GRAYED 
		);
	// We also need to disable the Quick Edit mode because it breaks the display when you click with the mouse on the console as it tries to select the text and that's not what we want.
	// We need to be able to use the mouse the way we want i.e. to push buttons or drag-and-drop
	const ::HANDLE										handleConsoleIn							= ::GetStdHandle(STD_INPUT_HANDLE);	// Get console input handle
	::DWORD												mode;
	::GetConsoleMode(handleConsoleIn, &mode);
	mode											&= ~(::DWORD)ENABLE_QUICK_EDIT_MODE;
	::SetConsoleMode(handleConsoleIn, mode);

	::gpftw_isConsoleCreated						= true;	// 
}


void											initWindowsConsoleProperties			(int width, int height, const uint32_t* palette)					{
	if(false == ::gpftw_isConsoleCreated) 	// we don't create the console twice.
		return;
	
	const ::HANDLE										handleConsoleOut						= ::GetStdHandle(STD_OUTPUT_HANDLE);	// Get console output handle

	::CONSOLE_SCREEN_BUFFER_INFOEX						csbiInfo								= {};
	::GetConsoleScreenBufferInfoEx(handleConsoleOut, &csbiInfo);
	csbiInfo.cbSize									= sizeof(::CONSOLE_SCREEN_BUFFER_INFOEX);
	csbiInfo.dwSize.X								= (::SHORT)width;
	csbiInfo.dwSize.Y								= (::SHORT)height;
	csbiInfo.dwMaximumWindowSize.X					= (::SHORT)width;
	csbiInfo.dwMaximumWindowSize.Y					= (::SHORT)height;

	csbiInfo.srWindow.Top							= 10;
	csbiInfo.srWindow.Left							= 10;
	csbiInfo.srWindow.Right							= 800;
	csbiInfo.srWindow.Bottom						= 600;

	// We're forced to set up the colors at this point because for some reason the GetConsoleScreenBufferInfoEx() function doesn't return the color table properly,
	// thus they're all 0 when we call SetConsoleScreenBufferInfoEx() causing all the colors to be reset to COLOR_BLACK.
	// Ideally we would have a setConsoleColors() function to do this separately.
	for(uint32_t iColor = 0; iColor < 16; ++iColor)
		csbiInfo.ColorTable[iColor]						= palette[iColor];

	csbiInfo.wAttributes							= ::ftwl::ASCII_COLOR_WHITE;
	
	::SetConsoleScreenBufferInfoEx(handleConsoleOut, &csbiInfo);
}


// ------------------------------------------------- 
::ftwl::error_t								ftwl::consoleDestroy			(::ftwl::SScreenASCII& console)											{
	if(false == ::gpftw_isConsoleCreated)	// check if the console has been initialized.
		return -1; // return an error value

	::FreeConsole();	// Tells Windows to close the console

	::FILE*										
	stream											= 0;	::freopen_s(&stream, "CONOUT$", "w+", stdout);
	stream											= 0;	::freopen_s(&stream, "CONIN$", "r+", stdin);

	if(console.Colors		.size()) ::free(console.Colors		.begin());	// Release the memory acquired with malloc() back to the system so it can be reused by us or other programs.
	if(console.Characters	.size()) ::free(console.Characters	.begin());	// Release the memory acquired with malloc() back to the system so it can be reused by us or other programs.
	if(console.Palette		.size()) ::free(console.Palette		.begin());	// Release the memory acquired with malloc() back to the system so it can be reused by us or other programs.
	console.Colors									= {};
	console.Characters								= {};
	console.Palette									= {};

	::gpftw_isConsoleCreated						= false;	// set this to false so we can create it back later if we want to.
	return 0;	// these functions hardly fail.
}	

::ftwl::error_t								ftwl::consoleCreate			(::ftwl::SScreenASCII& console, int width, int height)						{
	if(::gpftw_isConsoleCreated)	// check if the console has been initialized.
		return -1; // return an error value

	console.Width									= width		;
	console.Height									= height	;
	console.Palette									= ::ftwl::array_view<uint32_t>((uint32_t*)::malloc(sizeof(uint32_t) * 16), 16);	// Ask the system to give us a memory block of the desired size for us to use. We need to return it back to the system once we're done using it.
	console.Palette[0 ]								= ::ftwl::ASCII_COLOR_INDEX_0 ;
	console.Palette[1 ]								= ::ftwl::ASCII_COLOR_INDEX_1 ;
	console.Palette[2 ]								= ::ftwl::ASCII_COLOR_INDEX_2 ;
	console.Palette[3 ]								= ::ftwl::ASCII_COLOR_INDEX_3 ;
	console.Palette[4 ]								= ::ftwl::ASCII_COLOR_INDEX_4 ;
	console.Palette[5 ]								= ::ftwl::ASCII_COLOR_INDEX_5 ;
	console.Palette[6 ]								= ::ftwl::ASCII_COLOR_INDEX_6 ;
	console.Palette[7 ]								= ::ftwl::ASCII_COLOR_INDEX_7 ;
	console.Palette[8 ]								= ::ftwl::ASCII_COLOR_INDEX_8 ;
	console.Palette[9 ]								= ::ftwl::ASCII_COLOR_INDEX_9 ;
	console.Palette[10]								= ::ftwl::ASCII_COLOR_INDEX_10;
	console.Palette[11]								= ::ftwl::ASCII_COLOR_INDEX_11;
	console.Palette[12]								= ::ftwl::ASCII_COLOR_INDEX_12;
	console.Palette[13]								= ::ftwl::ASCII_COLOR_INDEX_13;
	console.Palette[14]								= ::ftwl::ASCII_COLOR_INDEX_14;
	console.Palette[15]								= ::ftwl::ASCII_COLOR_INDEX_15;

	::initWindowsConsole			();
	::initWindowsConsoleFont		();
	::initWindowsConsoleProperties	(width, height, &console.Palette[0]);

	console.Characters								= ::ftwl::array_view<uint8_t	>((uint8_t	*)::malloc(sizeof(uint8_t	) * width * height), width * height);	// Ask the system to give us a memory block of the desired size for us to use. We need to return it back to the system once we're done using it.
	console.Colors									= ::ftwl::array_view<uint16_t	>((uint16_t	*)::malloc(sizeof(uint16_t	) * width * height), width * height);	// Ask the system to give us a memory block of the desired size for us to use. We need to return it back to the system once we're done using it.

	::SetConsoleTitle("ASCII Console for the Win");	// I don't need to explain this one, right?
	return 0;
}

::ftwl::error_t								ftwl::consolePresent			(::ftwl::SScreenASCII& console)											{
	if(false == ::gpftw_isConsoleCreated)	// check if the console has been initialized.
		return -1; // return an error value

	if(0 == console.Colors.size() || 0 == console.Characters.size())
		return -1; // return an error value

	::initWindowsConsoleProperties(console.Width, console.Height, &console.Palette[0]);

	int													screenSize				= console.Width * console.Height;

	const ::HANDLE										handleConsoleOut		= ::GetStdHandle( STD_OUTPUT_HANDLE );	// Get console output handle	
	::CONSOLE_SCREEN_BUFFER_INFO						csbiInfo				= {};
    ::GetConsoleScreenBufferInfo( handleConsoleOut, &csbiInfo );

	::COORD												Coords					= {0, csbiInfo.dwSize.Y - (::SHORT)console.Height};
	::DWORD												dummy					= 0;
	::WriteConsoleOutputCharacter ( handleConsoleOut, (const char*)	console.Characters	.begin(), screenSize, Coords, &dummy );
	::WriteConsoleOutputAttribute ( handleConsoleOut,				console.Colors		.begin(), screenSize, Coords, &dummy );

	return 0;
}

::ftwl::error_t								ftwl::consoleClear			(::ftwl::SScreenASCII& console)												{
	int													screenSize				= console.Width * console.Height;
	::memset(&console.Characters	[0], 0, sizeof(char		) * screenSize);
	::memset(&console.Colors		[0], 0, sizeof(short	) * screenSize);

	const ::HANDLE										handleConsoleOut		= ::GetStdHandle( STD_OUTPUT_HANDLE );	// Get console output handle
	::CONSOLE_SCREEN_BUFFER_INFO						csbiInfo				= {};
    ::GetConsoleScreenBufferInfo( handleConsoleOut, &csbiInfo );

	::COORD												coords					= {0, csbiInfo.dwSize.Y - (::SHORT)console.Height};
	::DWORD												dummy					= 0;
	::WriteConsoleOutputCharacter ( handleConsoleOut, (const char*)	console.Characters	.begin(), screenSize, coords, &dummy );
	::WriteConsoleOutputAttribute ( handleConsoleOut,				console.Colors		.begin(), screenSize, coords, &dummy );

	return 0;
}