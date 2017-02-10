
//-----------Windows------------
#ifndef WINDOWS_USEFUL_TOOLS_
#define WINDOWS_USEFUL_TOOLS_
#define _CRT_RAND_S
#include <stdlib.h>
#include <Windows.h>
#include <MMSystem.h>
#pragma comment(lib,"Winmm.lib")
#include <assert.h>
#endif

//-----------D3D头、库文件------------
#ifndef D3D_USEFUL_TOOLS_
#define D3D_USEFUL_TOOLS_
#include <d3d9.h>
#pragma comment(lib,"d3d9.lib")
#include <d3dx9.h>
#pragma comment(lib,"d3dx9.lib")
#endif

//-----------安全释放宏------------
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
#endif

//-----------STL------------
#ifndef STL_TOOLS_
#define STL_TOOLS_
#include <sstream>
#include <string>
#include <vector>

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
#endif
