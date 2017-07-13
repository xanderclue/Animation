#include "stdafx.h"
#include "graphics_structs.h"

#pragma comment(lib, "d3d11.lib")

const RGBAColor RGBAColor::Red = RGBAColor( 1.0f, 0.0f, 0.0f );
const RGBAColor RGBAColor::Green = RGBAColor( 0.0f, 1.0f, 0.0f );
const RGBAColor RGBAColor::Blue = RGBAColor( 0.0f, 0.0f, 1.0f );
const RGBAColor RGBAColor::Cyan = RGBAColor( 0.0f, 1.0f, 1.0f );
const RGBAColor RGBAColor::Magenta = RGBAColor( 1.0f, 0.0f, 1.0f );
const RGBAColor RGBAColor::Yellow = RGBAColor( 1.0f, 1.0f, 0.0f );
const RGBAColor RGBAColor::Black = RGBAColor( 0.0f, 0.0f, 0.0f );
const RGBAColor RGBAColor::White = RGBAColor( 1.0f, 1.0f, 1.0f );
RGBAColor::RGBAColor( void )
{
}
RGBAColor::RGBAColor( float _r, float _g, float _b, float _a ) :
	m_red( _r ), m_green( _g ), m_blue( _b ), m_alpha( _a )
{
}
PositionColorVertex::PositionColorVertex( void )
{
}
PositionColorVertex::PositionColorVertex( float _x, float _y, float _z, const RGBAColor& _color ) :
	m_position( _x, _y, _z, 1.0f ), m_color( _color.m_rgba )
{
}
PositionColorVertex::PositionColorVertex( float _x, float _y, float _z, float _u, float _v ) :
	m_position( _x, _y, _z, 1.0f ), m_color( _u, _v, 0.2f, 1.0f )
{
}
Camera::Camera( void )
{
	XMStoreFloat4x4( &m_cameraTransform, DirectX::XMMatrixIdentity() );
}
void Camera::TranslateCamera( float _x, float _y, float _z )
{
	XMStoreFloat4x4( &m_cameraTransform, XMMatrixMultiply(
		DirectX::XMMatrixTranslation( _x, _y, _z ),
		XMLoadFloat4x4( &m_cameraTransform ) ) );
}
void Camera::RotateCamera( float _x, float _y )
{
	const float x_ = m_cameraTransform._41;
	const float y_ = m_cameraTransform._42;
	const float z_ = m_cameraTransform._43;

	m_cameraTransform._41 = 0.0f;
	m_cameraTransform._42 = 0.0f;
	m_cameraTransform._43 = 0.0f;

	XMStoreFloat4x4(
		&m_cameraTransform,
		XMMatrixMultiply( XMMatrixMultiply(
		DirectX::XMMatrixRotationX( DirectX::XMConvertToRadians( _x ) ),
		XMLoadFloat4x4( &m_cameraTransform ) ),
		DirectX::XMMatrixRotationY( DirectX::XMConvertToRadians( _y ) ) ) );
	m_cameraTransform._41 = x_;
	m_cameraTransform._42 = y_;
	m_cameraTransform._43 = z_;
}
DirectX::XMMATRIX Camera::GetViewMatrix( void ) const
{
	return XMMatrixTranspose( XMMatrixInverse( nullptr, XMLoadFloat4x4( &m_cameraTransform ) ) );
}
void Camera::Update( float _dt )
{
	static struct
	{
		float x = 0.0f, y = 0.0f;
	} currMousePos_, prevMousePos_;
	{
		POINT p_;
		GetCursorPos( &p_ );
		currMousePos_.x = ( float )p_.x;
		currMousePos_.y = ( float )p_.y;
	}

	_dt *= 1.5f;
	{
		float x_ = 0.0f, y_ = 0.0f, z_ = 0.0f;
		if ( GetAsyncKeyState( 'W' ) ) z_ += _dt;
		if ( GetAsyncKeyState( 'S' ) ) z_ -= _dt;
		if ( GetAsyncKeyState( 'A' ) ) x_ -= _dt;
		if ( GetAsyncKeyState( 'D' ) ) x_ += _dt;
		if ( GetAsyncKeyState( VK_SHIFT ) ) y_ -= _dt;
		if ( GetAsyncKeyState( VK_SPACE ) ) y_ += _dt;
		TranslateCamera( x_, y_, z_ );
	}

	if ( GetAsyncKeyState( VK_LBUTTON ) ||
		 GetAsyncKeyState( VK_MBUTTON ) ||
		 GetAsyncKeyState( VK_RBUTTON ) )
	{
		const float dx_ = currMousePos_.x - prevMousePos_.x;
		const float dy_ = currMousePos_.y - prevMousePos_.y;
		_dt *= 25.0f;
		RotateCamera( dy_ * _dt, dx_ * _dt );
	}
	prevMousePos_ = currMousePos_;
}
Triangle::Triangle( void )
{
}
Triangle::Triangle( const PositionColorVertex& _vertexA,
					const PositionColorVertex& _vertexB,
					const PositionColorVertex& _vertexC ) :
	m_vertexA( _vertexA ),
	m_vertexB( _vertexB ),
	m_vertexC( _vertexC )
{
}
Triangle::Triangle( const Triangle& _rhs ) :
	m_vertexA( _rhs.m_vertexA ),
	m_vertexB( _rhs.m_vertexB ),
	m_vertexC( _rhs.m_vertexC )
{
}
Triangle& Triangle::operator=( const Triangle& _rhs )
{
	if ( this != &_rhs )
	{
		m_vertexA = _rhs.m_vertexA;
		m_vertexB = _rhs.m_vertexB;
		m_vertexC = _rhs.m_vertexC;
	}
	return *this;
}
TriangleMesh::TriangleMesh( void ) :
	m_triangles( nullptr ),
	m_numTriangles( 0u ),
	m_arrSize( 0u )
{
}
TriangleMesh::TriangleMesh( const TriangleMesh& _rhs ) :
	m_numTriangles( _rhs.m_numTriangles ),
	m_arrSize( _rhs.m_arrSize )
{
	if ( 0u != m_arrSize )
	{
		m_triangles = new Triangle[ m_arrSize ];
		for ( unsigned int i = 0u; i < m_numTriangles; ++i )
			m_triangles[ i ] = _rhs.m_triangles[ i ];
	}
	else m_triangles = nullptr;
}
TriangleMesh::TriangleMesh( TriangleMesh&& _rhs ) :
	m_triangles( _rhs.m_triangles ),
	m_numTriangles( _rhs.m_numTriangles ),
	m_arrSize( _rhs.m_arrSize )
{
	_rhs.m_triangles = nullptr;
}
TriangleMesh& TriangleMesh::operator=( const TriangleMesh& _rhs )
{
	if ( this != &_rhs )
	{
		delete[ ] m_triangles;
		m_triangles = _rhs.m_triangles;
		m_numTriangles = _rhs.m_numTriangles;
		if ( 0u != m_arrSize )
		{
			m_triangles = new Triangle[ m_arrSize ];
			for ( unsigned int i = 0u; i < m_numTriangles; ++i )
				m_triangles[ i ] = _rhs.m_triangles[ i ];
		}
		else m_triangles = nullptr;
	}
	return *this;
}
TriangleMesh& TriangleMesh::operator=( TriangleMesh&& _rhs )
{
	if ( this != &_rhs )
	{
		delete[ ] m_triangles;
		m_triangles = _rhs.m_triangles;
		m_numTriangles = _rhs.m_numTriangles;
		m_arrSize = _rhs.m_arrSize;
		_rhs.m_triangles = nullptr;
	}
	return *this;
}
TriangleMesh::~TriangleMesh( void )
{
	delete[ ] m_triangles;
}
void TriangleMesh::InitSize( unsigned int _size )
{
	delete[ ] m_triangles;
	if ( 0u != ( m_arrSize = _size ) )
		m_triangles = new Triangle[ m_arrSize ];
	else m_triangles = nullptr;
	m_numTriangles = 0u;
}
void TriangleMesh::AddTriangle( const Triangle& _triangle )
{
	if ( m_numTriangles >= m_arrSize )
		ExtendDynArray( m_triangles, m_arrSize );
	m_triangles[ m_numTriangles ] = _triangle;
	++m_numTriangles;
}
void TriangleMesh::Clear( void )
{
	m_numTriangles = 0u;
}
const Triangle* TriangleMesh::GetTriangles( void ) const
{
	return m_triangles;
}
unsigned int TriangleMesh::GetNumTriangles( void ) const
{
	return m_numTriangles;
}
