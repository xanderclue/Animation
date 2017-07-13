#pragma once
#include <fbxsdk.h>
#include "fbxdll_structs.h"
#include "PrintNode.h"

struct JointNode
{
	FbxNode* m_node;
	int m_parent;
};

const FbxPose* GetBindPose( FbxScene* );
const FbxSkeleton* GetSkeletonRoot( const FbxPose* );
std::vector<PositionUvTriangle> MeshToTriangles( const FbxMesh* );
std::vector<JointNode> GetJointNodes( const FbxSkeleton* );
void GetAnimClip( FbxScene*, AnimClip& );
void CopyMatrix( const FbxAMatrix&, DirectX::XMFLOAT4X4& );
