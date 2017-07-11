#include "stdafx.h"
#include "FBXDLL.h"
#include <stdio.h>
#include <fbxsdk.h>

struct JointNode
{
	FbxNode* m_node;
	int m_parent;
};

const FbxPose* GetBindPose( FbxScene* );
const FbxSkeleton* GetSkeletonRoot( const FbxPose* );
std::vector<PositionTriangle> MeshToTriangles( const FbxMesh* );
std::vector<JointNode> GetJointNodes( const FbxSkeleton* );

std::vector<PositionTriangle> FBXDLL::FBX_GetMeshBindPose( const char* const _file )
{
	std::vector<PositionTriangle> triangles_;

	FbxManager* const manager_ = FbxManager::Create();
	FbxIOSettings* const ioSettings_ = FbxIOSettings::Create( manager_, IOSROOT );
	manager_->SetIOSettings( ioSettings_ );
	FbxImporter* const importer_ = FbxImporter::Create( manager_, "" );
	if ( !importer_->Initialize( _file, -1, manager_->GetIOSettings() ) )
	{
		printf( "Error: %s\n", importer_->GetStatus().GetErrorString() );
		importer_->Destroy();
		manager_->Destroy();
		return triangles_;
	}
	FbxScene* const scene_ = FbxScene::Create( manager_, "myScene" );
	importer_->Import( scene_ );
	importer_->Destroy();

	const FbxPose* bindPose_ = GetBindPose( scene_ );
	const int nodeCount_ = bindPose_->GetCount();
	const FbxMesh* mesh_;
	for ( int i = 0; i < nodeCount_; ++i )
	{
		mesh_ = bindPose_->GetNode( i )->GetMesh();
		if ( nullptr != mesh_ )
		{
			triangles_ = MeshToTriangles( mesh_ );
			break;
		}
	}

	scene_->Destroy();
	manager_->Destroy();

	return triangles_;
}
void CopyMatrix( const FbxAMatrix& _inMat, DirectX::XMFLOAT4X4& _outMat )
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
std::vector<JointTransform> FBXDLL::FBX_GetJointsBindPose( const char* const _file )
{
	std::vector<JointTransform> joints_;

	FbxManager* const manager_ = FbxManager::Create();
	FbxIOSettings* const ioSettings_ = FbxIOSettings::Create( manager_, IOSROOT );
	manager_->SetIOSettings( ioSettings_ );
	FbxImporter* const importer_ = FbxImporter::Create( manager_, "" );
	if ( !importer_->Initialize( _file, -1, manager_->GetIOSettings() ) )
	{
		printf( "Error: %s\n", importer_->GetStatus().GetErrorString() );
		importer_->Destroy();
		manager_->Destroy();
		return joints_;
	}
	FbxScene* const scene_ = FbxScene::Create( manager_, "myScene" );
	importer_->Import( scene_ );
	importer_->Destroy();

	const FbxSkeleton* skeletonRoot_ = GetSkeletonRoot( GetBindPose( scene_ ) );
	std::vector<JointNode> jointNodes_ = GetJointNodes( skeletonRoot_ );
	{
		JointTransform temp_;
		const unsigned int size_ = ( unsigned int )jointNodes_.size();
		for ( unsigned int i = 0u; i < size_; ++i )
		{
			CopyMatrix( jointNodes_[ i ].m_node->EvaluateGlobalTransform(), temp_.m_transform );
			temp_.m_parent = jointNodes_[ i ].m_parent;
			joints_.push_back( temp_ );
		}
	}

	scene_->Destroy();
	manager_->Destroy();

	return joints_;
}

#include "PrintNode.h"
bool FBXDLL::TestFBX_PrintInfo( const char* const _file )
{
	// Source: "Autodesk FBX SDK Documentation: Your First FBX SDK Program"
	FbxManager* const manager_ = FbxManager::Create();
	FbxIOSettings* const ioSettings_ = FbxIOSettings::Create( manager_, IOSROOT );
	manager_->SetIOSettings( ioSettings_ );
	FbxImporter* const importer_ = FbxImporter::Create( manager_, "" );
	if ( !importer_->Initialize( _file, -1, manager_->GetIOSettings() ) )
	{
		printf( "Error: %s\n", importer_->GetStatus().GetErrorString() );
		importer_->Destroy();
		manager_->Destroy();
		return false;
	}
	FbxScene* const scene_ = FbxScene::Create( manager_, "myScene" );
	importer_->Import( scene_ );
	importer_->Destroy();
	const FbxNode* const rootNode_ = scene_->GetRootNode();
	if ( rootNode_ )
		for ( int i = 0; i < rootNode_->GetChildCount(); ++i )
			PrintNode( rootNode_->GetChild( i ) );
	scene_->Destroy();
	manager_->Destroy();
	return true;
}

void GetAnimClip( FbxScene* _scene, AnimClip& _animClip )
{
	FbxAnimStack* animStack_ = _scene->GetCurrentAnimationStack();
	FbxTimeSpan timeSpan_ = animStack_->GetLocalTimeSpan();
	FbxTime duration_ = timeSpan_.GetDuration();
	_animClip.m_duration = duration_.GetSecondDouble();
	FbxLongLong frameCount_ = duration_.GetFrameCount( FbxTime::EMode::eFrames24 );
	std::vector<JointNode> jointNodes_ = GetJointNodes( GetSkeletonRoot( GetBindPose( _scene ) ) );
	FbxTime time_;
	FbxLongLong i;
	size_t j;
	for ( i = 1i64; i < frameCount_; ++i )
	{
		Keyframe keyFrame;
		time_.SetFrame( i, FbxTime::EMode::eFrames24 );
		keyFrame.m_time = time_.GetSecondDouble();
		for ( j = 0u; j < jointNodes_.size(); ++j )
		{
			JointTransform jointTransform_;
			jointTransform_.m_parent = jointNodes_[ j ].m_parent;
			CopyMatrix(
				jointNodes_[ j ].m_node->EvaluateGlobalTransform( time_ ),
				jointTransform_.m_transform );
			keyFrame.m_joints.push_back( jointTransform_ );
		}
		_animClip.m_frames.push_back( keyFrame );
	}
}

AnimClip FBXDLL::FBX_GetAnimationData( const char* const _file )
{
	AnimClip animClip_;

	FbxManager* const manager_ = FbxManager::Create();
	FbxIOSettings* const ioSettings_ = FbxIOSettings::Create( manager_, IOSROOT );
	manager_->SetIOSettings( ioSettings_ );
	FbxImporter* const importer_ = FbxImporter::Create( manager_, "" );
	if ( !importer_->Initialize( _file, -1, manager_->GetIOSettings() ) )
	{
		printf( "Error: %s\n", importer_->GetStatus().GetErrorString() );
		importer_->Destroy();
		manager_->Destroy();
		return animClip_;
	}
	FbxScene* const scene_ = FbxScene::Create( manager_, "myScene" );
	importer_->Import( scene_ );
	importer_->Destroy();

	GetAnimClip( scene_, animClip_ );

	scene_->Destroy();
	manager_->Destroy();

	return animClip_;
}

const FbxPose* GetBindPose( FbxScene* _scene )
{
	const int poseCount_ = _scene->GetPoseCount();
	const FbxPose* pose_;
	for ( int i = 0; i < poseCount_; ++i )
		if ( ( pose_ = _scene->GetPose( i ) )->IsBindPose() )
			return pose_;
	return nullptr;
}

const FbxSkeleton* GetSkeletonRoot( const FbxPose* _bindPose )
{
	const int nodeCount_ = _bindPose->GetCount();
	const FbxSkeleton* skeleton_;
	for ( int i = 0; i < nodeCount_; ++i )
		if ( nullptr != ( skeleton_ = _bindPose->GetNode( i )->GetSkeleton() ) )
			if ( skeleton_->IsSkeletonRoot() )
				return skeleton_;
	return nullptr;
}

std::vector<PositionTriangle> MeshToTriangles( const FbxMesh* _mesh )
{
	std::vector<PositionTriangle> triangles_;

	if ( !_mesh->IsTriangleMesh() )
	{
		// TODO : Triangulate
		printf( "ERROR: Not triangulated\n" );
		return triangles_;
	}

	const int polygonCount_ = _mesh->GetPolygonCount();
	const int* const polygonVertices_ = _mesh->GetPolygonVertices();
	const FbxVector4* const controlPoints_ = _mesh->GetControlPoints();
	PositionTriangle tempTriangle_;
	FbxVector4 tempVector_;
	for ( int i = 0; i < polygonCount_; ++i )
	{
		tempVector_ = controlPoints_[ polygonVertices_[ i * 3 ] ];
		tempTriangle_.m_posA.x = ( float )tempVector_.mData[ 0 ];
		tempTriangle_.m_posA.y = ( float )tempVector_.mData[ 1 ];
		tempTriangle_.m_posA.z = ( float )tempVector_.mData[ 2 ];

		tempVector_ = controlPoints_[ polygonVertices_[ i * 3 + 1 ] ];
		tempTriangle_.m_posB.x = ( float )tempVector_.mData[ 0 ];
		tempTriangle_.m_posB.y = ( float )tempVector_.mData[ 1 ];
		tempTriangle_.m_posB.z = ( float )tempVector_.mData[ 2 ];

		tempVector_ = controlPoints_[ polygonVertices_[ i * 3 + 2 ] ];
		tempTriangle_.m_posC.x = ( float )tempVector_.mData[ 0 ];
		tempTriangle_.m_posC.y = ( float )tempVector_.mData[ 1 ];
		tempTriangle_.m_posC.z = ( float )tempVector_.mData[ 2 ];

		triangles_.push_back( tempTriangle_ );
	}

	return triangles_;
}

void AddNodes( const FbxSkeleton* _skeleton, std::vector<JointNode>& _outNodes, int _parent )
{
	if ( nullptr == _skeleton )
		return;
	JointNode node_;
	node_.m_parent = _parent;
	node_.m_node = _skeleton->GetNode();
	int idx_ = ( int )_outNodes.size();
	_outNodes.push_back( node_ );
	int childCount_ = node_.m_node->GetChildCount();
	for ( int i = 0; i < childCount_; ++i )
		AddNodes( node_.m_node->GetChild( i )->GetSkeleton(), _outNodes, idx_ );
}

std::vector<JointNode> GetJointNodes( const FbxSkeleton* _skeletonRoot )
{
	std::vector<JointNode> joints_;
	AddNodes( _skeletonRoot, joints_, -1 );
	return joints_;
}