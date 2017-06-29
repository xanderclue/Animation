#include "stdafx.h"
#include "FBXDLL.h"
#include <stdio.h>
#include <fbxsdk.h>

const FbxPose* GetBindPose( FbxScene* );
const FbxSkeleton* GetSkeletonRoot( const FbxPose* );
std::vector<PositionTriangle> MeshToTriangles( const FbxMesh* );

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

int FBXDLL::TestFunc_Square( int _x )
{
	return _x * _x;
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
		tempTriangle_.a.x = ( float )tempVector_.mData[ 0 ];
		tempTriangle_.a.y = ( float )tempVector_.mData[ 1 ];
		tempTriangle_.a.z = ( float )tempVector_.mData[ 2 ];

		tempVector_ = controlPoints_[ polygonVertices_[ i * 3 + 1 ] ];
		tempTriangle_.b.x = ( float )tempVector_.mData[ 0 ];
		tempTriangle_.b.y = ( float )tempVector_.mData[ 1 ];
		tempTriangle_.b.z = ( float )tempVector_.mData[ 2 ];

		tempVector_ = controlPoints_[ polygonVertices_[ i * 3 + 2 ] ];
		tempTriangle_.c.x = ( float )tempVector_.mData[ 0 ];
		tempTriangle_.c.y = ( float )tempVector_.mData[ 1 ];
		tempTriangle_.c.z = ( float )tempVector_.mData[ 2 ];

		triangles_.push_back( tempTriangle_ );
	}

	return triangles_;
}