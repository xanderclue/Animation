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
	FbxManager* sdkManager_ = FbxManager::Create();
	FbxIOSettings* ioSettings_ = FbxIOSettings::Create( sdkManager_, IOSROOT );
	sdkManager_->SetIOSettings( ioSettings_ );
	FbxImporter* importer_ = FbxImporter::Create( sdkManager_, "" );
	if ( !importer_->Initialize( _file, -1, sdkManager_->GetIOSettings() ) )
	{
		printf( "Error: %s\n", importer_->GetStatus().GetErrorString() );
		importer_->Destroy();
		sdkManager_->Destroy();
		return false;
	}
	FbxScene* scene_ = FbxScene::Create( sdkManager_, "myScene" );
	importer_->Import( scene_ );
	importer_->Destroy();
	FbxNode* rootNode_ = scene_->GetRootNode();
	if ( rootNode_ )
		for ( int i = 0; i < rootNode_->GetChildCount(); ++i )
			PrintNode( rootNode_->GetChild( i ) );
	sdkManager_->Destroy();
	return true;
}