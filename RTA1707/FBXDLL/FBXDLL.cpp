#include "stdafx.h"
#include "FBXDLL.h"
#include <stdio.h>
#include "fbxdll_helper.h"
#include "PrintNode.h"
#include <fbxsdk.h>

std::vector<FBXDLL::PositionUvTriangle> FBXDLL::FBX_GetMesh( const char* const _file, std::vector<JointTransform>& _joints )
{
	std::vector<FBXDLL::PositionUvTriangle> triangles_;
	_joints.clear();

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

	std::vector<FBXDLL::JointNode> jointNodes_;
	{
		const fbxsdk::FbxSkeleton* skeletonRoot_ = GetSkeletonRoot( bindPose_ );
		jointNodes_ = GetJointNodes( skeletonRoot_ );
		{
			FBXDLL::JointTransform temp_;
			const unsigned int size_ = ( unsigned int )jointNodes_.size();
			for ( unsigned int i = 0u; i < size_; ++i )
			{
				CopyMatrix( jointNodes_[ i ].m_node->EvaluateGlobalTransform(), temp_.m_transform );
				temp_.m_parent = jointNodes_[ i ].m_parent;
				_joints.push_back( temp_ );
			}
		}
	}

	const int nodeCount_ = bindPose_->GetCount();
	const fbxsdk::FbxMesh* mesh_;
	int i, j, k;
	for ( i = 0; i < nodeCount_; ++i )
	{
		mesh_ = bindPose_->GetNode( i )->GetMesh();
		if ( nullptr != mesh_ )
		{
			triangles_ = MeshToTriangles( mesh_ );
			break;
		}
	}
	const fbxsdk::FbxDeformer* deformer_;
	const fbxsdk::FbxSkin* skin_ = nullptr;
	const int deformerCount_ = mesh_->GetDeformerCount();
	for ( i = 0; i < deformerCount_; ++i )
	{
		deformer_ = mesh_->GetDeformer( i );
		if ( fbxsdk::FbxDeformer::EDeformerType::eSkin == deformer_->GetDeformerType() )
		{
			skin_ = ( fbxsdk::FbxSkin* )deformer_;
			break;
		}
	}
	const fbxsdk::FbxCluster* cluster_;
	const int clusterCount_ = skin_->GetClusterCount();
	const fbxsdk::FbxNode* link_;
	int jointIndex_, cpCount_, cpIndex_;
	float weight_;
	for ( i = 0; i < clusterCount_; ++i )
	{
		cluster_ = skin_->GetCluster( i );
		link_ = cluster_->GetLink();
		cpCount_ = cluster_->GetControlPointIndicesCount();
		jointIndex_ = -1;
		for ( j = 0; j < ( int )jointNodes_.size(); ++j )
		{
			if ( jointNodes_[ j ].m_node == link_ )
			{
				jointIndex_ = j;
				break;
			}
		}
		for ( j = 0; j < cpCount_; ++j )
		{
			weight_ = ( float )cluster_->GetControlPointWeights()[ j ];
			cpIndex_ = cluster_->GetControlPointIndices()[ j ];
			for ( k = 0u; k < ( int )triangles_.size(); ++k )
			{
				if ( triangles_[ k ].m_skinnedA.m_cpIndex == cpIndex_ )
				{
					int& weightCount_ = triangles_[ k ].m_skinnedA.m_weightCount;
					if ( weightCount_ < 4 )
					{
						( ( float* )&triangles_[ k ].m_skinnedA.m_jointWeights )[ weightCount_ ] = weight_;
						triangles_[ k ].m_skinnedA.m_jointIndices[ weightCount_ ] = jointIndex_;
						++weightCount_;
					}
					else
					{
						float mx = triangles_[ k ].m_skinnedA.m_jointWeights.x;
						mx = min( triangles_[ k ].m_skinnedA.m_jointWeights.y, mx );
						mx = min( triangles_[ k ].m_skinnedA.m_jointWeights.z, mx );
						mx = min( triangles_[ k ].m_skinnedA.m_jointWeights.w, mx );
						if ( weight_ > mx )
						{
							float mn = triangles_[ k ].m_skinnedA.m_jointWeights.x;
							int mnidx = 0;
							if ( triangles_[ k ].m_skinnedA.m_jointWeights.y < mn )
							{
								mn = triangles_[ k ].m_skinnedA.m_jointWeights.y;
								mnidx = 1;
							}
							if ( triangles_[ k ].m_skinnedA.m_jointWeights.z < mn )
							{
								mn = triangles_[ k ].m_skinnedA.m_jointWeights.z;
								mnidx = 2;
							}
							if ( triangles_[ k ].m_skinnedA.m_jointWeights.w < mn )
							{
								mn = triangles_[ k ].m_skinnedA.m_jointWeights.w;
								mnidx = 3;
							}
							( ( float* )&triangles_[ k ].m_skinnedA.m_jointWeights )[ mnidx ] = weight_;
							triangles_[ k ].m_skinnedA.m_jointIndices[ mnidx ] = jointIndex_;
						}
					}
				}

				if ( triangles_[ k ].m_skinnedB.m_cpIndex == cpIndex_ )
				{
					int& weightCount_ = triangles_[ k ].m_skinnedB.m_weightCount;
					if ( weightCount_ < 4 )
					{
						( ( float* )&triangles_[ k ].m_skinnedB.m_jointWeights )[ weightCount_ ] = weight_;
						triangles_[ k ].m_skinnedB.m_jointIndices[ weightCount_ ] = jointIndex_;
						++weightCount_;
					}
					else
					{
						float mx = triangles_[ k ].m_skinnedB.m_jointWeights.x;
						mx = min( triangles_[ k ].m_skinnedB.m_jointWeights.y, mx );
						mx = min( triangles_[ k ].m_skinnedB.m_jointWeights.z, mx );
						mx = min( triangles_[ k ].m_skinnedB.m_jointWeights.w, mx );
						if ( weight_ > mx )
						{
							float mn = triangles_[ k ].m_skinnedB.m_jointWeights.x;
							int mnidx = 0;
							if ( triangles_[ k ].m_skinnedB.m_jointWeights.y < mn )
							{
								mn = triangles_[ k ].m_skinnedB.m_jointWeights.y;
								mnidx = 1;
							}
							if ( triangles_[ k ].m_skinnedB.m_jointWeights.z < mn )
							{
								mn = triangles_[ k ].m_skinnedB.m_jointWeights.z;
								mnidx = 2;
							}
							if ( triangles_[ k ].m_skinnedB.m_jointWeights.w < mn )
							{
								mn = triangles_[ k ].m_skinnedB.m_jointWeights.w;
								mnidx = 3;
							}
							( ( float* )&triangles_[ k ].m_skinnedB.m_jointWeights )[ mnidx ] = weight_;
							triangles_[ k ].m_skinnedB.m_jointIndices[ mnidx ] = jointIndex_;
						}
					}
				}

				if ( triangles_[ k ].m_skinnedC.m_cpIndex == cpIndex_ )
				{
					int& weightCount_ = triangles_[ k ].m_skinnedC.m_weightCount;
					if ( weightCount_ < 4 )
					{
						( ( float* )&triangles_[ k ].m_skinnedC.m_jointWeights )[ weightCount_ ] = weight_;
						triangles_[ k ].m_skinnedC.m_jointIndices[ weightCount_ ] = jointIndex_;
						++weightCount_;
					}
					else
					{
						float mx = triangles_[ k ].m_skinnedC.m_jointWeights.x;
						mx = min( triangles_[ k ].m_skinnedC.m_jointWeights.y, mx );
						mx = min( triangles_[ k ].m_skinnedC.m_jointWeights.z, mx );
						mx = min( triangles_[ k ].m_skinnedC.m_jointWeights.w, mx );
						if ( weight_ > mx )
						{
							float mn = triangles_[ k ].m_skinnedC.m_jointWeights.x;
							int mnidx = 0;
							if ( triangles_[ k ].m_skinnedC.m_jointWeights.y < mn )
							{
								mn = triangles_[ k ].m_skinnedC.m_jointWeights.y;
								mnidx = 1;
							}
							if ( triangles_[ k ].m_skinnedC.m_jointWeights.z < mn )
							{
								mn = triangles_[ k ].m_skinnedC.m_jointWeights.z;
								mnidx = 2;
							}
							if ( triangles_[ k ].m_skinnedC.m_jointWeights.w < mn )
							{
								mn = triangles_[ k ].m_skinnedC.m_jointWeights.w;
								mnidx = 3;
							}
							( ( float* )&triangles_[ k ].m_skinnedC.m_jointWeights )[ mnidx ] = weight_;
							triangles_[ k ].m_skinnedC.m_jointIndices[ mnidx ] = jointIndex_;
						}
					}
				}
			}
		}
	}

	scene_->Destroy();
	manager_->Destroy();

	return triangles_;
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
