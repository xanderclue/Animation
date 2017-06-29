#pragma once

// Source: "Autodesk FBX SDK Documentation: Your First FBX SDK Program"
// http://docs.autodesk.com/FBX/2014/ENU/FBX-SDK-Documentation/files/GUID-29C09995-47A9-4B49-9535-2F6BDC5C4107.htm

#ifndef _FBXSDK_H_
#include <fbxsdk.h>
#endif

FbxString GetAttributeTypeName( FbxNodeAttribute::EType type )
{
	switch ( type )
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
static int numTabs = 0;
void PrintTabs( void )
{
	for ( int i = 0; i < numTabs; ++i )
		printf( "\t" );
}
void PrintAttribute( const FbxNodeAttribute* pAttribute )
{
	if ( nullptr != pAttribute )
	{
		PrintTabs();
		printf( "<attribute type='%s' name='%s'/>\n",
				GetAttributeTypeName( pAttribute->GetAttributeType() ).Buffer(),
				pAttribute->GetName() );
	}
}
void PrintNode( const FbxNode* pNode )
{
	PrintTabs();
	{
		const char* const nodeName = pNode->GetName();
		const FbxDouble3 translation = pNode->LclTranslation.Get();
		const FbxDouble3 rotation = pNode->LclRotation.Get();
		const FbxDouble3 scaling = pNode->LclScaling.Get();
		printf( "<node name='%s' translation='(%f, %f, %f)' rotation='(%f, %f, %f)' scaling='(%f, %f, %f)'>\n",
				nodeName,
				translation[ 0 ], translation[ 1 ], translation[ 2 ],
				rotation[ 0 ], rotation[ 1 ], rotation[ 2 ],
				scaling[ 0 ], scaling[ 1 ], scaling[ 2 ]
		);
	}
	++numTabs;
	{
		int i = 0;
		for ( ; i < pNode->GetNodeAttributeCount(); ++i )
			PrintAttribute( pNode->GetNodeAttributeByIndex( i ) );
		for ( i = 0; i < pNode->GetChildCount(); ++i )
			PrintNode( pNode->GetChild( i ) );
	}
	--numTabs;
	PrintTabs();
	printf( "</node>\n" );
}