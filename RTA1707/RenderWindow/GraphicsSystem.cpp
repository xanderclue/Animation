#include "stdafx.h"
#include "GraphicsSystem.h"
#include <D3Dcompiler.h>
#include <stdio.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
extern HWND g_hWnd;
extern const int g_windowWidth;
extern const int g_windowHeight;

const DirectX::XMFLOAT4X4 Renderer::GraphicsSystem::IDENTITYMATRIX = DirectX::XMFLOAT4X4( 1.0f, 0.0f, 0.0f, 0.0f,
																						  0.0f, 1.0f, 0.0f, 0.0f,
																						  0.0f, 0.0f, 1.0f, 0.0f,
																						  0.0f, 0.0f, 0.0f, 1.0f );

static struct HRESULT2
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

void Renderer::GraphicsSystem::AddMesh( const Renderer::TriangleMesh* _mesh, const DirectX::XMFLOAT4X4& _world )
{
	m_meshes.push_back( _mesh );
	m_worldMatrices.push_back( _world );
}
void Renderer::GraphicsSystem::ClearMeshes( void )
{
	m_meshes.clear();
	m_worldMatrices.clear();
}
void Renderer::GraphicsSystem::AddDebugLine( const Renderer::PositionColorVertex& _a, const Renderer::PositionColorVertex& _b )
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
void Renderer::GraphicsSystem::DrawDebugGraphics( void )
{
	if ( 0u == m_DEBUG_LINES_numVertices )
		return;

	static const UINT stride_ = sizeof( Renderer::PositionColorVertex );
	static const UINT offset_ = 0u;
	D3D11_BUFFER_DESC bufferDesc_;
	ZEROSTRUCT( bufferDesc_ );
	ID3D11Buffer* vertexBuffer_;
	D3D11_MAPPED_SUBRESOURCE mappedResource_;

	bufferDesc_.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_.ByteWidth = m_DEBUG_LINES_numVertices * sizeof( Renderer::PositionColorVertex );
	bufferDesc_.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc_.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	g_hResult = m_device->CreateBuffer( &bufferDesc_, nullptr, &vertexBuffer_ );
	g_hResult = m_deviceContext->Map( vertexBuffer_, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource_ );
	memcpy( mappedResource_.pData, m_DEBUG_LINES_vertexArray, m_DEBUG_LINES_numVertices * sizeof( Renderer::PositionColorVertex ) );
	m_deviceContext->Unmap( vertexBuffer_, 0u );
	m_deviceContext->IASetVertexBuffers( 0, 1, &vertexBuffer_, &stride_, &offset_ );
	m_deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_LINELIST );

	m_deviceContext->Draw( m_DEBUG_LINES_numVertices, 0u );

	vertexBuffer_->Release();
}
void Renderer::GraphicsSystem::DrawMeshes( void )
{
	const unsigned int size_ = ( unsigned int )m_meshes.size();
	if ( 0u != size_ )
		for ( unsigned int i = 0u; i < size_; ++i )
			DrawMesh( *( m_meshes[ i ] ), m_worldMatrices[ i ] );
	else
	{
		static const UINT stride_ = sizeof( Renderer::PositionColorVertex );
		static const UINT offset_ = 0u;
		ID3D11DeviceContext& deviceContext_ = *m_deviceContext;
		deviceContext_.IASetVertexBuffers( 0, 1, &m_defaultVertexBuffer, &stride_, &offset_ );
		deviceContext_.IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		//deviceContext_.Draw( m_defaultVertexCount, 0u );
	}
}
void Renderer::GraphicsSystem::SetMvpBuffer( void )
{
	m_modelViewProjection.m_model = IDENTITYMATRIX;
	DirectX::XMStoreFloat4x4( &m_modelViewProjection.m_view, m_mainCamera.GetViewMatrix() );

	m_deviceContext->UpdateSubresource( m_modelViewProjectionBuffer, 0u, nullptr, &m_modelViewProjection, 0u, 0u );
	m_deviceContext->VSSetConstantBuffers( 0u, 1u, &m_modelViewProjectionBuffer );
}
void Renderer::GraphicsSystem::DrawMesh( const Renderer::TriangleMesh& _mesh, const DirectX::XMFLOAT4X4& _world )
{
	const unsigned int numTriangles_ = _mesh.GetNumTriangles();
	const unsigned int numVertices_ = numTriangles_ * 3u;
	const Renderer::Triangle* const triangles_ = _mesh.GetTriangles();
	Renderer::PositionColorVertex* vertices_ = new Renderer::PositionColorVertex[ numVertices_ ];
	unsigned int i, j;
	for ( i = 0u; i < numTriangles_; ++i )
		for ( j = 0u; j < 3u; ++j )
			vertices_[ i * 3u + j ] = triangles_[ i ].m_vertices[ j ];
	ID3D11Buffer* vertexBuffer_;
	D3D11_BUFFER_DESC bufferDesc_;
	ZEROSTRUCT( bufferDesc_ );
	D3D11_MAPPED_SUBRESOURCE mappedResource_;
	bufferDesc_.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_.ByteWidth = sizeof( Renderer::PositionColorVertex ) * numVertices_;
	bufferDesc_.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc_.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	g_hResult = m_device->CreateBuffer( &bufferDesc_, nullptr, &vertexBuffer_ );
	g_hResult = m_deviceContext->Map( vertexBuffer_, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource_ );
	memcpy( mappedResource_.pData, vertices_, sizeof( Renderer::PositionColorVertex ) * numVertices_ );
	m_deviceContext->Unmap( vertexBuffer_, 0u );
	static const UINT stride_ = sizeof( Renderer::PositionColorVertex );
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
Renderer::GraphicsSystem::GraphicsSystem( void ) :
	m_debugRendererEnabled( true ),
	m_DEBUG_LINES_arraySize( 1024u ),
	m_DEBUG_LINES_numVertices( 0u )
{
	ZEROSTRUCT( m_defaultPipeline );
	m_mainCamera.TranslateCamera( 0.0f, 5.0f, -7.0f );
	m_mainCamera.RotateCamera( 28.0f, 0.0f );
	m_DEBUG_LINES_vertexArray = new Renderer::PositionColorVertex[ m_DEBUG_LINES_arraySize ];
}
Renderer::GraphicsSystem::~GraphicsSystem( void )
{
	delete[ ] m_DEBUG_LINES_vertexArray;
}
void Renderer::GraphicsSystem::InitializeViewport( void )
{
	ZEROSTRUCT( m_viewport );
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	m_viewport.Width = ( FLOAT )g_windowWidth;
	m_viewport.Height = ( FLOAT )g_windowHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
}
void Renderer::GraphicsSystem::SetPipelineStages( Renderer::PipelineState* _pipelineState )
{
	m_deviceContext->OMSetDepthStencilState( _pipelineState->m_depthStencilState, 1u );
	m_deviceContext->OMSetRenderTargets( 1u, &_pipelineState->m_renderTargetView, _pipelineState->m_depthStencilView );
	m_deviceContext->RSSetState( _pipelineState->m_rasterizerState );
	m_deviceContext->VSSetShader( _pipelineState->m_vertexShader, nullptr, 0u );
	m_deviceContext->PSSetShader( _pipelineState->m_pixelShader, nullptr, 0u );
	m_deviceContext->IASetInputLayout( _pipelineState->m_inputLayout );
}
void Renderer::GraphicsSystem::SetupDefaultBuffer( void )
{
	static const Renderer::PositionColorVertex triangle_[ ] =
	{
		Renderer::PositionColorVertex( 0.0f, 1.0f, 0.5f, Renderer::RGBAColor::Red ),
		Renderer::PositionColorVertex( -1.0f, -1.0f, 0.5f, Renderer::RGBAColor::Green ),
		Renderer::PositionColorVertex( 1.0f, -1.0f, 0.5f, Renderer::RGBAColor::Blue )
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
void Renderer::GraphicsSystem::InitializeMvpBuffer( void )
{
	static const CD3D11_BUFFER_DESC mvpBufferDesc_( sizeof( Renderer::ModelViewProjection ), D3D11_BIND_CONSTANT_BUFFER );
	g_hResult = m_device->CreateBuffer( &mvpBufferDesc_, nullptr, &m_modelViewProjectionBuffer );
	static const float aspect_ = ( float )g_windowWidth / ( float )g_windowHeight;
	static const float fov_ = DirectX::XMConvertToRadians( 70.0f );
	static const float near_ = 0.01f;
	static const float far_ = 100.0f;
	XMStoreFloat4x4( &m_modelViewProjection.m_projection, XMMatrixTranspose( DirectX::XMMatrixPerspectiveFovLH( fov_, aspect_, near_, far_ ) ) );
}
void Renderer::GraphicsSystem::InitializeGraphicsSystem( void )
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
void Renderer::GraphicsSystem::InitializeDeviceContextChain( void )
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
void Renderer::GraphicsSystem::InitializeDepthStencilBuffer( void )
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
void Renderer::GraphicsSystem::InitializeDepthStencilState( void )
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
void Renderer::GraphicsSystem::InitializeDepthStencilView( void )
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
void Renderer::GraphicsSystem::InitializeRasterizerState( void )
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
void Renderer::GraphicsSystem::ToggleWireframe( void )
{
	m_defaultPipeline.m_rasterizerState->Release();
	m_deviceContext->RSGetState( &m_defaultPipeline.m_rasterizerState );
	D3D11_RASTERIZER_DESC rasterizerDesc_;
	m_defaultPipeline.m_rasterizerState->GetDesc( &rasterizerDesc_ );
	rasterizerDesc_.FillMode = D3D11_FILL_WIREFRAME == rasterizerDesc_.FillMode ? D3D11_FILL_SOLID : D3D11_FILL_WIREFRAME;
	g_hResult = m_device->CreateRasterizerState( &rasterizerDesc_, &m_defaultPipeline.m_rasterizerState );
	m_deviceContext->RSSetState( m_defaultPipeline.m_rasterizerState );
}
void Renderer::GraphicsSystem::InitializeShadersAndInputLayout( void )
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
void Renderer::GraphicsSystem::DrawGroundLines( void )
{
	static const unsigned int numVertices_ = 44u;
	static Renderer::PositionColorVertex groundVertices_[ numVertices_ ];
	static bool groundInited = false;
	if ( !groundInited )
	{
		for ( unsigned int i = 0u; i < 11u; ++i )
		{
			groundVertices_[ 2u * i ] = Renderer::PositionColorVertex( 7.5f, 0.0f, i * 1.5f - 7.5f, Renderer::RGBAColor::Green );
			groundVertices_[ 2u * i + 1u ] = Renderer::PositionColorVertex( -7.5f, 0.0f, i * 1.5f - 7.5f, Renderer::RGBAColor::Green );
			groundVertices_[ 2u * i + 22u ] = Renderer::PositionColorVertex( i * 1.5f - 7.5f, 0.0f, 7.5f, Renderer::RGBAColor::Green );
			groundVertices_[ 2u * i + 23u ] = Renderer::PositionColorVertex( i * 1.5f - 7.5f, 0.0f, -7.5f, Renderer::RGBAColor::Green );
		}
		groundInited = true;
	}
	static const UINT stride_ = sizeof( Renderer::PositionColorVertex );
	static const UINT offset_ = 0u;
	D3D11_BUFFER_DESC bufferDesc_;
	ZEROSTRUCT( bufferDesc_ );
	ID3D11Buffer* vertexBuffer_;
	D3D11_MAPPED_SUBRESOURCE mappedResource_;

	bufferDesc_.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_.ByteWidth = numVertices_ * sizeof( Renderer::PositionColorVertex );
	bufferDesc_.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc_.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	g_hResult = m_device->CreateBuffer( &bufferDesc_, nullptr, &vertexBuffer_ );
	g_hResult = m_deviceContext->Map( vertexBuffer_, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource_ );
	memcpy( mappedResource_.pData, groundVertices_, numVertices_ * sizeof( Renderer::PositionColorVertex ) );
	m_deviceContext->Unmap( vertexBuffer_, 0u );
	m_deviceContext->IASetVertexBuffers( 0, 1, &vertexBuffer_, &stride_, &offset_ );
	m_deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_LINELIST );

	m_deviceContext->Draw( numVertices_, 0u );

	vertexBuffer_->Release();
}
void Renderer::GraphicsSystem::DrawFrame( void )
{
	m_deviceContext->ClearRenderTargetView( m_defaultPipeline.m_renderTargetView, Renderer::RGBAColor::Black.m_channels );
	m_deviceContext->ClearDepthStencilView( m_defaultPipeline.m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0ui8 );
	SetMvpBuffer();
	if ( m_debugRendererEnabled )
		DrawDebugGraphics();
	m_DEBUG_LINES_numVertices = 0u;
	DrawGroundLines();
	DrawMeshes();

	g_hResult = m_swapChain->Present( 1u, 0u );
}
void Renderer::GraphicsSystem::ReleaseAll( void )
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
void Renderer::GraphicsSystem::EnableDebugGraphics( bool _enableDebugGraphics )
{
	m_debugRendererEnabled = _enableDebugGraphics;
}
Renderer::Camera& Renderer::GraphicsSystem::GetCamera( void )
{
	return m_mainCamera;
}
