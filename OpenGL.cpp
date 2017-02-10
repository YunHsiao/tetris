#include "Utility.h"
#include "OpenGL.h"
#include "Window.h"
#include "lodepng.h"

COpenGL::COpenGL()
{
}

COpenGL::~COpenGL()
{
	for (auto it(m_vTexture.begin()); it != m_vTexture.end(); it++)
		glDeleteTextures(1, &*it);
	m_vTexture.clear();
	m_vSize.clear();
	glDeleteLists(m_font, 128);
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext( m_RC);
	ReleaseDC(m_hWnd, m_DC);
}

bool COpenGL::onInit()
{
	PIXELFORMATDESCRIPTOR pfd;
	int format;
	m_hWnd = CWindow::getInstance()->getHWND();
	m_rWnd.right = CWindow::getInstance()->getWinWidth();
	m_rWnd.bottom = CWindow::getInstance()->getWinHeight();
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
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	HFONT hFont = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, ANSI_CHARSET, 0, 0, 
		ANTIALIASED_QUALITY, 0, TEXT("Kristen ITC"));
	if (!hFont) hFont = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, ANSI_CHARSET, 0, 0, 
		ANTIALIASED_QUALITY, 0, TEXT("宋体"));
	if (!hFont) return false;
	HFONT hOldFont = (HFONT)SelectObject(m_DC, hFont);
	DeleteObject(hOldFont);
	m_font = glGenLists(128);
	wglUseFontBitmaps(m_DC, 0, 128, m_font);
	return true;
}

float* COpenGL::TextPosition(float* p, std::vector<int>& len, RECT* rct, int Format, unsigned line)
{
	float x(11.f), y(24.f), ymargin(5.f);

	if (Format & DT_CENTER) p[0] = rct->left + (rct->right - rct->left - len[line] * x) / 2.f;
	else if (Format & DT_RIGHT) p[0] = rct->right - (len[line] + 1) * x;
	else p[0] = rct->left + 1.f;

	if (Format & DT_VCENTER) p[1] = rct->top + (rct->bottom - rct->top - len.size() * y 
		- (len.size() - 1) * ymargin) / 2.f + (line + 1) * y + line * ymargin;
	else if (Format & DT_BOTTOM) p[1] = rct->bottom - (len.size() - line) * y 
		- (len.size() - line - 1) * ymargin;
	else p[1] = rct->top + (line + 1) * y + line * ymargin;

	return p;
}

void COpenGL::SpriteDrawText(const char* strText, SRect* rect, int Format, unsigned long color)
{
	unsigned char a, r, g, b;
	a = (color >> 24) & 0xff;
	r = (color >> 16) & 0xff;
	g = (color >> 8) & 0xff;
	b = color & 0xff;
	glColor4ub(r, g, b, a); 

	std::vector<int> len;
	const char* str = strText;
	int l = 0, lmax = 0;
	for(; *str != 0; str++, l++) {
		if (*str =='\n') {
			len.push_back(l);
			if (l > lmax) lmax = l;
			l = 0;
		}
	}
	if (l) len.push_back(l);

	float p[2];
	RECT* rct = rect ? reinterpret_cast<RECT*>(rect) : &m_rWnd;
	str = strText;
	for (unsigned line = 0; line < len.size(); line++) {
		glRasterPos2fv(TextPosition(p, len, rct, Format, line));
		for (int i = 0; i < len[line]; i++, str++) 
			glCallList(m_font + *str);
	}
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

	/**/
	glColor4ub(r, g, b, a); 
	glBindTexture(GL_TEXTURE_2D, m_vTexture[pTexture]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.f, 0.f);	glVertex2f(x, y);
	glTexCoord2f(1.f, 0.f);	glVertex2f(x + m_vSize[pTexture].x, y);
	glTexCoord2f(1.f, 1.f);	glVertex2f(x + m_vSize[pTexture].x, y + m_vSize[pTexture].y);
	glTexCoord2f(0.f, 1.f);	glVertex2f(x, y + m_vSize[pTexture].y);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	/**
	glBegin(GL_QUADS);
	glColor4ub(r, g, b, a); 
	glVertex2f(x, y);
	glVertex2f(x+25.f, y);
	glVertex2f(x+25.f, y+25.f);
	glVertex2f(x, y+25.f);
	glEnd();
	/**/
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
	std::vector<unsigned char> image;
	unsigned w, h;
	if(lodepng::decode(image, w, h, pSrcFile)) return -1;

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA, w, h, 0,
	GL_RGBA, GL_UNSIGNED_BYTE, (void*) image.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	m_vTexture.push_back(texture);
	POINT pt;
	pt.x = w;
	pt.y = h;
	m_vSize.push_back(pt);
	return m_vTexture.size() - 1;
}
