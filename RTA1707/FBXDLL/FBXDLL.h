#pragma once
#include "fbxdll_structs.h"

#ifdef FBXDLL_EXPORTS
#define FBXDLL_API __declspec( dllexport )
#else
#define FBXDLL_API __declspec( dllimport )
#endif

namespace FBXDLL
{
	FBXDLL_API std::vector<PositionUvTriangle> FBX_GetMeshBindPose( const char* const );
	FBXDLL_API std::vector<JointTransform> FBX_GetJointsBindPose( const char* const );
	FBXDLL_API AnimClip FBX_GetAnimationData( const char* const );
	FBXDLL_API bool TestFBX_PrintInfo( const char* const );
}
