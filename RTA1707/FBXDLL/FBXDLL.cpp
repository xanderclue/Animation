#include "stdafx.h"
#include "FBXDLL.h"
#include <stdio.h>
#include "fbxdll_helper.h"
#include "PrintNode.h"
#include <fbxsdk.h>

std::vector<FBXDLL::PositionUvTriangle> FBXDLL::FBX_GetMeshBindPose( const char* const _file )
{
	std::vector<FBXDLL::PositionUvTriangle> triangles_;

	fbxsdk::FbxManager* const manager_ = fbxsdk::FbxManager::Create();
	fbxsdk::FbxIOSettings* const ioSettings_ = fbxsdk::FbxIOSettings::Create( manager_, IOSROOT );
	manager_->SetIOSettings( ioSettings_ );
	fbxsdk::FbxImporter* const importer_ = fbxsdk::FbxImporter::Create( manager_, "" );
	if ( !importer_->Initialize( _file, -1, manager_->GetIOSettings() ) )
	{
		printf( "Error: %s\n", importer_->GetStatus().GetErrorString() );
		importer_->Destroy();
		manager_->Destroy();
		return triangles_;
	}
	fbxsdk::FbxScene* const scene_ = fbxsdk::FbxScene::Create( manager_, "myScene" );
	importer_->Import( scene_ );
	importer_->Destroy();

	const fbxsdk::FbxPose* bindPose_ = GetBindPose( scene_ );
	const int nodeCount_ = bindPose_->GetCount();
	const fbxsdk::FbxMesh* mesh_;
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
std::vector<FBXDLL::JointTransform> FBXDLL::FBX_GetJointsBindPose( const char* const _file )
{
	std::vector<FBXDLL::JointTransform> joints_;

	fbxsdk::FbxManager* const manager_ = fbxsdk::FbxManager::Create();
	fbxsdk::FbxIOSettings* const ioSettings_ = fbxsdk::FbxIOSettings::Create( manager_, IOSROOT );
	manager_->SetIOSettings( ioSettings_ );
	fbxsdk::FbxImporter* const importer_ = fbxsdk::FbxImporter::Create( manager_, "" );
	if ( !importer_->Initialize( _file, -1, manager_->GetIOSettings() ) )
	{
		printf( "Error: %s\n", importer_->GetStatus().GetErrorString() );
		importer_->Destroy();
		manager_->Destroy();
		return joints_;
	}
	fbxsdk::FbxScene* const scene_ = fbxsdk::FbxScene::Create( manager_, "myScene" );
	importer_->Import( scene_ );
	importer_->Destroy();

	const fbxsdk::FbxSkeleton* skeletonRoot_ = GetSkeletonRoot( GetBindPose( scene_ ) );
	std::vector<FBXDLL::JointNode> jointNodes_ = GetJointNodes( skeletonRoot_ );
	{
		FBXDLL::JointTransform temp_;
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
	fbxsdk::FbxManager* const manager_ = fbxsdk::FbxManager::Create();
	fbxsdk::FbxIOSettings* const ioSettings_ = fbxsdk::FbxIOSettings::Create( manager_, IOSROOT );
	manager_->SetIOSettings( ioSettings_ );
	fbxsdk::FbxImporter* const importer_ = fbxsdk::FbxImporter::Create( manager_, "" );
	if ( !importer_->Initialize( _file, -1, manager_->GetIOSettings() ) )
	{
		printf( "Error: %s\n", importer_->GetStatus().GetErrorString() );
		importer_->Destroy();
		manager_->Destroy();
		return false;
	}
	fbxsdk::FbxScene* const scene_ = fbxsdk::FbxScene::Create( manager_, "myScene" );
	importer_->Import( scene_ );
	importer_->Destroy();
	const fbxsdk::FbxNode* const rootNode_ = scene_->GetRootNode();
	if ( rootNode_ )
		for ( int i = 0; i < rootNode_->GetChildCount(); ++i )
			PrintNode( rootNode_->GetChild( i ) );
	scene_->Destroy();
	manager_->Destroy();
	return true;
}
FBXDLL::AnimClip FBXDLL::FBX_GetAnimationData( const char* const _file )
{
	FBXDLL::AnimClip animClip_;

	fbxsdk::FbxManager* const manager_ = fbxsdk::FbxManager::Create();
	fbxsdk::FbxIOSettings* const ioSettings_ = fbxsdk::FbxIOSettings::Create( manager_, IOSROOT );
	manager_->SetIOSettings( ioSettings_ );
	fbxsdk::FbxImporter* const importer_ = fbxsdk::FbxImporter::Create( manager_, "" );
	if ( !importer_->Initialize( _file, -1, manager_->GetIOSettings() ) )
	{
		printf( "Error: %s\n", importer_->GetStatus().GetErrorString() );
		importer_->Destroy();
		manager_->Destroy();
		return animClip_;
	}
	fbxsdk::FbxScene* const scene_ = fbxsdk::FbxScene::Create( manager_, "myScene" );
	importer_->Import( scene_ );
	importer_->Destroy();

	GetAnimClip( scene_, animClip_ );

	scene_->Destroy();
	manager_->Destroy();

	return animClip_;
}
