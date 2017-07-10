#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

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

struct ModelViewProjection
{
	DirectX::XMFLOAT4X4 m_model;
	DirectX::XMFLOAT4X4 m_view;
	DirectX::XMFLOAT4X4 m_projection;
};

struct Camera
{
	DirectX::XMFLOAT4X4 m_cameraTransform;
	Camera( void );
	void TranslateCamera( float, float, float );
	void RotateCamera( float, float );
	DirectX::XMMATRIX GetViewMatrix( void ) const;
	void Update( float );
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

struct Triangle
{
	union
	{
		struct
		{
			PositionColorVertex m_vertices[ 3u ];
		};
		struct
		{
			PositionColorVertex m_vertexA;
			PositionColorVertex m_vertexB;
			PositionColorVertex m_vertexC;
		};
	};
	Triangle( void );
	Triangle( const PositionColorVertex&,
			  const PositionColorVertex&,
			  const PositionColorVertex& );
	Triangle( const Triangle& );
	Triangle& operator=( const Triangle& );
};

class TriangleMesh
{
private:
	Triangle* m_triangles;
	unsigned int m_numTriangles, m_arrSize;
public:
	TriangleMesh( void );
	TriangleMesh( const TriangleMesh& );
	TriangleMesh( TriangleMesh&& );
	TriangleMesh& operator=( const TriangleMesh& );
	TriangleMesh& operator=( TriangleMesh&& );
	~TriangleMesh( void );
	void InitSize( unsigned int );
	void AddTriangle( const Triangle& );
	void Clear( void );
	const Triangle* GetTriangles( void ) const;
	unsigned int GetNumTriangles( void ) const;
};

class GraphicsSystem
{
private:
	PipelineState m_defaultPipeline;
	Camera m_mainCamera;
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11Buffer* m_defaultVertexBuffer = nullptr;
	D3D11_VIEWPORT m_viewport;
	UINT m_defaultVertexCount = 0u;
	ID3D11Buffer* m_modelViewProjectionBuffer = nullptr;
	ModelViewProjection m_modelViewProjection;
	std::vector<const TriangleMesh*> m_meshes;
	std::vector<DirectX::XMFLOAT4X4> m_worldMatrices;

	bool m_debugRendererEnabled;
	unsigned int m_DEBUG_LINES_numVertices;
	unsigned int m_DEBUG_LINES_arraySize;
	PositionColorVertex* m_DEBUG_LINES_vertexArray;

	void DrawGroundLines( void );
	void InitializeViewport( void );
	void SetPipelineStages( PipelineState* );
	void SetupDefaultBuffer( void );
	void InitializeDeviceContextChain( void );
	void InitializeDepthStencilBuffer( void );
	void InitializeDepthStencilState( void );
	void InitializeDepthStencilView( void );
	void InitializeRasterizerState( void );
	void InitializeShadersAndInputLayout( void );
	void InitializeMvpBuffer( void );
	void DrawDebugGraphics( void );
	void DrawMeshes( void );
	void DrawMesh( const TriangleMesh&, const DirectX::XMFLOAT4X4& );
	void SetMvpBuffer( void );

public:
	static const DirectX::XMFLOAT4X4 IDENTITYMATRIX;

	GraphicsSystem( void );
	~GraphicsSystem( void );
	void InitializeGraphicsSystem( void );
	void AddMesh( const TriangleMesh*, const DirectX::XMFLOAT4X4& = IDENTITYMATRIX );
	void ClearMeshes( void );
	void DrawFrame( void );
	void ReleaseAll( void );
	void EnableDebugGraphics( bool = true );
	void AddDebugLine( const PositionColorVertex&, const PositionColorVertex& );
	Camera& GetCamera( void );
	void ToggleWireframe( void );
};