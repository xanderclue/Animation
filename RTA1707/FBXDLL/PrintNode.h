#pragma once

// Source: "Autodesk FBX SDK Documentation: Your First FBX SDK Program"
// http://docs.autodesk.com/FBX/2014/ENU/FBX-SDK-Documentation/files/GUID-29C09995-47A9-4B49-9535-2F6BDC5C4107.htm

#ifndef _FBXSDK_H_
#include <fbxsdk.h>
#endif

FbxString GetAttributeTypeName( FbxNodeAttribute::EType _type )
{
	switch ( _type )
	{
		case FbxNodeAttribute::eUnknown: return "unidentified";
		case FbxNodeAttribute::eNull: return "null";
		case FbxNodeAttribute::eMarker: return "marker";
		case FbxNodeAttribute::eSkeleton: return "skeleton";
		case FbxNodeAttribute::eMesh: return "mesh";
		case FbxNodeAttribute::eNurbs: return "nurbs";
		case FbxNodeAttribute::ePatch: return "patch";
		case FbxNodeAttribute::eCamera: return "camera";
		case FbxNodeAttribute::eCameraStereo: return "stereo";
		case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
		case FbxNodeAttribute::eLight: return "light";
		case FbxNodeAttribute::eOpticalReference: return "optical reference";
		case FbxNodeAttribute::eOpticalMarker: return "marker";
		case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
		case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
		case FbxNodeAttribute::eBoundary: return "boundary";
		case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
		case FbxNodeAttribute::eShape: return "shape";
		case FbxNodeAttribute::eLODGroup: return "lodgroup";
		case FbxNodeAttribute::eSubDiv: return "subdiv";
		default: return "unknown";
	}
}
static int g_numTabs = 0;
void PrintTabs( void )
{
	for ( int i = 0; i < g_numTabs; ++i )
		printf( "\t" );
}
void PrintAttribute( const FbxNodeAttribute* _nodeAttribute )
{
	if ( nullptr != _nodeAttribute )
	{
		PrintTabs();
		printf( "<attribute type='%s' name='%s'/>\n",
				GetAttributeTypeName( _nodeAttribute->GetAttributeType() ).Buffer(),
				_nodeAttribute->GetName() );
	}
}
void PrintNode( const FbxNode* _node )
{
	PrintTabs();
	{
		const FbxDouble3 translation_ = _node->LclTranslation.Get();
		const FbxDouble3 rotation_ = _node->LclRotation.Get();
		const FbxDouble3 scaling_ = _node->LclScaling.Get();
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