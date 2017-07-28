#pragma once
#include "GraphicsSystem.h"
extern std::vector<DirectX::XMFLOAT4X4> g_nothing;
Renderer::TriangleMesh PositionTrianglesToMesh( const std::vector<FBXDLL::PositionUvTriangle>& );
void DrawSkeleton( const std::vector<FBXDLL::JointTransform>&, const DirectX::XMFLOAT4X4& = Renderer::GraphicsSystem::IDENTITYMATRIX );
void DrawSkeletonAnimation( const FBXDLL::AnimClip&, const DirectX::XMFLOAT4X4& = Renderer::GraphicsSystem::IDENTITYMATRIX, double = 0.0, bool = true, std::vector<DirectX::XMFLOAT4X4>& _joints = g_nothing );
void CheckKeyToggleWireframe( void );
void CheckKeyToggleTweening( bool& );
DirectX::XMFLOAT4X4& operator*=( DirectX::XMFLOAT4X4&, const DirectX::XMMATRIX& );
DirectX::XMFLOAT4X4 operator*( DirectX::XMFLOAT4X4, const DirectX::XMMATRIX& );
DirectX::XMFLOAT4X4 operator*( DirectX::XMFLOAT4X4, const DirectX::XMFLOAT4X4& );
DirectX::XMFLOAT4X4 GetMatrixInverse( const DirectX::XMFLOAT4X4& );