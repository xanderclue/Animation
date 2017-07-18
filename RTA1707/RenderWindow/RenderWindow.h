#pragma once
#include "resource.h"
#include "GraphicsSystem.h"

#define MAX_LOADSTRING 100

HINSTANCE g_hInst;
WCHAR g_szTitle[ MAX_LOADSTRING ];
WCHAR g_szWindowClass[ MAX_LOADSTRING ];
HWND g_hWnd;
Renderer::GraphicsSystem g_graphicsSystem;
extern const int g_windowWidth = 1000;
extern const int g_windowHeight = 500;

ATOM MyRegisterClass( HINSTANCE );
BOOL InitInstance( HINSTANCE, int );
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
INT_PTR CALLBACK About( HWND, UINT, WPARAM, LPARAM );
