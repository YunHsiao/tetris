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
	size_t CreateTexture(unsigned char *image, unsigned w, unsigned h, bool bRGB);

	HWND m_hWnd;
	size_t m_font;
	std::vector<HDC> m_vDC;
	std::vector<HBITMAP> m_vTexture;
	std::vector<SPoint> m_vSize;
};
#endif