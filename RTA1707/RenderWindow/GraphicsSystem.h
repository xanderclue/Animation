#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

struct RGBAColor
{
	union
	{
		struct
		{
			DirectX::XMFLOAT4 m_rgba;
		};
		struct
		{
			float m_channels[ 4 ];
		};
		struct
		{
			float m_red, m_green, m_blue, m_alpha;
		};
	};
	RGBAColor( void );
	RGBAColor( float, float, float, float = 1.0f );
	static const RGBAColor Red, Green, Blue, Cyan, Magenta, Yellow, Black, White;
};

struct PositionColorVertex
{
	DirectX::XMFLOAT4 m_position;
	DirectX::XMFLOAT4 m_color;
	PositionColorVertex( void );
	PositionColorVertex( float, float, float, const RGBAColor& = RGBAColor::White );
};

struct PipelineState
{
	ID3D11InputLayout* m_inputLayout;
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterizerState;
};

class GraphicsSystem
{
private:
	PipelineState m_defaultPipeline;
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11Buffer* m_defaultVertexBuffer = nullptr;
	D3D11_VIEWPORT m_viewport;
	UINT m_vertexCount = 0u;

	bool m_debugRendererEnabled;
	unsigned int m_DEBUG_LINES_numVertices;
	unsigned int m_DEBUG_LINES_arraySize;
	PositionColorVertex* m_DEBUG_LINES_vertexArray;

public:
	GraphicsSystem( void );
	~GraphicsSystem( void );
	void InitializeViewport( void );
	void SetPipelineStages( PipelineState* );
	void SetupDefaultBuffer( void );
	void InitializeGraphicsSystem( void );
	void InitializeDeviceContextChain( void );
	void InitializeDepthStencilBuffer( void );
	void InitializeDepthStencilState( void );
	void InitializeDepthStencilView( void );
	void InitializeRasterizerState( void );
	void InitializeShadersAndInputLayout( void );
	void DrawFrame( void );
	void AddDebugLine( const PositionColorVertex&, const PositionColorVertex& );
	void DrawDebugGraphics( void );
	void ReleaseAll( void );
	void EnableDebugGraphics( bool = true );
};