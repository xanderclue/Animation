#pragma once
#include "fbxdll_structs.h"

namespace fbxsdk
{
	class FbxNode;
	class FbxPose;
	class FbxScene;
	class FbxSkeleton;
	class FbxMesh;
	class FbxAMatrix;
}

namespace FBXDLL
{
	struct JointNode
	{
		fbxsdk::FbxNode* m_node;
		int m_parent;
	};
}
const fbxsdk::FbxPose* GetBindPose( fbxsdk::FbxScene* );
const fbxsdk::FbxSkeleton* GetSkeletonRoot( const fbxsdk::FbxPose* );
std::vector<FBXDLL::PositionUvTriangle> MeshToTriangles( const fbxsdk::FbxMesh* );
std::vector<FBXDLL::JointNode> GetJointNodes( const fbxsdk::FbxSkeleton* );
void GetAnimClip( fbxsdk::FbxScene*, FBXDLL::AnimClip& );
void CopyMatrix( const fbxsdk::FbxAMatrix&, DirectX::XMFLOAT4X4& );
