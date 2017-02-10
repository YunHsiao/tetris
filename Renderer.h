#ifndef CRenderer_H_
#define CRenderer_H_

#include "Window.h"
#include "ft2build.h"
#include FT_GLYPH_H
#ifndef _WIN64
#ifdef _DEBUG
#pragma comment(lib,"freetype27d.lib")
#else
#pragma comment(lib,"freetype27.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib,"freetype27d_64.lib")
#else
#pragma comment(lib,"freetype27_64.lib")
#endif
#endif

#define FONT_SIZE 18

class CRenderer
{
public:
	CRenderer();
	virtual ~CRenderer();
	virtual bool onInit() = 0;
	virtual void PreRender() = 0;
	virtual void PostRender() = 0;
	
	virtual void SpriteDrawText(const char* strText, SRect* rect = 0, int Format = DT_LEFT, 
		unsigned long color = 0xffffffff) = 0;
	virtual void SpriteDraw(size_t pTexture, const SVector* pPosition = 0,
		unsigned long color = 0xffffffff) = 0;
	virtual size_t CreateTexture(const char* pSrcFile) = 0;
protected:
	SVector* TextPosition(SVector* p, float* len, size_t size, RECT* rct, int Format, unsigned line);
	void CountLines(const char* str, std::vector<float>& len);

	float m_xoff[128];
	RECT m_rWnd;
};
#endif