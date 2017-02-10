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

	void SpriteDrawText(const char* strText, SRect* rect, int Format, unsigned long color);
	void SpriteDraw(size_t pTexture, const SVector* pPosition, unsigned long color);
	size_t CreateTexture(const char* pSrcFile);

private:
	HFONT m_font;
	HWND m_hWnd;
	bool m_bFontAdded;
	std::vector<HDC> m_vDC;
	std::vector<HBITMAP> m_vTexture;
	std::vector<POINT> m_vSize;
};
#endif