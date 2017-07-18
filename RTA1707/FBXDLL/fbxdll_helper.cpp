#include "stdafx.h"
#include "fbxdll_helper.h"
#include <fbxsdk.h>

void CopyMatrix( const fbxsdk::FbxAMatrix& _inMat, DirectX::XMFLOAT4X4& _outMat )
{
	_outMat._11 = ( float )( _inMat.mData[ 0 ][ 0 ] );
	_outMat._12 = ( float )( _inMat.mData[ 0 ][ 1 ] );
	_outMat._13 = ( float )( _inMat.mData[ 0 ][ 2 ] );
	_outMat._14 = ( float )( _inMat.mData[ 0 ][ 3 ] );
	_outMat._21 = ( float )( _inMat.mData[ 1 ][ 0 ] );
	_outMat._22 = ( float )( _inMat.mData[ 1 ][ 1 ] );
	_outMat._23 = ( float )( _inMat.mData[ 1 ][ 2 ] );
	_outMat._24 = ( float )( _inMat.mData[ 1 ][ 3 ] );
	_outMat._31 = ( float )( _inMat.mData[ 2 ][ 0 ] );
	_outMat._32 = ( float )( _inMat.mData[ 2 ][ 1 ] );
	_outMat._33 = ( float )( _inMat.mData[ 2 ][ 2 ] );
	_outMat._34 = ( float )( _inMat.mData[ 2 ][ 3 ] );
	_outMat._41 = ( float )( _inMat.mData[ 3 ][ 0 ] );
	_outMat._42 = ( float )( _inMat.mData[ 3 ][ 1 ] );
	_outMat._43 = ( float )( _inMat.mData[ 3 ][ 2 ] );
	_outMat._44 = ( float )( _inMat.mData[ 3 ][ 3 ] );
}
void GetAnimClip( fbxsdk::FbxScene* _scene, FBXDLL::AnimClip& _animClip )
{
	fbxsdk::FbxAnimStack* animStack_ = _scene->GetCurrentAnimationStack();
	fbxsdk::FbxTimeSpan timeSpan_ = animStack_->GetLocalTimeSpan();
	fbxsdk::FbxTime duration_ = timeSpan_.GetDuration();
	_animClip.m_duration = duration_.GetSecondDouble();
	fbxsdk::FbxLongLong frameCount_ = duration_.GetFrameCount( fbxsdk::FbxTime::EMode::eFrames24 );
	std::vector<FBXDLL::JointNode> jointNodes_ = GetJointNodes( GetSkeletonRoot( GetBindPose( _scene ) ) );
	fbxsdk::FbxTime time_;
	fbxsdk::FbxLongLong i;
	size_t j;
	for ( i = 1i64; i < frameCount_; ++i )
	{
		FBXDLL::Keyframe keyFrame;
		time_.SetFrame( i, fbxsdk::FbxTime::EMode::eFrames24 );
		keyFrame.m_time = time_.GetSecondDouble();
		for ( j = 0u; j < jointNodes_.size(); ++j )
		{
			FBXDLL::JointTransform jointTransform_;
			jointTransform_.m_parent = jointNodes_[ j ].m_parent;
			CopyMatrix(
				jointNodes_[ j ].m_node->EvaluateGlobalTransform( time_ ),
				jointTransform_.m_transform );
			keyFrame.m_joints.push_back( jointTransform_ );
		}
		_animClip.m_frames.push_back( keyFrame );
	}
}
const fbxsdk::FbxPose* GetBindPose( fbxsdk::FbxScene* _scene )
{
	const int poseCount_ = _scene->GetPoseCount();
	const fbxsdk::FbxPose* pose_;
	for ( int i = 0; i < poseCount_; ++i )
		if ( ( pose_ = _scene->GetPose( i ) )->IsBindPose() )
			return pose_;
	return nullptr;
}
const fbxsdk::FbxSkeleton* GetSkeletonRoot( const fbxsdk::FbxPose* _bindPose )
{
	const int nodeCount_ = _bindPose->GetCount();
	const fbxsdk::FbxSkeleton* skeleton_;
	for ( int i = 0; i < nodeCount_; ++i )
		if ( nullptr != ( skeleton_ = _bindPose->GetNode( i )->GetSkeleton() ) )
			if ( skeleton_->IsSkeletonRoot() )
				return skeleton_;
	return nullptr;
}
std::vector<FBXDLL::PositionUvTriangle> MeshToTriangles( const fbxsdk::FbxMesh* _mesh )
{
	std::vector<FBXDLL::PositionUvTriangle> triangles_;

	if ( !_mesh->IsTriangleMesh() )
	{
		// TODO : Triangulate
		printf( "ERROR: Not triangulated\n" );
		return triangles_;
	}

	const int polygonCount_ = _mesh->GetPolygonCount();
	const int* const polygonVertices_ = _mesh->GetPolygonVertices();
	const fbxsdk::FbxVector4* const controlPoints_ = _mesh->GetControlPoints();
	FBXDLL::PositionUvTriangle tempTriangle_;
	fbxsdk::FbxVector4 tempVector_;
	fbxsdk::FbxVector2 tempUv_;
	bool unmapped_;
	fbxsdk::FbxStringList uvSetNames_;
	_mesh->GetUVSetNames( uvSetNames_ );
	const char* uvSetName_ = uvSetNames_[ 0 ];
	for ( int i = 0; i < polygonCount_; ++i )
	{
		tempVector_ = controlPoints_[ polygonVertices_[ i * 3 ] ];
		_mesh->GetPolygonVertexUV( i, 0, uvSetName_, tempUv_, unmapped_ );
		tempTriangle_.m_posA.x = ( float )tempVector_.mData[ 0 ];
		tempTriangle_.m_posA.y = ( float )tempVector_.mData[ 1 ];
		tempTriangle_.m_posA.z = ( float )tempVector_.mData[ 2 ];
		tempTriangle_.m_uvA.x = ( float )tempUv_.mData[ 0 ];
		tempTriangle_.m_uvA.y = ( float )tempUv_.mData[ 1 ];

		tempVector_ = controlPoints_[ polygonVertices_[ i * 3 + 1 ] ];
		_mesh->GetPolygonVertexUV( i, 1, uvSetName_, tempUv_, unmapped_ );
		tempTriangle_.m_posB.x = ( float )tempVector_.mData[ 0 ];
		tempTriangle_.m_posB.y = ( float )tempVector_.mData[ 1 ];
		tempTriangle_.m_posB.z = ( float )tempVector_.mData[ 2 ];
		tempTriangle_.m_uvB.x = ( float )tempUv_.mData[ 0 ];
		tempTriangle_.m_uvB.y = ( float )tempUv_.mData[ 1 ];

		tempVector_ = controlPoints_[ polygonVertices_[ i * 3 + 2 ] ];
		_mesh->GetPolygonVertexUV( i, 2, uvSetName_, tempUv_, unmapped_ );
		tempTriangle_.m_posC.x = ( float )tempVector_.mData[ 0 ];
		tempTriangle_.m_posC.y = ( float )tempVector_.mData[ 1 ];
		tempTriangle_.m_posC.z = ( float )tempVector_.mData[ 2 ];
		tempTriangle_.m_uvC.x = ( float )tempUv_.mData[ 0 ];
		tempTriangle_.m_uvC.y = ( float )tempUv_.mData[ 1 ];

		triangles_.push_back( tempTriangle_ );
	}

	return triangles_;
}
void AddNodes( const fbxsdk::FbxSkeleton* _skeleton, std::vector<FBXDLL::JointNode>& _outNodes, int _parent )
{
	if ( nullptr == _skeleton )
		return;
	FBXDLL::JointNode node_;
	node_.m_parent = _parent;
	node_.m_node = _skeleton->GetNode();
	int idx_ = ( int )_outNodes.size();
	_outNodes.push_back( node_ );
	int childCount_ = node_.m_node->GetChildCount();
	for ( int i = 0; i < childCount_; ++i )
		AddNodes( node_.m_node->GetChild( i )->GetSkeleton(), _outNodes, idx_ );
}
std::vector<FBXDLL::JointNode> GetJointNodes( const fbxsdk::FbxSkeleton* _skeletonRoot )
{
	std::vector<FBXDLL::JointNode> joints_;
	AddNodes( _skeletonRoot, joints_, -1 );
	return joints_;
}
