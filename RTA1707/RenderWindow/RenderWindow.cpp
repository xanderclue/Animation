#include "stdafx.h"
#include "RenderWindow.h"
#include <iostream>
#include <chrono>
#include "GraphicsSystem.h"
#include "FBXDLL.h"

#define MAX_LOADSTRING 100

HINSTANCE g_hInst;
WCHAR g_szTitle[ MAX_LOADSTRING ];
WCHAR g_szWindowClass[ MAX_LOADSTRING ];
HWND g_hWnd;
GraphicsSystem g_graphicsSystem;
extern const int g_windowWidth = 1000;
extern const int g_windowHeight = 500;

ATOM MyRegisterClass( HINSTANCE );
BOOL InitInstance( HINSTANCE, int );
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
INT_PTR CALLBACK About( HWND, UINT, WPARAM, LPARAM );

TriangleMesh PositionTrianglesToMesh( const std::vector<PositionTriangle>&, const RGBAColor& = RGBAColor( 0.2f, 0.2f, 0.2f, 1.0f ) );
void DrawSkeleton( const std::vector<JointTransform>&, const DirectX::XMFLOAT4X4& = GraphicsSystem::IDENTITYMATRIX );
void DrawSkeletonAnimation( const AnimClip&, const DirectX::XMFLOAT4X4& = GraphicsSystem::IDENTITYMATRIX, double = 0.0 );

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

	DirectX::XMFLOAT4X4 teddyWorldMatrix_ = GraphicsSystem::IDENTITYMATRIX;
	XMStoreFloat4x4( &teddyWorldMatrix_, XMMatrixMultiply( XMLoadFloat4x4( &teddyWorldMatrix_ ), DirectX::XMMatrixScaling( 0.03f, 0.03f, 0.03f ) ) );
	XMStoreFloat4x4( &teddyWorldMatrix_, XMMatrixMultiply( XMLoadFloat4x4( &teddyWorldMatrix_ ), DirectX::XMMatrixRotationY( DirectX::XMConvertToRadians( 180.0f ) ) ) );
	XMStoreFloat4x4( &teddyWorldMatrix_, XMMatrixMultiply( XMLoadFloat4x4( &teddyWorldMatrix_ ), DirectX::XMMatrixTranslation( 3.0f, 0.0f, 0.0f ) ) );

	DirectX::XMFLOAT4X4 mageWorldMatrix_ = GraphicsSystem::IDENTITYMATRIX;
	XMStoreFloat4x4( &mageWorldMatrix_, XMMatrixMultiply( XMLoadFloat4x4( &mageWorldMatrix_ ), DirectX::XMMatrixRotationY( DirectX::XMConvertToRadians( 180.0f ) ) ) );
	XMStoreFloat4x4( &mageWorldMatrix_, XMMatrixMultiply( XMLoadFloat4x4( &mageWorldMatrix_ ), DirectX::XMMatrixTranslation( -3.0f, 0.0f, 0.0f ) ) );

#ifndef NDEBUG
	std::cout << "Init Graphics System...";
	g_graphicsSystem.InitializeGraphicsSystem(); std::cout << " Done\n";
	std::cout << "Reading Teddy_Run.fbx (Mesh)...";
	TriangleMesh teddyMesh_ = PositionTrianglesToMesh( FBXDLL::FBX_GetMeshBindPose( "Teddy_Run.fbx" ) ); std::cout << " Done\n";
	std::cout << "Adding Mesh Teddy_Run...";
	g_graphicsSystem.AddMesh( &teddyMesh_, teddyWorldMatrix_ ); std::cout << " Done\n";
	std::cout << "Reading Teddy_Run.fbx (Skeleton)...";
	std::vector<JointTransform> jointsTeddy_ = FBXDLL::FBX_GetJointsBindPose( "Teddy_Run.fbx" ); std::cout << " Done\n";
	std::cout << "Reading Mage_Run.fbx (Mesh)...";
	TriangleMesh mageMesh_ = PositionTrianglesToMesh( FBXDLL::FBX_GetMeshBindPose( "Mage_Run.fbx" ) ); std::cout << " Done\n";
	std::cout << "Adding Mesh Mage_Run...";
	g_graphicsSystem.AddMesh( &mageMesh_, mageWorldMatrix_ ); std::cout << " Done\n";
	std::cout << "Reading Mage_Run.fbx (Skeleton)...";
	std::vector<JointTransform> jointsMage_ = FBXDLL::FBX_GetJointsBindPose( "Mage_Run.fbx" ); std::cout << " Done\n";
	//std::cout << "Reading Teddy_Run.fbx (Animation)...";
	//AnimClip animTeddy_ = FBXDLL::FBX_GetAnimationData( "Teddy_Run.fbx" ); std::cout << " Done\n";
	//std::cout << "Reading Mage_Run.fbx (Animation)...";
	//AnimClip animMage_ = FBXDLL::FBX_GetAnimationData( "Mage_Run.fbx" ); std::cout << " Done\n";
#else
	g_graphicsSystem.InitializeGraphicsSystem();
	TriangleMesh mageMesh_ = PositionTrianglesToMesh( FBXDLL::FBX_GetMeshBindPose( "Mage_Run.fbx" ) );
	TriangleMesh teddyMesh_ = PositionTrianglesToMesh( FBXDLL::FBX_GetMeshBindPose( "Teddy_Run.fbx" ) );
	std::vector<JointTransform> jointsTeddy_ = FBXDLL::FBX_GetJointsBindPose( "Teddy_Run.fbx" );
	std::vector<JointTransform> jointsMage_ = FBXDLL::FBX_GetJointsBindPose( "Mage_Run.fbx" );
	g_graphicsSystem.AddMesh( &mageMesh_, mageWorldMatrix_ );
	g_graphicsSystem.AddMesh( &teddyMesh_, teddyWorldMatrix_ );
	//AnimClip animTeddy_ = FBXDLL::FBX_GetAnimationData( "Teddy_Run.fbx" );
	//AnimClip animMage_ = FBXDLL::FBX_GetAnimationData( "Mage_Run.fbx" );
#endif

	g_graphicsSystem.EnableDebugGraphics( true );
	long long t2_ = std::chrono::system_clock::now().time_since_epoch().count(), t1_;
	double time_ = 0.0;
	while ( WM_QUIT != msg_.message )
	{
		t1_ = t2_;
		t2_ = std::chrono::system_clock::now().time_since_epoch().count();
		g_graphicsSystem.GetCamera().Update( ( t2_ - t1_ ) * 0.0000001f );
		DrawSkeleton( jointsTeddy_, teddyWorldMatrix_ );
		DrawSkeleton( jointsMage_, mageWorldMatrix_ );
		time_ += ( t2_ - t1_ ) * 0.0000001;
		//DrawSkeletonAnimation( animTeddy_, teddyWorldMatrix_, time_ );
		//DrawSkeletonAnimation( animMage_, mageWorldMatrix_, time_ );
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

TriangleMesh PositionTrianglesToMesh( const std::vector<PositionTriangle>& _triangles, const RGBAColor& _color )
{
	TriangleMesh outMesh_;
	Triangle tempTriangle_;
	tempTriangle_.m_vertexA.m_color =
		tempTriangle_.m_vertexB.m_color =
		tempTriangle_.m_vertexC.m_color = _color.m_rgba;
	tempTriangle_.m_vertexA.m_position.w =
		tempTriangle_.m_vertexB.m_position.w =
		tempTriangle_.m_vertexC.m_position.w = 1.0f;
	for ( unsigned int i = 0u; i < _triangles.size(); ++i )
	{
		tempTriangle_.m_vertexA.m_position.x = _triangles[ i ].m_posA.x;
		tempTriangle_.m_vertexA.m_position.y = _triangles[ i ].m_posA.y;
		tempTriangle_.m_vertexA.m_position.z = _triangles[ i ].m_posA.z;
		tempTriangle_.m_vertexB.m_position.x = _triangles[ i ].m_posB.x;
		tempTriangle_.m_vertexB.m_position.y = _triangles[ i ].m_posB.y;
		tempTriangle_.m_vertexB.m_position.z = _triangles[ i ].m_posB.z;
		tempTriangle_.m_vertexC.m_position.x = _triangles[ i ].m_posC.x;
		tempTriangle_.m_vertexC.m_position.y = _triangles[ i ].m_posC.y;
		tempTriangle_.m_vertexC.m_position.z = _triangles[ i ].m_posC.z;
		outMesh_.AddTriangle( tempTriangle_ );
	}
	return outMesh_;
}

void DrawJoint( DirectX::XMFLOAT4X4 _transformationMatrix, const DirectX::XMFLOAT4X4& _worldMatrix )
{
	XMStoreFloat4x4( &_transformationMatrix, XMMatrixMultiply( XMLoadFloat4x4( &_transformationMatrix ), XMLoadFloat4x4( &_worldMatrix ) ) );
	PositionColorVertex xAxis_, yAxis_, zAxis_, position_;
	xAxis_.m_position = DirectX::XMFLOAT4(
		_transformationMatrix._11,
		_transformationMatrix._12,
		_transformationMatrix._13,
		_transformationMatrix._14 );
	yAxis_.m_position = DirectX::XMFLOAT4(
		_transformationMatrix._21,
		_transformationMatrix._22,
		_transformationMatrix._23,
		_transformationMatrix._24 );
	zAxis_.m_position = DirectX::XMFLOAT4(
		_transformationMatrix._31,
		_transformationMatrix._32,
		_transformationMatrix._33,
		_transformationMatrix._34 );
	position_.m_position = DirectX::XMFLOAT4(
		_transformationMatrix._41,
		_transformationMatrix._42,
		_transformationMatrix._43,
		_transformationMatrix._44 );
	XMStoreFloat4( &xAxis_.m_position, DirectX::XMVectorAdd( XMLoadFloat4( &position_.m_position ), XMLoadFloat4( &xAxis_.m_position ) ) );
	XMStoreFloat4( &yAxis_.m_position, DirectX::XMVectorAdd( XMLoadFloat4( &position_.m_position ), XMLoadFloat4( &yAxis_.m_position ) ) );
	XMStoreFloat4( &zAxis_.m_position, DirectX::XMVectorAdd( XMLoadFloat4( &position_.m_position ), XMLoadFloat4( &zAxis_.m_position ) ) );

	position_.m_color = xAxis_.m_color = RGBAColor::Red.m_rgba;
	g_graphicsSystem.AddDebugLine( position_, xAxis_ );
	position_.m_color = yAxis_.m_color = RGBAColor::Green.m_rgba;
	g_graphicsSystem.AddDebugLine( position_, yAxis_ );
	position_.m_color = zAxis_.m_color = RGBAColor::Blue.m_rgba;
	g_graphicsSystem.AddDebugLine( position_, zAxis_ );
}
void DrawBone( DirectX::XMFLOAT4X4 _transformA, DirectX::XMFLOAT4X4 _transformB, const DirectX::XMFLOAT4X4& _worldMatrix )
{
	XMStoreFloat4x4( &_transformA, XMMatrixMultiply( XMLoadFloat4x4( &_transformA ), XMLoadFloat4x4( &_worldMatrix ) ) );
	XMStoreFloat4x4( &_transformB, XMMatrixMultiply( XMLoadFloat4x4( &_transformB ), XMLoadFloat4x4( &_worldMatrix ) ) );
	g_graphicsSystem.AddDebugLine( PositionColorVertex( _transformA._41, _transformA._42, _transformA._43, RGBAColor::Cyan ),
								   PositionColorVertex( _transformB._41, _transformB._42, _transformB._43, RGBAColor::Cyan ) );
}
void DrawSkeleton( const std::vector<JointTransform>& _joints, const DirectX::XMFLOAT4X4& _worldMatrix )
{
	const unsigned int size_ = ( unsigned int )_joints.size();
	for ( unsigned int i = 0u; i < size_; ++i )
	{
		DrawJoint( _joints[ i ].m_transform, _worldMatrix );
		if ( _joints[ i ].m_parent >= 0 )
			DrawBone( _joints[ i ].m_transform, _joints[ _joints[ i ].m_parent ].m_transform, _worldMatrix );
	}
}

DirectX::XMFLOAT4X4 Interpolate( const DirectX::XMFLOAT4X4& _a, const DirectX::XMFLOAT4X4& _b, float _t )
{
	DirectX::XMFLOAT4X4 outMat_;
	DirectX::XMVECTOR scaleA_, rotA_, transA_, scaleB_, rotB_, transB_, scaleOut_, rotOut_, transOut_;
	DirectX::XMMatrixDecompose( &scaleA_, &rotA_, &transA_, XMLoadFloat4x4( &_a ) );
	DirectX::XMMatrixDecompose( &scaleB_, &rotB_, &transB_, XMLoadFloat4x4( &_b ) );

	scaleOut_ = DirectX::XMVectorLerp( scaleA_, scaleB_, _t );
	rotOut_ = DirectX::XMQuaternionSlerp( rotA_, rotB_, _t );
	transOut_ = DirectX::XMVectorLerp( transA_, transB_, _t );

	DirectX::XMMatrixAffineTransformation( scaleOut_, DirectX::XMVectorZero(), rotOut_, transOut_ );
	XMStoreFloat4x4( &outMat_, DirectX::XMMatrixAffineTransformation( scaleOut_, DirectX::XMVectorZero(), rotOut_, transOut_ ) );
	return outMat_;
}

JointTransform Interpolate( const JointTransform& _a, const JointTransform& _b, double _t )
{
	JointTransform outJoint_;
	outJoint_.m_parent = _a.m_parent;
	outJoint_.m_transform = Interpolate( _a.m_transform, _b.m_transform, ( float )_t );
	return outJoint_;
}

std::vector<JointTransform> Interpolate( const std::vector<JointTransform>& _a,
										 const std::vector<JointTransform>& _b, double _t )
{
	std::vector<JointTransform> outJoints_;
	for ( unsigned int i = 0u; i < _a.size(); ++i )
		outJoints_.push_back( Interpolate( _a[ i ], _b[ i ], _t ) );
	return outJoints_;
}

void DrawSkeletonAnimation( const AnimClip& _anim, const DirectX::XMFLOAT4X4& _world, double _time )
{
	std::vector<JointTransform> currFrame_;
	_time = fmod( _time, _anim.m_duration );
	unsigned int prevFrameIdx_ = 0u, nextFrameIdx_ = 0u;
	double prevFrameTime_ = -DBL_MAX;
	double nextFrameTime_ = DBL_MAX;
	for ( unsigned int i = 0u; i < _anim.m_frames.size(); ++i )
	{
		const double time_ = _anim.m_frames[ i ].m_time;
		if ( time_ > prevFrameTime_ )
		{
			prevFrameTime_ = time_;
			prevFrameIdx_ = i;
		}
		if ( time_ < nextFrameTime_ )
		{
			nextFrameTime_ = time_;
			nextFrameIdx_ = i;
		}
	}
	for ( unsigned int i = 0u; i < _anim.m_frames.size(); ++i )
	{
		const double frameTime_ = _anim.m_frames[ i ].m_time;
		if ( ( frameTime_ <= _time && frameTime_ >= prevFrameTime_ ) ||
			( prevFrameTime_ > _time && frameTime_ <= _time ) )
		{
			prevFrameTime_ = frameTime_;
			prevFrameIdx_ = i;
		}
		if ( ( frameTime_ >= _time && frameTime_ <= nextFrameTime_ ) ||
			( prevFrameTime_ > _time && frameTime_ <= _time ) )
		{
			nextFrameTime_ = frameTime_;
			nextFrameIdx_ = i;
		}
	}
	if ( nextFrameIdx_ == prevFrameIdx_ )
		currFrame_ = _anim.m_frames[ nextFrameIdx_ ].m_joints;
	else
		currFrame_ = Interpolate( _anim.m_frames[ prevFrameIdx_ ].m_joints,
								  _anim.m_frames[ nextFrameIdx_ ].m_joints,
								  ( _time - prevFrameTime_ ) / ( nextFrameTime_ - prevFrameTime_ ) );

	DrawSkeleton( currFrame_, _world );
}