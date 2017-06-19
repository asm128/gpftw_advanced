#include "gpftw_ascii_screen.h"

#include "gpftw_ascii_color.h"

#include <stdio.h>
#include <Windows.h>

// We need this function because we give its pointer to Windows so it can report to our program when CTRL_C was pressed
BOOL WINAPI								HandlerRoutine							(_In_ DWORD dwCtrlType)												{
	switch(dwCtrlType) {
	case CTRL_C_EVENT:
	default:
		return TRUE;
	}
}

static bool								gpftw_isConsoleCreated					= false;

void									initWindowsConsoleFont					()																	{
	if(false == ::gpftw_isConsoleCreated) // we don't create the console twice.
		return;

	const ::HANDLE								hConsoleOut								= ::GetStdHandle(STD_OUTPUT_HANDLE);
	::CONSOLE_FONT_INFOEX						infoFont;
	::GetCurrentConsoleFontEx( hConsoleOut, TRUE, &infoFont );

	infoFont.cbSize							= sizeof(::CONSOLE_FONT_INFOEX);
	infoFont.dwFontSize.X					= 8;
	infoFont.dwFontSize.Y					= 12;
	infoFont.FontFamily						= 0;
	infoFont.FontWeight						= 0;
	infoFont.nFont							= 0;
	::wcscpy_s(infoFont.FaceName, L"Terminal");
	::SetCurrentConsoleFontEx(hConsoleOut, FALSE, &infoFont);
}

void									initWindowsConsole						()																	{
	if(::gpftw_isConsoleCreated) // we don't create the console twice.
		return;

	::AllocConsole();		// This function tells windows to create a console for our program.
	::AttachConsole(GetCurrentProcessId());	// This function tells windows that we want to receive notifications from this process' console. This is so we can catch the CTRL_C event and prevent the program to terminate in an undesired way.
		
	// The following lines are needed to redirect printf() and other Input/Output functions to the newly created console.
	::FILE*										
	stream									= 0;	::freopen_s(&stream, "CONOUT$", "w+", stdout);
	stream									= 0;	::freopen_s(&stream, "CONIN$", "r+", stdin);

	// With this function we tell to Windows that we want to use the HandlerRoutine function to get the CTRL-C notification. 
	::SetConsoleCtrlHandler(HandlerRoutine, TRUE);	// What this does is to send the pointer to HandlerRoutine as the first parameter and Windows then can call the function through its address.

	::EnableMenuItem (	::GetSystemMenu ( ::GetConsoleWindow(), FALSE)	// We do this to grey out/disable the Cross Button used to close the window.
		,				SC_CLOSE
		,				MF_BYCOMMAND | MF_GRAYED 
		);

	// We also need to disable the Quick Edit mode because it breaks the display when you click with the mouse on the console as it tries to select the text and that's not what we want.
	// We need to be able to use the mouse the way we want i.e. to push buttons or drag-and-drop
	const ::HANDLE								hConsoleIn								= ::GetStdHandle(STD_INPUT_HANDLE);	// Get console input handle
	::DWORD										mode;
	::GetConsoleMode(hConsoleIn, &mode);
	mode									&= ~(::DWORD)ENABLE_QUICK_EDIT_MODE;
	::SetConsoleMode(hConsoleIn, mode);

	::gpftw_isConsoleCreated					= true;	// 
}


void									initWindowsConsoleProperties			(int width, int height, const uint32_t* palette)												{
	if(false == ::gpftw_isConsoleCreated) 	// we don't create the console twice.
		return;
	
	const ::HANDLE								hConsoleOut								= ::GetStdHandle(STD_OUTPUT_HANDLE);	// Get console output handle

	::CONSOLE_SCREEN_BUFFER_INFOEX				csbiInfo								= {};
	::GetConsoleScreenBufferInfoEx(hConsoleOut, &csbiInfo);
	csbiInfo.cbSize							= sizeof(::CONSOLE_SCREEN_BUFFER_INFOEX);
	csbiInfo.dwSize.X						= (::SHORT)width;
	csbiInfo.dwSize.Y						= (::SHORT)height;
	csbiInfo.dwMaximumWindowSize.X			= (::SHORT)width;
	csbiInfo.dwMaximumWindowSize.Y			= (::SHORT)height;

	csbiInfo.srWindow.Top					= 10;
	csbiInfo.srWindow.Left					= 10;
	csbiInfo.srWindow.Right					= 800;
	csbiInfo.srWindow.Bottom				= 600;

	// We're forced to set up the colors at this point because for some reason the GetConsoleScreenBufferInfoEx() function doesn't return the color table properly,
	// thus they're all 0 when we call SetConsoleScreenBufferInfoEx() causing all the colors to be reset to COLOR_BLACK.
	// Ideally we would have a setConsoleColors() function to do this separately.
	for(uint32_t iColor = 0; iColor < 16; ++iColor)
		csbiInfo.ColorTable[iColor]				= palette[iColor];

	csbiInfo.wAttributes					= ::gpftw::ASCII_COLOR_INDEX_15;
	
	::SetConsoleScreenBufferInfoEx( hConsoleOut, &csbiInfo );
}


// ------------------------------------------------- 
::gpftw::error_t						gpftw::destroyConsole			(::gpftw::SASCIIScreen& console)																{
	if(false == ::gpftw_isConsoleCreated)	// check if the console has been initialized.
		return -1; // return an error value

	::FreeConsole();	// Tells Windows to close the console

	::free(console.Colors		.begin());	// Release the memory acquired with malloc() back to the system so it can be reused by us or other programs.
	::free(console.Characters	.begin());	// Release the memory acquired with malloc() back to the system so it can be reused by us or other programs.
	::free(console.Palette		.begin());	// Release the memory acquired with malloc() back to the system so it can be reused by us or other programs.
	console.Colors							= {};
	console.Characters						= {};
	console.Palette							= {};

	::gpftw_isConsoleCreated					= false;	// set this to false so we can create it back later if we want to.
	return 0;	// these functions hardly fail.
}	

::gpftw::error_t						gpftw::createConsole			(::gpftw::SASCIIScreen& console, int width, int height)										{
	if(::gpftw_isConsoleCreated)	// check if the console has been initialized.
		return -1; // return an error value

	console.Width							= width		;
	console.Height							= height	;
	console.Palette							= ::gpftw::array_view<uint32_t>((uint32_t*)::malloc(sizeof(uint32_t) * 16), 16);	// Ask the system to give us a memory block of the desired size for us to use. We need to return it back to the system once we're done using it.
	console.Palette[0 ]						= ::gpftw::ASCII_COLOR_BLACK		;
	console.Palette[1 ]						= ::gpftw::ASCII_COLOR_DARKBLUE		;
	console.Palette[2 ]						= ::gpftw::ASCII_COLOR_DARKGREEN	;
	console.Palette[3 ]						= ::gpftw::ASCII_COLOR_DARKCYAN		;
	console.Palette[4 ]						= ::gpftw::ASCII_COLOR_DARKRED		;
	console.Palette[5 ]						= ::gpftw::ASCII_COLOR_DARKMAGENTA	;
	console.Palette[6 ]						= ::gpftw::ASCII_COLOR_DARKYELLOW	;
	console.Palette[7 ]						= ::gpftw::ASCII_COLOR_LIGHTGREY	;
	console.Palette[8 ]						= ::gpftw::ASCII_COLOR_DARKGREY		;
	console.Palette[9 ]						= ::gpftw::ASCII_COLOR_BLUE			;
	console.Palette[10]						= ::gpftw::ASCII_COLOR_GREEN		;
	console.Palette[11]						= ::gpftw::ASCII_COLOR_CYAN			;
	console.Palette[12]						= ::gpftw::ASCII_COLOR_RED			;
	console.Palette[13]						= ::gpftw::ASCII_COLOR_MAGENTA		;
	console.Palette[14]						= ::gpftw::ASCII_COLOR_YELLOW		;
	console.Palette[15]						= ::gpftw::ASCII_COLOR_WHITE		;

	::initWindowsConsole();
	::initWindowsConsoleFont();
	::initWindowsConsoleProperties(width, height, &console.Palette[0]);

	console.Characters						= ::gpftw::array_view<uint8_t	>((uint8_t	*)::malloc(sizeof(uint8_t	) * width * height), width * height);	// Ask the system to give us a memory block of the desired size for us to use. We need to return it back to the system once we're done using it.
	console.Colors							= ::gpftw::array_view<uint16_t	>((uint16_t	*)::malloc(sizeof(uint16_t	) * width * height), width * height);	// Ask the system to give us a memory block of the desired size for us to use. We need to return it back to the system once we're done using it.

	::SetConsoleTitle("ASCII Console for the Win");	// I don't need to explain this one, right?
	return 0;
}

::gpftw::error_t						gpftw::presentConsole			(::gpftw::SASCIIScreen& console)																{
	if(false == ::gpftw_isConsoleCreated)	// check if the console has been initialized.
		return -1; // return an error value

	if(0 == console.Colors.size() || 0 == console.Characters.size())
		return -1; // return an error value

	::initWindowsConsoleProperties(console.Width, console.Height, &console.Palette[0]);

	int											screenSize				= console.Width * console.Height;

	const ::HANDLE								hConsoleOut				= ::GetStdHandle( STD_OUTPUT_HANDLE );	// Get console output handle
	::CONSOLE_SCREEN_BUFFER_INFO				csbiInfo				= {};
    ::GetConsoleScreenBufferInfo( hConsoleOut, &csbiInfo );

	::COORD										Coords					= {0, csbiInfo.dwSize.Y - (::SHORT)console.Height};
	::DWORD										dummy					= 0;
	::WriteConsoleOutputCharacter ( hConsoleOut, (const char*)	console.Characters	.begin(), screenSize, Coords, &dummy );
	::WriteConsoleOutputAttribute ( hConsoleOut,				console.Colors		.begin(), screenSize, Coords, &dummy );

	return 0;
}

::gpftw::error_t						gpftw::clearConsole			(::gpftw::SASCIIScreen& console)																{
	int											screenSize				= console.Width * console.Height;
	::memset(&console.Characters	[0], 0, sizeof(char		) * screenSize);
	::memset(&console.Colors		[0], 0, sizeof(short	) * screenSize);

	const ::HANDLE								hConsoleOut				= ::GetStdHandle( STD_OUTPUT_HANDLE );	// Get console output handle
	::CONSOLE_SCREEN_BUFFER_INFO				csbiInfo				= {};
    ::GetConsoleScreenBufferInfo( hConsoleOut, &csbiInfo );

	::COORD										Coords					= {0, csbiInfo.dwSize.Y - (::SHORT)console.Height};
	::DWORD										dummy					= 0;
	::WriteConsoleOutputCharacter ( hConsoleOut, (const char*)	console.Characters	.begin(), screenSize, Coords, &dummy );
	::WriteConsoleOutputAttribute ( hConsoleOut,				console.Colors		.begin(), screenSize, Coords, &dummy );

	return 0;
}