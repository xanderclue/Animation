#pragma once
#include "graphics_structs.h"

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
