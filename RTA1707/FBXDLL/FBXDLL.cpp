#include "stdafx.h"
#include "FBXDLL.h"
#include <stdio.h>
#include "fbxdll_helper.h"

std::vector<PositionUvTriangle> FBXDLL::FBX_GetMeshBindPose( const char* const _file )
{
	std::vector<PositionUvTriangle> triangles_;

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
