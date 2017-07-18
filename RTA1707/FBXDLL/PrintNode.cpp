#include "stdafx.h"
#include "PrintNode.h"
#include <fbxsdk.h>

// Source: "Autodesk FBX SDK Documentation: Your First FBX SDK Program"
// http://docs.autodesk.com/FBX/2014/ENU/FBX-SDK-Documentation/files/GUID-29C09995-47A9-4B49-9535-2F6BDC5C4107.htm

fbxsdk::FbxString GetAttributeTypeName( fbxsdk::FbxNodeAttribute::EType _type )
{
	switch ( _type )
	{
		case fbxsdk::FbxNodeAttribute::eUnknown: return "unidentified";
		case fbxsdk::FbxNodeAttribute::eNull: return "null";
		case fbxsdk::FbxNodeAttribute::eMarker: return "marker";
		case fbxsdk::FbxNodeAttribute::eSkeleton: return "skeleton";
		case fbxsdk::FbxNodeAttribute::eMesh: return "mesh";
		case fbxsdk::FbxNodeAttribute::eNurbs: return "nurbs";
		case fbxsdk::FbxNodeAttribute::ePatch: return "patch";
		case fbxsdk::FbxNodeAttribute::eCamera: return "camera";
		case fbxsdk::FbxNodeAttribute::eCameraStereo: return "stereo";
		case fbxsdk::FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
		case fbxsdk::FbxNodeAttribute::eLight: return "light";
		case fbxsdk::FbxNodeAttribute::eOpticalReference: return "optical reference";
		case fbxsdk::FbxNodeAttribute::eOpticalMarker: return "marker";
		case fbxsdk::FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
		case fbxsdk::FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
		case fbxsdk::FbxNodeAttribute::eBoundary: return "boundary";
		case fbxsdk::FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
		case fbxsdk::FbxNodeAttribute::eShape: return "shape";
		case fbxsdk::FbxNodeAttribute::eLODGroup: return "lodgroup";
		case fbxsdk::FbxNodeAttribute::eSubDiv: return "subdiv";
		default: return "unknown";
	}
}
static int g_numTabs = 0;
void PrintTabs( void )
{
	for ( int i = 0; i < g_numTabs; ++i )
		printf( "\t" );
}
void PrintAttribute( const fbxsdk::FbxNodeAttribute* _nodeAttribute )
{
	if ( nullptr != _nodeAttribute )
	{
		PrintTabs();
		printf( "<attribute type='%s' name='%s'/>\n",
				GetAttributeTypeName( _nodeAttribute->GetAttributeType() ).Buffer(),
				_nodeAttribute->GetName() );
	}
}
void PrintNode( const fbxsdk::FbxNode* _node )
{
	PrintTabs();
	{
		const fbxsdk::FbxDouble3 translation_ = _node->LclTranslation.Get();
		const fbxsdk::FbxDouble3 rotation_ = _node->LclRotation.Get();
		const fbxsdk::FbxDouble3 scaling_ = _node->LclScaling.Get();
		printf( "<node name='%s' translation='(%f, %f, %f)' rotation='(%f, %f, %f)' scaling='(%f, %f, %f)'>\n",
				_node->GetName(),
				translation_[ 0 ], translation_[ 1 ], translation_[ 2 ],
				rotation_[ 0 ], rotation_[ 1 ], rotation_[ 2 ],
				scaling_[ 0 ], scaling_[ 1 ], scaling_[ 2 ]
		);
	}
	++g_numTabs;
	{
		int i, j;
		for ( i = 0, j = _node->GetNodeAttributeCount(); i < j; ++i )
			PrintAttribute( _node->GetNodeAttributeByIndex( i ) );
		for ( i = 0, j = _node->GetChildCount(); i < j; ++i )
			PrintNode( _node->GetChild( i ) );
	}
	--g_numTabs;
	PrintTabs();
	printf( "</node>\n" );
}