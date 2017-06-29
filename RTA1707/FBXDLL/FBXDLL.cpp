#include "stdafx.h"
#include "FBXDLL.h"
#include <stdio.h>
#include <fbxsdk.h>

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