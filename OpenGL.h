#ifndef COpenGL_H_
#define COpenGL_H_

#include "Renderer.h"

class COpenGL : public CRenderer
{
public:
	COpenGL();
	~COpenGL();

	bool onInit();
	void PreRender();
	void PostRender();

	void SpriteDrawText(const char* strText, SRect* rect, int Format, unsigned long color);
	void SpriteDraw(unsigned int pTexture, const SVector* pPosition, unsigned long color);
	unsigned int CreateTexture(const char* pSrcFile);

private:
	HDC m_DC;
	HGLRC m_RC;
	HWND m_hWnd;
	std::vector<HDC> m_vTextureDC;
};
#endif