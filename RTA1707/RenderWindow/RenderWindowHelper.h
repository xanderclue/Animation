#pragma once
#include "GraphicsSystem.h"

TriangleMesh PositionTrianglesToMesh( const std::vector<PositionUvTriangle>& );
void DrawSkeleton( const std::vector<JointTransform>&, const DirectX::XMFLOAT4X4& = GraphicsSystem::IDENTITYMATRIX );
void DrawSkeletonAnimation( const AnimClip&, const DirectX::XMFLOAT4X4& = GraphicsSystem::IDENTITYMATRIX, double = 0.0, bool = true );
void CheckKeyToggleWireframe( void );
void CheckKeyToggleTweening( bool& );
DirectX::XMFLOAT4X4& operator*=( DirectX::XMFLOAT4X4&, const DirectX::XMMATRIX& );
DirectX::XMFLOAT4X4 operator*( DirectX::XMFLOAT4X4, const DirectX::XMMATRIX& );
