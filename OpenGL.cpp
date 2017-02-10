#include "Utility.h"
#include "OpenGL.h"
#include "Window.h"
#include "gl/gl.h"

COpenGL::COpenGL()
{
}

COpenGL::~COpenGL()
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext( m_RC);
	ReleaseDC(m_hWnd, m_DC);
}

bool COpenGL::onInit()
{
	PIXELFORMATDESCRIPTOR pfd;
	int format;
	m_hWnd = CWindow::getInstance()->getHWND();
	m_DC = GetDC(m_hWnd);

	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	format = ChoosePixelFormat(m_DC, &pfd);
	SetPixelFormat(m_DC, format, &pfd);

	m_RC = wglCreateContext(m_DC);
	wglMakeCurrent(m_DC, m_RC);
	// 转换到D3D屏幕坐标系
	glPushMatrix();
	glTranslatef(-1.f, 1.f, 0.f);
	glScalef(2.f / CWindow::getInstance()->getWinWidth(), 
		-2.f / CWindow::getInstance()->getWinHeight(), 1.f);
	return true;
}

void COpenGL::SpriteDrawText(const char* strText, SRect* rect, int Format, unsigned long color)
{

}

void COpenGL::SpriteDraw(unsigned int pTexture, const SVector* pPosition, unsigned long color) 
{
	float x(0.f), y(0.f);
	if (pPosition) {
		x = pPosition->x;
		y = pPosition->y;
	}
	unsigned char a, r, g, b;
	a = (color >> 24) & 0xff;
	r = (color >> 16) & 0xff;
	g = (color >> 8) & 0xff;
	b = color & 0xff;

	glBegin(GL_QUADS);
	glColor4ub(r, g, b, a); 
	glVertex2f(x, y);
	glVertex2f(x+25.f, y);
	glVertex2f(x+25.f, y+25.f);
	glVertex2f(x, y+25.f);
	glEnd();
}

void COpenGL::PreRender() 
{
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void COpenGL::PostRender()
{
	SwapBuffers(m_DC);
}

unsigned int COpenGL::CreateTexture(const char* pSrcFile) 
{
	return -1;
}
