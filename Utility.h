#if defined(_DEBUG) && !defined(_WIN64)
#include "vld.h"
#endif

//-----------Windows------------
#ifndef WINDOWS_USEFUL_TOOLS_
#define WINDOWS_USEFUL_TOOLS_
#define _CRT_RAND_S
#include <stdlib.h>
#include <Windows.h>
#include <MMSystem.h>
#include <assert.h>
#pragma comment(lib,"Winmm.lib")
#endif

//-----------°²È«ÊÍ·Åºê------------
#ifndef SAFE_CLEANUP_
#define SAFE_CLEANUP_
template<typename T>inline void Safe_Delete( T* & p )
{
	if(p) { delete p ; p = NULL; }
}
template<typename T>inline void Safe_Release( T* & p )
{
	if(p) { p->Release() ; p = NULL; }
}
template<typename T>inline void Safe_Delete_Array( T* & p )
{
	if(p) { delete[] p; p = NULL; }
}
template<typename T>inline void Safe_Delete_Object( T* & p )
{
	if(p) { DeleteObject(p); p = NULL; }
}
template<typename T>inline void Safe_Delete_DC( T* & p )
{
	if(p) { DeleteDC(p); p = NULL; }
}
#endif

//-----------STL------------
#ifndef STL_TOOLS_
#define STL_TOOLS_
#include <sstream>
#include <string>
#include <vector>
#include <map>

template<typename T>
std::string toString(const T& value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}
#endif

//----------Game Tools----------
#ifndef GAME_TOOLS_
#define GAME_TOOLS_
struct SRect
{
	long left;
	long top;
	long right;
	long bottom;
};

struct SVector
{
	float x;
	float y;
	float z;
};

struct SPoint
{
	long x;
	long y;
};
#endif
