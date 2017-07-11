#include "stdafx.h"
#include "GraphicsSystem.h"
#include <D3Dcompiler.h>
#include <stdio.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
extern HWND g_hWnd;
extern const int g_windowWidth;
extern const int g_windowHeight;

const DirectX::XMFLOAT4X4 GraphicsSystem::IDENTITYMATRIX = DirectX::XMFLOAT4X4( 1.0f, 0.0f, 0.0f, 0.0f,
																				0.0f, 1.0f, 0.0f, 0.0f,
																				0.0f, 0.0f, 1.0f, 0.0f,
																				0.0f, 0.0f, 0.0f, 1.0f );

struct HRESULT2
{
	HRESULT2( void )
	{
	}
	HRESULT& operator=( HRESULT&& _hResult )
	{
#ifndef NDEBUG
		if ( S_OK != _hResult )
		{
			printf( "\n\nERROR: %d\n\n", ( int )( _hResult ) );
			throw;
		}
#endif
		return _hResult;
	}
} g_hResult;

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
template <typename T>
void ExtendDynArray( T*& _arr, unsigned int& _size )
{
	if ( 0u == _size )
	{
		delete[ ] _arr;
		_arr = new T[ _size = 1u ];
	}
	else
	{
		T* const newArr_ = new T[ _size << 1 ];
		for ( unsigned int i = 0u; i < _size; ++i )
			newArr_[ i ] = _arr[ i ];
		delete[ ] _arr;
		_arr = newArr_;
		_size <<= 1;
	}
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
void GraphicsSystem::AddMesh( const TriangleMesh* _mesh, const DirectX::XMFLOAT4X4& _world )
{
	m_meshes.push_back( _mesh );
	m_worldMatrices.push_back( _world );
}
void GraphicsSystem::ClearMeshes( void )
{
	m_meshes.clear();
	m_worldMatrices.clear();
}
void GraphicsSystem::AddDebugLine( const PositionColorVertex& _a, const PositionColorVertex& _b )
{
	if ( m_DEBUG_LINES_arraySize == m_DEBUG_LINES_numVertices )
		ExtendDynArray( m_DEBUG_LINES_vertexArray, m_DEBUG_LINES_arraySize );
	m_DEBUG_LINES_vertexArray[ m_DEBUG_LINES_numVertices ] = _a;
	++m_DEBUG_LINES_numVertices;

	if ( m_DEBUG_LINES_arraySize == m_DEBUG_LINES_numVertices )
		ExtendDynArray( m_DEBUG_LINES_vertexArray, m_DEBUG_LINES_arraySize );
	m_DEBUG_LINES_vertexArray[ m_DEBUG_LINES_numVertices ] = _b;
	++m_DEBUG_LINES_numVertices;
}
void GraphicsSystem::DrawDebugGraphics( void )
{
	if ( 0u == m_DEBUG_LINES_numVertices )
		return;

	static const UINT stride_ = sizeof( PositionColorVertex );
	static const UINT offset_ = 0u;
	D3D11_BUFFER_DESC bufferDesc_;
	ZEROSTRUCT( bufferDesc_ );
	ID3D11Buffer* vertexBuffer_;
	D3D11_MAPPED_SUBRESOURCE mappedResource_;

	bufferDesc_.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_.ByteWidth = m_DEBUG_LINES_numVertices * sizeof( PositionColorVertex );
	bufferDesc_.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc_.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	g_hResult = m_device->CreateBuffer( &bufferDesc_, nullptr, &vertexBuffer_ );
	g_hResult = m_deviceContext->Map( vertexBuffer_, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource_ );
	memcpy( mappedResource_.pData, m_DEBUG_LINES_vertexArray, m_DEBUG_LINES_numVertices * sizeof( PositionColorVertex ) );
	m_deviceContext->Unmap( vertexBuffer_, 0u );
	m_deviceContext->IASetVertexBuffers( 0, 1, &vertexBuffer_, &stride_, &offset_ );
	m_deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_LINELIST );

	m_deviceContext->Draw( m_DEBUG_LINES_numVertices, 0u );

	vertexBuffer_->Release();
}
void GraphicsSystem::DrawMeshes( void )
{
	const unsigned int size_ = ( unsigned int )m_meshes.size();
	if ( 0u != size_ )
		for ( unsigned int i = 0u; i < size_; ++i )
			DrawMesh( *( m_meshes[ i ] ), m_worldMatrices[ i ] );
	else
	{
		static const UINT stride_ = sizeof( PositionColorVertex );
		static const UINT offset_ = 0u;
		ID3D11DeviceContext& deviceContext_ = *m_deviceContext;
		deviceContext_.IASetVertexBuffers( 0, 1, &m_defaultVertexBuffer, &stride_, &offset_ );
		deviceContext_.IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		//deviceContext_.Draw( m_defaultVertexCount, 0u );
	}
}
void GraphicsSystem::SetMvpBuffer( void )
{
	m_modelViewProjection.m_model = IDENTITYMATRIX;
	DirectX::XMStoreFloat4x4( &m_modelViewProjection.m_view, m_mainCamera.GetViewMatrix() );

	m_deviceContext->UpdateSubresource( m_modelViewProjectionBuffer, 0u, nullptr, &m_modelViewProjection, 0u, 0u );
	m_deviceContext->VSSetConstantBuffers( 0u, 1u, &m_modelViewProjectionBuffer );
}
void GraphicsSystem::DrawMesh( const TriangleMesh& _mesh, const DirectX::XMFLOAT4X4& _world )
{
	const unsigned int numTriangles_ = _mesh.GetNumTriangles();
	const unsigned int numVertices_ = numTriangles_ * 3u;
	const Triangle* const triangles_ = _mesh.GetTriangles();
	PositionColorVertex* vertices_ = new PositionColorVertex[ numVertices_ ];
	unsigned int i, j;
	for ( i = 0u; i < numTriangles_; ++i )
		for ( j = 0u; j < 3u; ++j )
			vertices_[ i * 3u + j ] = triangles_[ i ].m_vertices[ j ];
	ID3D11Buffer* vertexBuffer_;
	D3D11_BUFFER_DESC bufferDesc_;
	ZEROSTRUCT( bufferDesc_ );
	D3D11_MAPPED_SUBRESOURCE mappedResource_;
	bufferDesc_.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_.ByteWidth = sizeof( PositionColorVertex ) * numVertices_;
	bufferDesc_.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc_.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	g_hResult = m_device->CreateBuffer( &bufferDesc_, nullptr, &vertexBuffer_ );
	g_hResult = m_deviceContext->Map( vertexBuffer_, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource_ );
	memcpy( mappedResource_.pData, vertices_, sizeof( PositionColorVertex ) * numVertices_ );
	m_deviceContext->Unmap( vertexBuffer_, 0u );
	static const UINT stride_ = sizeof( PositionColorVertex );
	static const UINT offset_ = 0u;
	m_deviceContext->IASetVertexBuffers( 0, 1, &vertexBuffer_, &stride_, &offset_ );
	m_deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	XMStoreFloat4x4( &m_modelViewProjection.m_model, XMMatrixTranspose( XMLoadFloat4x4( &_world ) ) );
	m_deviceContext->UpdateSubresource( m_modelViewProjectionBuffer, 0u, nullptr, &m_modelViewProjection, 0u, 0u );
	m_deviceContext->VSSetConstantBuffers( 0u, 1u, &m_modelViewProjectionBuffer );
	m_deviceContext->Draw( numVertices_, 0u );
	vertexBuffer_->Release();
	delete[ ] vertices_;
}
GraphicsSystem::GraphicsSystem( void ) :
	m_debugRendererEnabled( true ),
	m_DEBUG_LINES_arraySize( 1024u ),
	m_DEBUG_LINES_numVertices( 0u )
{
	ZEROSTRUCT( m_defaultPipeline );
	m_mainCamera.TranslateCamera( 0.0f, 5.0f, -7.0f );
	m_mainCamera.RotateCamera( 28.0f, 0.0f );
	m_DEBUG_LINES_vertexArray = new PositionColorVertex[ m_DEBUG_LINES_arraySize ];
}
GraphicsSystem::~GraphicsSystem( void )
{
	delete[ ] m_DEBUG_LINES_vertexArray;
}
void GraphicsSystem::InitializeViewport( void )
{
	ZEROSTRUCT( m_viewport );
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	m_viewport.Width = ( FLOAT )g_windowWidth;
	m_viewport.Height = ( FLOAT )g_windowHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
}
void GraphicsSystem::SetPipelineStages( PipelineState* _pipelineState )
{
	m_deviceContext->OMSetDepthStencilState( _pipelineState->m_depthStencilState, 1u );
	m_deviceContext->OMSetRenderTargets( 1u, &_pipelineState->m_renderTargetView, _pipelineState->m_depthStencilView );
	m_deviceContext->RSSetState( _pipelineState->m_rasterizerState );
	m_deviceContext->VSSetShader( _pipelineState->m_vertexShader, nullptr, 0u );
	m_deviceContext->PSSetShader( _pipelineState->m_pixelShader, nullptr, 0u );
	m_deviceContext->IASetInputLayout( _pipelineState->m_inputLayout );
}
void GraphicsSystem::SetupDefaultBuffer( void )
{
	static const PositionColorVertex triangle_[ ] =
	{
		PositionColorVertex( 0.0f, 1.0f, 0.5f, RGBAColor::Red ),
		PositionColorVertex( -1.0f, -1.0f, 0.5f, RGBAColor::Green ),
		PositionColorVertex( 1.0f, -1.0f, 0.5f, RGBAColor::Blue )
	};
	m_defaultVertexCount = 3u;

	D3D11_BUFFER_DESC bufferDesc_;
	ZEROSTRUCT( bufferDesc_ );
	D3D11_MAPPED_SUBRESOURCE mappedResource_;

	bufferDesc_.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_.ByteWidth = sizeof( triangle_ );
	bufferDesc_.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc_.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	g_hResult = m_device->CreateBuffer( &bufferDesc_, nullptr, &m_defaultVertexBuffer );
	g_hResult = m_deviceContext->Map( m_defaultVertexBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource_ );
	memcpy( mappedResource_.pData, triangle_, sizeof( triangle_ ) );
	m_deviceContext->Unmap( m_defaultVertexBuffer, 0u );
}
void GraphicsSystem::InitializeMvpBuffer( void )
{
	static const CD3D11_BUFFER_DESC mvpBufferDesc_( sizeof( ModelViewProjection ), D3D11_BIND_CONSTANT_BUFFER );
	g_hResult = m_device->CreateBuffer( &mvpBufferDesc_, nullptr, &m_modelViewProjectionBuffer );
	static const float aspect_ = ( float )g_windowWidth / ( float )g_windowHeight;
	static const float fov_ = DirectX::XMConvertToRadians( 70.0f );
	static const float near_ = 0.01f;
	static const float far_ = 100.0f;
	XMStoreFloat4x4( &m_modelViewProjection.m_projection, XMMatrixTranspose( DirectX::XMMatrixPerspectiveFovLH( fov_, aspect_, near_, far_ ) ) );
}
void GraphicsSystem::InitializeGraphicsSystem( void )
{
	InitializeDeviceContextChain();
	InitializeDepthStencilBuffer();
	InitializeDepthStencilState();
	InitializeDepthStencilView();
	InitializeRasterizerState();
	InitializeViewport();
	InitializeShadersAndInputLayout();
	InitializeMvpBuffer();
	SetupDefaultBuffer();
	SetPipelineStages( &m_defaultPipeline );
	m_deviceContext->RSSetViewports( 1u, &m_viewport );
}
void GraphicsSystem::InitializeDeviceContextChain( void )
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc_;
	ZEROSTRUCT( swapChainDesc_ );
	static const D3D_FEATURE_LEVEL featureLevel_ = D3D_FEATURE_LEVEL_11_0;
	ID3D11Texture2D* backBuffer_;

	swapChainDesc_.BufferCount = 1u;
	swapChainDesc_.BufferDesc.Width = g_windowWidth;
	swapChainDesc_.BufferDesc.Height = g_windowHeight;
	swapChainDesc_.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc_.OutputWindow = g_hWnd;
	swapChainDesc_.SampleDesc.Count = 1u;
	swapChainDesc_.SampleDesc.Quality = 0u;
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc_.Windowed = TRUE;
	g_hResult = D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0u,
											   &featureLevel_, 1u, D3D11_SDK_VERSION,
											   &swapChainDesc_, &m_swapChain,
											   &m_device, nullptr, &m_deviceContext );

	g_hResult = m_swapChain->GetBuffer( 0u, __uuidof( ID3D11Texture2D ), ( void** )( &backBuffer_ ) );
	g_hResult = m_device->CreateRenderTargetView( backBuffer_, nullptr, &m_defaultPipeline.m_renderTargetView );
	backBuffer_->Release();
}
void GraphicsSystem::InitializeDepthStencilBuffer( void )
{
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc_;
	ZEROSTRUCT( depthStencilBufferDesc_ );

	depthStencilBufferDesc_.Width = g_windowWidth;
	depthStencilBufferDesc_.Height = g_windowHeight;
	depthStencilBufferDesc_.MipLevels = 1u;
	depthStencilBufferDesc_.ArraySize = 1u;
	depthStencilBufferDesc_.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	depthStencilBufferDesc_.SampleDesc.Count = 1u;
	depthStencilBufferDesc_.SampleDesc.Quality = 0u;
	depthStencilBufferDesc_.Usage = D3D11_USAGE_DEFAULT;
	depthStencilBufferDesc_.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc_.CPUAccessFlags = 0u;
	depthStencilBufferDesc_.MiscFlags = 0u;
	g_hResult = m_device->CreateTexture2D( &depthStencilBufferDesc_, nullptr, &m_defaultPipeline.m_depthStencilBuffer );
}
void GraphicsSystem::InitializeDepthStencilState( void )
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc_;
	ZEROSTRUCT( depthStencilDesc_ );

	depthStencilDesc_.DepthEnable = TRUE;
	depthStencilDesc_.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc_.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc_.StencilEnable = TRUE;
	depthStencilDesc_.StencilReadMask = 0xffui8;
	depthStencilDesc_.StencilWriteMask = 0xffui8;
	depthStencilDesc_.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc_.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc_.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc_.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc_.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc_.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc_.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc_.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	g_hResult = m_device->CreateDepthStencilState( &depthStencilDesc_, &m_defaultPipeline.m_depthStencilState );
}
void GraphicsSystem::InitializeDepthStencilView( void )
{
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc_;
	ZEROSTRUCT( depthStencilViewDesc_ );

	depthStencilViewDesc_.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	depthStencilViewDesc_.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc_.Texture2D.MipSlice = 0u;
	g_hResult = m_device->CreateDepthStencilView( m_defaultPipeline.m_depthStencilBuffer,
												  &depthStencilViewDesc_,
												  &m_defaultPipeline.m_depthStencilView );
}
void GraphicsSystem::InitializeRasterizerState( void )
{
	D3D11_RASTERIZER_DESC rasterizerDesc_;
	ZEROSTRUCT( rasterizerDesc_ );

	rasterizerDesc_.AntialiasedLineEnable = FALSE;
	rasterizerDesc_.CullMode = D3D11_CULL_NONE;
	rasterizerDesc_.DepthBias = 0;
	rasterizerDesc_.DepthBiasClamp = 0.0f;
	rasterizerDesc_.DepthClipEnable = TRUE;
	rasterizerDesc_.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc_.FrontCounterClockwise = TRUE;
	rasterizerDesc_.MultisampleEnable = FALSE;
	rasterizerDesc_.ScissorEnable = FALSE;
	rasterizerDesc_.SlopeScaledDepthBias = 0.0f;
	g_hResult = m_device->CreateRasterizerState( &rasterizerDesc_, &m_defaultPipeline.m_rasterizerState );
}
void GraphicsSystem::ToggleWireframe( void )
{
	m_defaultPipeline.m_rasterizerState->Release();
	m_deviceContext->RSGetState( &m_defaultPipeline.m_rasterizerState );
	D3D11_RASTERIZER_DESC rasterizerDesc_;
	m_defaultPipeline.m_rasterizerState->GetDesc( &rasterizerDesc_ );
	rasterizerDesc_.FillMode = D3D11_FILL_WIREFRAME == rasterizerDesc_.FillMode ? D3D11_FILL_SOLID : D3D11_FILL_WIREFRAME;
	g_hResult = m_device->CreateRasterizerState( &rasterizerDesc_, &m_defaultPipeline.m_rasterizerState );
	m_deviceContext->RSSetState( m_defaultPipeline.m_rasterizerState );
}
void GraphicsSystem::InitializeShadersAndInputLayout( void )
{
	ID3DBlob *vertexShaderBlob_, *pixelShaderBlob_;
#ifndef NDEBUG
	g_hResult = D3DCompileFromFile( L"shaders.shader", nullptr, nullptr, "VShader",
									"vs_5_0", D3DCOMPILE_DEBUG, 0u, &vertexShaderBlob_, nullptr );
	g_hResult = D3DCompileFromFile( L"shaders.shader", nullptr, nullptr, "PShader",
									"ps_5_0", D3DCOMPILE_DEBUG, 0u, &pixelShaderBlob_, nullptr );
#else
	g_hResult = D3DCompileFromFile( L"shaders.shader", nullptr, nullptr, "VShader",
									"vs_5_0", 0u, 0u, &vertexShaderBlob_, nullptr );
	g_hResult = D3DCompileFromFile( L"shaders.shader", nullptr, nullptr, "PShader",
									"ps_5_0", 0u, 0u, &pixelShaderBlob_, nullptr );
#endif
	g_hResult = m_device->CreateVertexShader( vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize(),
											  NULL, &m_defaultPipeline.m_vertexShader );
	g_hResult = m_device->CreatePixelShader( pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize(),
											 NULL, &m_defaultPipeline.m_pixelShader );

	static const D3D11_INPUT_ELEMENT_DESC inputElementDesc_[ 2u ] =
	{
		{ "POSITION", 0u, DXGI_FORMAT_R32G32B32A32_FLOAT, 0u,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u },
		{ "COLOR", 0u, DXGI_FORMAT_R32G32B32A32_FLOAT, 0u,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u }
	};
	g_hResult = m_device->CreateInputLayout( inputElementDesc_, 2u, vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize(), &m_defaultPipeline.m_inputLayout );
}
void GraphicsSystem::DrawGroundLines( void )
{
	static const unsigned int numVertices_ = 44u;
	static PositionColorVertex groundVertices_[ numVertices_ ];
	static bool groundInited = false;
	if ( !groundInited )
	{
		for ( unsigned int i = 0u; i < 11u; ++i )
		{
			groundVertices_[ 2u * i ] = PositionColorVertex( 7.5f, 0.0f, i * 1.5f - 7.5f, RGBAColor::Green );
			groundVertices_[ 2u * i + 1u ] = PositionColorVertex( -7.5f, 0.0f, i * 1.5f - 7.5f, RGBAColor::Green );
			groundVertices_[ 2u * i + 22u ] = PositionColorVertex( i * 1.5f - 7.5f, 0.0f, 7.5f, RGBAColor::Green );
			groundVertices_[ 2u * i + 23u ] = PositionColorVertex( i * 1.5f - 7.5f, 0.0f, -7.5f, RGBAColor::Green );
		}
		groundInited = true;
	}
	static const UINT stride_ = sizeof( PositionColorVertex );
	static const UINT offset_ = 0u;
	D3D11_BUFFER_DESC bufferDesc_;
	ZEROSTRUCT( bufferDesc_ );
	ID3D11Buffer* vertexBuffer_;
	D3D11_MAPPED_SUBRESOURCE mappedResource_;

	bufferDesc_.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_.ByteWidth = numVertices_ * sizeof( PositionColorVertex );
	bufferDesc_.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc_.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	g_hResult = m_device->CreateBuffer( &bufferDesc_, nullptr, &vertexBuffer_ );
	g_hResult = m_deviceContext->Map( vertexBuffer_, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource_ );
	memcpy( mappedResource_.pData, groundVertices_, numVertices_ * sizeof( PositionColorVertex ) );
	m_deviceContext->Unmap( vertexBuffer_, 0u );
	m_deviceContext->IASetVertexBuffers( 0, 1, &vertexBuffer_, &stride_, &offset_ );
	m_deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_LINELIST );

	m_deviceContext->Draw( numVertices_, 0u );

	vertexBuffer_->Release();
}
void GraphicsSystem::DrawFrame( void )
{
	m_deviceContext->ClearRenderTargetView( m_defaultPipeline.m_renderTargetView, RGBAColor::Black.m_channels );
	m_deviceContext->ClearDepthStencilView( m_defaultPipeline.m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0ui8 );
	SetMvpBuffer();
	if ( m_debugRendererEnabled )
		DrawDebugGraphics();
	m_DEBUG_LINES_numVertices = 0u;
	DrawGroundLines();
	DrawMeshes();

	g_hResult = m_swapChain->Present( 1u, 0u );
}
void GraphicsSystem::ReleaseAll( void )
{
	m_defaultVertexBuffer->Release();
	m_defaultPipeline.m_inputLayout->Release();
	m_defaultPipeline.m_vertexShader->Release();
	m_defaultPipeline.m_pixelShader->Release();
	m_defaultPipeline.m_rasterizerState->Release();
	m_defaultPipeline.m_depthStencilView->Release();
	m_defaultPipeline.m_depthStencilState->Release();
	m_defaultPipeline.m_depthStencilBuffer->Release();
	m_defaultPipeline.m_renderTargetView->Release();
	m_deviceContext->Release();
	m_device->Release();
	m_swapChain->Release();
	m_modelViewProjectionBuffer->Release();
}
void GraphicsSystem::EnableDebugGraphics( bool _enableDebugGraphics )
{
	m_debugRendererEnabled = _enableDebugGraphics;
}
Camera& GraphicsSystem::GetCamera( void )
{
	return m_mainCamera;
}