#include "stdafx.h"
#include "RenderWindow.h"
#include <iostream>
#include "GraphicsSystem.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE g_hInst;
WCHAR szTitle[ MAX_LOADSTRING ];
WCHAR szWindowClass[ MAX_LOADSTRING ];
HWND g_hWnd;
GraphicsSystem g_graphicsSystem;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass( HINSTANCE );
BOOL                InitInstance( HINSTANCE, int );
LRESULT CALLBACK    WndProc( HWND, UINT, WPARAM, LPARAM );
INT_PTR CALLBACK    About( HWND, UINT, WPARAM, LPARAM );

int APIENTRY wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
					   _In_ LPWSTR lpCmdLine, _In_ int nCmdShow )
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );
	// TODO: Place code here.

	// Initialize global strings
	LoadStringW( hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING );
	LoadStringW( hInstance, IDC_RENDERWINDOW, szWindowClass, MAX_LOADSTRING );
	MyRegisterClass( hInstance );

	if ( !InitInstance( hInstance, nCmdShow ) )
	{
		return FALSE;
	}
#ifndef NDEBUG
	AllocConsole();
	{
		FILE* new_std_in_out;
		freopen_s( &new_std_in_out, "CONOUT$", "w", stdout );
		freopen_s( &new_std_in_out, "CONIN$", "r", stdin );
	}
	std::cout << "Hello world!\n";
#endif
	HACCEL hAccelTable = LoadAccelerators( hInstance, MAKEINTRESOURCE( IDC_RENDERWINDOW ) );
	MSG msg_;
	ZEROSTRUCT( msg_ );

	g_graphicsSystem.InitializeGraphicsSystem();

	while ( WM_QUIT != msg_.message )
	{
		if ( PeekMessage( &msg_, nullptr, 0, 0, PM_REMOVE ) )
		{
			if ( !TranslateAccelerator( msg_.hwnd, hAccelTable, &msg_ ) )
			{
				TranslateMessage( &msg_ );
				DispatchMessage( &msg_ );
			}
		}
	}

#ifndef NDEBUG
	FreeConsole();
#endif
	return ( int )msg_.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass( HINSTANCE hInstance )
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof( WNDCLASSEX );

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE( IDI_RENDERWINDOW ) );
	wcex.hCursor = LoadCursor( nullptr, IDC_ARROW );
	wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon( wcex.hInstance, MAKEINTRESOURCE( IDI_SMALL ) );

	return RegisterClassExW( &wcex );
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance( HINSTANCE hInstance, int nCmdShow )
{
	g_hInst = hInstance; // Store instance handle in our global variable

	g_hWnd = CreateWindowW( szWindowClass, szTitle, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
							CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr );

	if ( !g_hWnd )
	{
		return FALSE;
	}

	ShowWindow( g_hWnd, nCmdShow );
	UpdateWindow( g_hWnd );

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch ( message )
	{
		case WM_DESTROY:
			PostQuitMessage( 0 );
			break;
		default:
			return DefWindowProc( hWnd, message, wParam, lParam );
	}
	return 0;
}