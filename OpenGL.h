#ifndef COpenGL_H_
#define COpenGL_H_

#include "gl/gl.h"
#pragma comment(lib,"OpenGL32.lib")
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
	void SpriteDraw(size_t pTexture, const SVector* pPosition, unsigned long color);
	size_t CreateTexture(const char* pSrcFile);

private:
	float* TextPosition(float* p, std::vector<float>& len, RECT* rct, int Format, unsigned line);

	HDC m_DC;
	HGLRC m_RC;
	HWND m_hWnd;
	GLuint m_font, m_textures[128];
	float m_xoff[128];
	std::vector<GLuint> m_vTexture;
	std::vector<POINT> m_vSize;
};
#endif