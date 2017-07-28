#include "stdafx.h"
#include "RenderWindow.h"
#include <iostream>
#include <chrono>
#include "FBXDLL.h"
#include "RenderWindowHelper.h"

int APIENTRY wWinMain( _In_ HINSTANCE _hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int _nCmdShow )
{
	LoadStringW( _hInstance, IDS_APP_TITLE, g_szTitle, MAX_LOADSTRING );
	LoadStringW( _hInstance, IDC_RENDERWINDOW, g_szWindowClass, MAX_LOADSTRING );
	MyRegisterClass( _hInstance );

	if ( !InitInstance( _hInstance, _nCmdShow ) )
	{
		return FALSE;
	}
#ifndef NDEBUG
	AllocConsole();
	{
		FILE* new_std_in_out_;
		freopen_s( &new_std_in_out_, "CONOUT$", "w", stdout );
		freopen_s( &new_std_in_out_, "CONIN$", "r", stdin );
	}
	FBXDLL::TestFBX_PrintInfo( "Teddy_Run.fbx" );
#endif
	HACCEL hAccelTable_ = LoadAccelerators( _hInstance, MAKEINTRESOURCE( IDC_RENDERWINDOW ) );
	MSG msg_;
	ZEROSTRUCT( msg_ );

	DirectX::XMFLOAT4X4 teddyWorldMatrix_ = Renderer::GraphicsSystem::IDENTITYMATRIX;
	teddyWorldMatrix_ *= DirectX::XMMatrixScaling( 0.03f, 0.03f, 0.03f );
	teddyWorldMatrix_ *= DirectX::XMMatrixRotationY( DirectX::XMConvertToRadians( 180.0f ) );
	teddyWorldMatrix_ *= DirectX::XMMatrixTranslation( 3.0f, 0.0f, 0.0f );

	DirectX::XMFLOAT4X4 mageWorldMatrix_ = Renderer::GraphicsSystem::IDENTITYMATRIX;
	mageWorldMatrix_ *= DirectX::XMMatrixRotationY( DirectX::XMConvertToRadians( 180.0f ) );
	mageWorldMatrix_ *= DirectX::XMMatrixTranslation( -3.0f, 0.0f, 0.0f );

	g_graphicsSystem.InitializeGraphicsSystem();

#ifndef NDEBUG
	std::cout << "Loading Teddy_Run.fbx ( Mesh & Skeleton w/ Weights )...";
	std::vector<FBXDLL::JointTransform> jointsTeddy_;
	Renderer::TriangleMesh teddyMesh_ = PositionTrianglesToMesh( FBXDLL::FBX_GetMesh( "Teddy_Run.fbx", jointsTeddy_ ) );
	g_graphicsSystem.AddMesh( &teddyMesh_, teddyWorldMatrix_ ); std::cout << " Done\n";
	std::cout << "Loading Teddy_Run.fbx ( Animation Frames )...";
	FBXDLL::AnimClip animTeddy_ = FBXDLL::FBX_GetAnimationData( "Teddy_Run.fbx" ); std::cout << " Done\n";
	std::cout << "Loading Mage_Run.fbx ( Mesh & Skeleton w/ Weights )...";
	std::vector<FBXDLL::JointTransform> jointsMage_;
	Renderer::TriangleMesh mageMesh_ = PositionTrianglesToMesh( FBXDLL::FBX_GetMesh( "Mage_Run.fbx", jointsMage_ ) );
	g_graphicsSystem.AddMesh( &mageMesh_, mageWorldMatrix_ ); std::cout << " Done\n";
	std::cout << "Loading Mage_Run.fbx ( Animation Frames )...";
	FBXDLL::AnimClip animMage_ = FBXDLL::FBX_GetAnimationData( "Mage_Run.fbx" ); std::cout << " Done\n";
#else
	g_graphicsSystem.InitializeGraphicsSystem();
	std::vector<FBXDLL::JointTransform> jointsTeddy_;
	std::vector<FBXDLL::JointTransform> jointsMage_;
	Renderer::TriangleMesh mageMesh_ = PositionTrianglesToMesh( FBXDLL::FBX_GetMesh( "Mage_Run.fbx", jointsMage_ ) );
	Renderer::TriangleMesh teddyMesh_ = PositionTrianglesToMesh( FBXDLL::FBX_GetMesh( "Teddy_Run.fbx", jointsTeddy_ ) );
	FBXDLL::AnimClip animTeddy_ = FBXDLL::FBX_GetAnimationData( "Teddy_Run.fbx" );
	FBXDLL::AnimClip animMage_ = FBXDLL::FBX_GetAnimationData( "Mage_Run.fbx" );
	g_graphicsSystem.AddMesh( &mageMesh_, mageWorldMatrix_ );
	g_graphicsSystem.AddMesh( &teddyMesh_, teddyWorldMatrix_ );
#endif

	std::vector<DirectX::XMFLOAT4X4>
		jointTransformsTeddy_, jointTransformsMage_,
		jointTransformsTeddyInvBind_, jointTransformsMageInvBind_;
	jointTransformsTeddy_.reserve( jointsTeddy_.size() );
	jointTransformsMage_.reserve( jointsMage_.size() );
	unsigned int i;
	for ( i = 0u; i < ( unsigned int )jointsTeddy_.size(); ++i )
	{
		jointTransformsTeddy_.push_back( jointsTeddy_[ i ].m_transform );
		jointTransformsTeddyInvBind_.push_back( GetMatrixInverse( jointsTeddy_[ i ].m_transform ) );
	}
	for ( i = 0u; i < ( unsigned int )jointsMage_.size(); ++i )
	{
		jointTransformsMage_.push_back( jointsMage_[ i ].m_transform );
		jointTransformsMageInvBind_.push_back( GetMatrixInverse( jointsMage_[ i ].m_transform ) );
	}
	teddyMesh_.m_numJoints = ( int )jointsTeddy_.size();
	mageMesh_.m_numJoints = ( int )jointsMage_.size();
	teddyMesh_.m_joints = &( jointTransformsTeddy_[ 0u ] );
	mageMesh_.m_joints = &( jointTransformsMage_[ 0u ] );
	teddyMesh_.m_invBindJoints = &( jointTransformsTeddyInvBind_[ 0u ] );
	mageMesh_.m_invBindJoints = &( jointTransformsMageInvBind_[ 0u ] );

	//WriteBinFile( "TeddyRun.bin", teddyMesh_, animTeddy_ );
	//WriteBinFile( "MageRun.bin", mageMesh_, animMage_ );

	g_graphicsSystem.EnableDebugGraphics( true );
	long long t2_ = std::chrono::system_clock::now().time_since_epoch().count(), t1_;
	double time_ = 0.0;
	bool tweening_ = true;
	while ( WM_QUIT != msg_.message )
	{
		CheckKeyToggleWireframe();
		CheckKeyToggleTweening( tweening_ );
		t1_ = t2_;
		t2_ = std::chrono::system_clock::now().time_since_epoch().count();
		g_graphicsSystem.GetCamera().Update( ( t2_ - t1_ ) * 0.0000001f );
		time_ += ( t2_ - t1_ ) * 0.0000001;
		DrawSkeletonAnimation( animTeddy_, teddyWorldMatrix_, time_, tweening_, jointTransformsTeddy_ );
		DrawSkeletonAnimation( animMage_, mageWorldMatrix_, time_, tweening_, jointTransformsMage_ );
		g_graphicsSystem.DrawFrame();
		if ( PeekMessage( &msg_, nullptr, 0, 0, PM_REMOVE ) )
		{
			if ( !TranslateAccelerator( msg_.hwnd, hAccelTable_, &msg_ ) )
			{
				TranslateMessage( &msg_ );
				DispatchMessage( &msg_ );
			}
		}
	}
	CleanUp();
	g_graphicsSystem.ReleaseAll();

#ifndef NDEBUG
	FreeConsole();
#endif
	return ( int )msg_.wParam;
}
ATOM MyRegisterClass( HINSTANCE _hInstance )
{
	WNDCLASSEXW wcex_;

	wcex_.cbSize = sizeof( WNDCLASSEX );

	wcex_.style = CS_HREDRAW | CS_VREDRAW;
	wcex_.lpfnWndProc = WndProc;
	wcex_.cbClsExtra = 0;
	wcex_.cbWndExtra = 0;
	wcex_.hInstance = _hInstance;
	wcex_.hIcon = LoadIcon( _hInstance, MAKEINTRESOURCE( IDI_RENDERWINDOW ) );
	wcex_.hCursor = LoadCursor( nullptr, IDC_ARROW );
	wcex_.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
	wcex_.lpszMenuName = nullptr;
	wcex_.lpszClassName = g_szWindowClass;
	wcex_.hIconSm = LoadIcon( wcex_.hInstance, MAKEINTRESOURCE( IDI_SMALL ) );

	return RegisterClassExW( &wcex_ );
}
BOOL InitInstance( HINSTANCE _hInstance, int _nCmdShow )
{
	g_hInst = _hInstance;

	g_hWnd = CreateWindowW( g_szWindowClass, g_szTitle, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
							CW_USEDEFAULT, 0, g_windowWidth, g_windowHeight, nullptr, nullptr, _hInstance, nullptr );

	if ( !g_hWnd )
		return FALSE;

	ShowWindow( g_hWnd, _nCmdShow );
	UpdateWindow( g_hWnd );

	return TRUE;
}
LRESULT CALLBACK WndProc( HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam )
{
	switch ( _message )
	{
		case WM_DESTROY:
			PostQuitMessage( 0 );
			break;
		default:
			return DefWindowProc( _hWnd, _message, _wParam, _lParam );
	}
	return 0ll;
}
