#pragma once

#ifdef FBXDLL_EXPORTS
#define FBXDLL_API __declspec( dllexport )
#else
#define FBXDLL_API __declspec( dllimport )
#endif

namespace FBXDLL
{
	FBXDLL_API int TestFunc_Square( int );
	FBXDLL_API bool TestFBX_PrintInfo( const char* const );
}