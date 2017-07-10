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

std::vector<PositionTriangle> FBXDLL::FBX_GetBindPoseMesh( const char* const _file )
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

std::vector<JointTransform> FBXDLL::FBX_GetJoints( const char* const _file )
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
			temp_.m_transform._11 = ( float )( jointNodes_[ i ].m_node->EvaluateGlobalTransform().mData[ 0 ][ 0 ] );
			temp_.m_transform._12 = ( float )( jointNodes_[ i ].m_node->EvaluateGlobalTransform().mData[ 0 ][ 1 ] );
			temp_.m_transform._13 = ( float )( jointNodes_[ i ].m_node->EvaluateGlobalTransform().mData[ 0 ][ 2 ] );
			temp_.m_transform._14 = ( float )( jointNodes_[ i ].m_node->EvaluateGlobalTransform().mData[ 0 ][ 3 ] );
			temp_.m_transform._21 = ( float )( jointNodes_[ i ].m_node->EvaluateGlobalTransform().mData[ 1 ][ 0 ] );
			temp_.m_transform._22 = ( float )( jointNodes_[ i ].m_node->EvaluateGlobalTransform().mData[ 1 ][ 1 ] );
			temp_.m_transform._23 = ( float )( jointNodes_[ i ].m_node->EvaluateGlobalTransform().mData[ 1 ][ 2 ] );
			temp_.m_transform._24 = ( float )( jointNodes_[ i ].m_node->EvaluateGlobalTransform().mData[ 1 ][ 3 ] );
			temp_.m_transform._31 = ( float )( jointNodes_[ i ].m_node->EvaluateGlobalTransform().mData[ 2 ][ 0 ] );
			temp_.m_transform._32 = ( float )( jointNodes_[ i ].m_node->EvaluateGlobalTransform().mData[ 2 ][ 1 ] );
			temp_.m_transform._33 = ( float )( jointNodes_[ i ].m_node->EvaluateGlobalTransform().mData[ 2 ][ 2 ] );
			temp_.m_transform._34 = ( float )( jointNodes_[ i ].m_node->EvaluateGlobalTransform().mData[ 2 ][ 3 ] );
			temp_.m_transform._41 = ( float )( jointNodes_[ i ].m_node->EvaluateGlobalTransform().mData[ 3 ][ 0 ] );
			temp_.m_transform._42 = ( float )( jointNodes_[ i ].m_node->EvaluateGlobalTransform().mData[ 3 ][ 1 ] );
			temp_.m_transform._43 = ( float )( jointNodes_[ i ].m_node->EvaluateGlobalTransform().mData[ 3 ][ 2 ] );
			temp_.m_transform._44 = ( float )( jointNodes_[ i ].m_node->EvaluateGlobalTransform().mData[ 3 ][ 3 ] );
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