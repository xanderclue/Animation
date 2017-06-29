#pragma once

#ifdef FBXDLL_EXPORTS
#define FBXDLL_API __declspec( dllexport )
#else
#define FBXDLL_API __declspec( dllimport )
#endif

namespace FBXDLL
{
	FBXDLL_API int TestFunc( int _x );
}