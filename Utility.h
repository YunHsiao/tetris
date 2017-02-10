#if defined(_DEBUG) && !defined(_WIN64)
//#include "vld.h"
#endif

//----------Windows----------
#ifndef WINDOWS_USEFUL_TOOLS_
#define WINDOWS_USEFUL_TOOLS_
#define _CRT_RAND_S
#include <stdlib.h>
#include <xmmintrin.h>
#include <Windows.h>
#include <MMSystem.h>
#include <assert.h>
#pragma comment(lib,"Winmm.lib")
#endif

//----------°²È«ÊÍ·Åºê----------
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
template<typename T>inline void Safe_Delete_Object( T & p )
{
	if(p) { DeleteObject(p); p = NULL; }
}
template<typename T>inline void Safe_Delete_DC( T & p )
{
	if(p) { DeleteDC(p); p = NULL; }
}
#endif

//----------STL----------
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

template<typename T>
long long toNumber(std::basic_string<T> str)
{
	int n(10), m(0);
	long long num(0);
	int ss = ' ', sn = '\n', st = '\t';
	int s0 = '0', s9 = '9';
	int sa = 'a', sA = 'A';
	int sb = 'b', sB = 'B';
	int sf = 'f', sF = 'F';
	int sx = 'x', sX = 'X';
	auto b = str.begin(), e = str.end();
	// trim out spaces
	for (; b != e; b++)
		if (*b != ss && *b != sn && *b != st)
			break;
	if (b == e) return 0;
	for (e--; e != b; e--)
		if (*e != ss && *e != sn && *e != st)
			break;
	// decide radix (2, 8, 10, 16)
	if (b != e++ && *b == s0) {
		if (*(b + 1) == sx || *(b + 1) == sX) {
			n = 16; 
			b++;
		}
		else if (*(b + 1) == sb || *(b + 1) == sB) {
			n = 2; 
			b++;
		}
		else {
			n = 8;
		}
		b++;
	}
	// parse number
	for (auto it = b; it != e; it++) {
		num *= n;
		if (*it >= s0 && *it <= s9) {
			m = *it - s0;
		}
		else if (*it >= sa && *it <= sf) {
			m = *it - sa + 10;
		}
		else if (*it >= sA && *it <= sF) {
			m = *it - sA + 10;
		}
		else return 0;
		if (m >= n) return 0;
		num += m;
	}
	return num;
}

template<typename T>
long long toNumber(T const* str)
{
	return toNumber(std::basic_string<T>(str));
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

//----------Game Resources----------
#ifndef GAME_RESOURCES_
#define GAME_RESOURCES_

#define TRS_TEXTURE_BG		"res\\bg.png"
#define TRS_TEXTURE_TILE	"res\\tile.png"
#define TRS_TEXTURE_PREVIEW "res\\preview.png"

#define TRS_TEXTURE_BUTTON0 "res\\button0.png"
#define TRS_TEXTURE_BUTTON1 "res\\button1.png"
#define TRS_TEXTURE_BUTTON2 "res\\button2.png"

#define TRS_FONT_FILE		"res\\ITCKRIST.ttf"
#define TRS_FONT_NAME		"Kristen ITC"
#endif
