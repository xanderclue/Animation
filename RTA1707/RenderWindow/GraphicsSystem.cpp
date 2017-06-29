#include "stdafx.h"
#include "GraphicsSystem.h"
#include <D3Dcompiler.h>
#include <stdio.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
extern HWND g_hWnd;
extern const int g_windowWidth;
extern const int g_windowHeight;

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
	r( _r ), g( _g ), b( _b ), a( _a )
{
}
PositionColorVertex::PositionColorVertex( void )
{
}
PositionColorVertex::PositionColorVertex( float _x, float _y, float _z, const RGBAColor& _color ) :
	position( _x, _y, _z, 1.0f ), color( _color.m_rgba )
{
}
void ExtendVertexArray( PositionColorVertex*& _arr, unsigned int& _size )
{
	PositionColorVertex* newArr_ = new PositionColorVertex[ _size << 1 ];
	for ( unsigned int i = 0u; i < _size; ++i )
		newArr_[ i ] = _arr[ i ];
	delete[ ] _arr;
	_arr = newArr_;
	_size <<= 1;
}
void GraphicsSystem::AddDebugLine( const PositionColorVertex& _a, const PositionColorVertex& _b )
{
	if ( m_DEBUG_LINES_arraySize == m_DEBUG_LINES_numVertices )
		ExtendVertexArray( m_DEBUG_LINES_vertexArray, m_DEBUG_LINES_arraySize );
	m_DEBUG_LINES_vertexArray[ m_DEBUG_LINES_numVertices ] = _a;
	++m_DEBUG_LINES_numVertices;

	if ( m_DEBUG_LINES_arraySize == m_DEBUG_LINES_numVertices )
		ExtendVertexArray( m_DEBUG_LINES_vertexArray, m_DEBUG_LINES_arraySize );
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

	m_DEBUG_LINES_numVertices = 0u;
	vertexBuffer_->Release();
}
GraphicsSystem::GraphicsSystem( void )
{
	ZEROSTRUCT( m_defaultPipeline );
	m_DEBUG_LINES_arraySize = 1024u;
	m_DEBUG_LINES_vertexArray = new PositionColorVertex[ 1024u ];
	m_DEBUG_LINES_numVertices = 0u;
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
	m_vertexCount = 3u;

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
void GraphicsSystem::InitializeGraphicsSystem( void )
{
	InitializeDeviceContextChain();
	InitializeDepthStencilBuffer();
	InitializeDepthStencilState();
	InitializeDepthStencilView();
	InitializeRasterizerState();
	InitializeViewport();
	InitializeShadersAndInputLayout();
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
	rasterizerDesc_.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc_.FrontCounterClockwise = TRUE;
	rasterizerDesc_.MultisampleEnable = FALSE;
	rasterizerDesc_.ScissorEnable = FALSE;
	rasterizerDesc_.SlopeScaledDepthBias = 0.0f;
	g_hResult = m_device->CreateRasterizerState( &rasterizerDesc_, &m_defaultPipeline.m_rasterizerState );
}
void GraphicsSystem::InitializeShadersAndInputLayout( void )
{
	ID3DBlob *VS, *PS;
#ifndef NDEBUG
	g_hResult = D3DCompileFromFile( L"shaders.shader", nullptr, nullptr, "VShader", "vs_5_0", D3DCOMPILE_DEBUG, 0u, &VS, nullptr );
	g_hResult = D3DCompileFromFile( L"shaders.shader", nullptr, nullptr, "PShader", "ps_5_0", D3DCOMPILE_DEBUG, 0u, &PS, nullptr );
#else
	g_hResult = D3DCompileFromFile( L"shaders.shader", nullptr, nullptr, "VShader", "vs_5_0", 0u, 0u, &VS, nullptr );
	g_hResult = D3DCompileFromFile( L"shaders.shader", nullptr, nullptr, "PShader", "ps_5_0", 0u, 0u, &PS, nullptr );
#endif
	g_hResult = m_device->CreateVertexShader( VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &m_defaultPipeline.m_vertexShader );
	g_hResult = m_device->CreatePixelShader( PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &m_defaultPipeline.m_pixelShader );

	static const D3D11_INPUT_ELEMENT_DESC desc[ 2u ] =
	{
		{ "POSITION", 0u, DXGI_FORMAT_R32G32B32A32_FLOAT, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u },
		{ "COLOR", 0u, DXGI_FORMAT_R32G32B32A32_FLOAT, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u }
	};
	g_hResult = m_device->CreateInputLayout( desc, 2u, VS->GetBufferPointer(), VS->GetBufferSize(), &m_defaultPipeline.m_inputLayout );
}
void GraphicsSystem::DrawFrame( void )
{
	static const UINT stride = sizeof( PositionColorVertex );
	static const UINT offset = 0u;
	ID3D11DeviceContext& deviceContext_ = *m_deviceContext;

	deviceContext_.ClearRenderTargetView( m_defaultPipeline.m_renderTargetView, RGBAColor::Black.m_channels );
	deviceContext_.ClearDepthStencilView( m_defaultPipeline.m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0ui8 );
	deviceContext_.IASetVertexBuffers( 0, 1, &m_defaultVertexBuffer, &stride, &offset );
	deviceContext_.IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	deviceContext_.Draw( m_vertexCount, 0u );
	DrawDebugGraphics();
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
}