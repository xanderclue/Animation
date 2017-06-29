#pragma once

#ifdef FBXDLL_EXPORTS
#define FBXDLL_API __declspec( dllexport )
#else
#define FBXDLL_API __declspec( dllimport )
#endif
#include <vector>
struct PositionVertex
{
	float x, y, z;
};
struct PositionTriangle
{
	PositionVertex a, b, c;
};
namespace FBXDLL
{
	FBXDLL_API int TestFunc_Square( int );
	FBXDLL_API bool TestFBX_PrintInfo( const char* const );
	FBXDLL_API std::vector<PositionTriangle> FBX_GetBindPoseMesh( const char* const );
}