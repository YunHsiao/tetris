#ifndef CGDI_H_
#define CGDI_H_

#include "Renderer.h"

class CGDI : public CRenderer
{
public:
	CGDI();
	~CGDI();

	bool onInit();
	void PreRender();
	void PostRender();
	HDC getDC() { return m_DC; }

	void SpriteDrawText(const char* strText, SRect* rect, int Format, unsigned long color);
	void SpriteDraw(unsigned int pTexture, const SVector* pPosition, unsigned long color);
	unsigned int CreateTexture(const char* pSrcFile);

private:
	HDC m_DC, m_screenDC;
	HFONT m_font;
	HWND m_hWnd;
	HBITMAP m_hb;
	std::vector<HDC> m_vTextureDC;
	std::vector<HBITMAP> m_vTextures;
	std::vector<POINT> m_vSizes;
};
#endif