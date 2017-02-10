#ifndef COpenGL_H_
#define COpenGL_H_

#include "gl/gl.h"
#pragma comment(lib,"OpenGL32.lib")
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
	float* TextPosition(float* p, std::vector<int>& len, RECT* rct, int Format, unsigned line);

	HDC m_DC;
	HGLRC m_RC;
	HWND m_hWnd;
	GLuint m_font;
	std::vector<GLuint> m_vTexture;
	std::vector<POINT> m_vSize;
};
#endif