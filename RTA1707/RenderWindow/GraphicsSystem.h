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
		struct
		{
			float r, g, b, a;
		};
	};
	RGBAColor( void );
	RGBAColor( float, float, float, float = 1.0f );
	static const RGBAColor Red, Green, Blue, Cyan, Magenta, Yellow, Black, White;
};

struct PositionColorVertex
{
	DirectX::XMFLOAT4 position;
	DirectX::XMFLOAT4 color;
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
	struct DebugRenderer
	{
		unsigned int m_numVertices;
		unsigned int m_arraySize;
		PositionColorVertex* m_vertexArray;
		DebugRenderer( void );
		~DebugRenderer( void );
	} m_debugRenderer;
	PipelineState m_defaultPipeline;
public:
	GraphicsSystem( void );
	~GraphicsSystem( void );
	void InitializeViewport( void );
	void SetPipelineStages( PipelineState* );
	void SetupDefaultBuffer( void );
	void InitializeGraphicsSystem( void );
	void CreateDeviceContextSwapChain( void );
	void InitializeDepthStencilBuffer( void );
	void InitializeDepthStencilState( void );
	void InitializeDepthStencilView( void );
	void InitializeRasterizerState( void );
	void InitializeShadersAndInputLayout( void );
	void DrawFrame( void );
	void AddDebugLine( void );
	void FlushDebugRenderer( void );
};