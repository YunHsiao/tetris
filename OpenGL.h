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
	void SpriteDraw(size_t pTexture, const SVector* pPosition, unsigned long color);
	size_t CreateTexture(const char* pSrcFile);

private:
	size_t CreateTexture(unsigned char *image, unsigned w, unsigned h, GLenum format);

	HDC m_DC;
	HGLRC m_RC;
	HWND m_hWnd;
	GLuint m_font;
	std::vector<GLuint> m_vTexture;
	std::vector<SPoint> m_vSize;
};
#endif