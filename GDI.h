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
	size_t CreateTexture(unsigned char *image, unsigned w, unsigned h, bool bRGBA);

	HWND m_hWnd;
	HDC m_DC;
	HBITMAP m_hb;
	unsigned *m_framebuffer, m_width, m_height;
	unsigned char *m_fb;
	std::vector<unsigned char*> m_vBuffer;
	std::vector<SPoint> m_vSize;
};
#endif