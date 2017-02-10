#include "Utility.h"
#include "OpenGL.h"
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

	// 使用freetype生成字体位图
	FT_Face face;
	FT_Library library;
	if (FT_Init_FreeType(&library)) 
		return false;
	if (FT_New_Face(library, "ITCKRIST.TTF", 0, &face)) 
		return false;

	FT_Set_Char_Size(face, FONT_SIZE << 6, FONT_SIZE << 6, 96, 96);
	m_font = glGenLists(128);
	for (unsigned char i = 0; i < 128; i++) {
		if (FT_Load_Glyph(face, FT_Get_Char_Index(face, i), FT_LOAD_DEFAULT))
			return false;
		FT_Glyph glyph;
		if (FT_Get_Glyph(face->glyph, &glyph))
			return false;

		FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1);
		FT_Bitmap& bitmap = ((FT_BitmapGlyph)glyph)->bitmap;

		std::vector<unsigned char> image;
		unsigned w = 1, h = 1;
		while (w < bitmap.width) w <<= 1;
		while (h < bitmap.rows) h <<= 1;

		image.reserve(w * h * 2);
		for (unsigned j = 0; j < h; j++) {
			for (unsigned i = 0; i < w; i++) {
				unsigned char c = 0;
				if (i < bitmap.width && j < bitmap.rows) 
					c = bitmap.buffer[j * bitmap.width + i];
				image.push_back(c);
				image.push_back(c ? 0xff : 0);
			}
		}
		m_xoff[i] = (float) (face->glyph->advance.x >> 6);
		size_t index = CreateTexture(image.data(), w, h, GL_LUMINANCE_ALPHA);

		glNewList(m_font + i, GL_COMPILE);
		glBindTexture(GL_TEXTURE_2D, m_vTexture[index]);
		glPushMatrix();
		glTranslatef(0, -(float)bitmap.rows, 0);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2i(0, 0);
		glTexCoord2f(1, 0); glVertex2i(w, 0);
		glTexCoord2f(1, 1); glVertex2i(w, h);
		glTexCoord2f(0, 1); glVertex2i(0, h);
		glEnd();
		glPopMatrix();
		glTranslatef(m_xoff[i], 0, 0);
		glEndList();
		FT_Done_Glyph(glyph);
	}

	FT_Done_Face(face);
	FT_Done_FreeType(library);
	return true;
}

void COpenGL::SpriteDrawText(const char* strText, SRect* rect, int Format, unsigned long color)
{
	unsigned char a, r, g, b;
	a = (color >> 24) & 0xff;
	r = (color >> 16) & 0xff;
	g = (color >> 8) & 0xff;
	b = color & 0xff;
	glColor4ub(r, g, b, a); 

	std::vector<float> len;
	CountLines(strText, len);

	SVector p;
	RECT* rct = rect ? reinterpret_cast<RECT*>(rect) : &m_rWnd;
	for (unsigned line = 0; line < len.size(); line++, strText++) {
		glPushMatrix();
		TextPosition(&p, len.data(), len.size(), rct, Format, line);
		glTranslatef(p.x, p.y, 0.f);
		while (*strText && *strText != '\n')
			glCallList(m_font + *strText++);
		glPopMatrix();
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

void COpenGL::SpriteDraw(size_t pTexture, const SVector* pPosition, unsigned long color)
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

	glColor4ub(r, g, b, a); 
	glBindTexture(GL_TEXTURE_2D, m_vTexture[pTexture]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.f, 0.f);	glVertex2f(x, y);
	glTexCoord2f(1.f, 0.f);	glVertex2f(x + m_vSize[pTexture].x, y);
	glTexCoord2f(1.f, 1.f);	glVertex2f(x + m_vSize[pTexture].x, y + m_vSize[pTexture].y);
	glTexCoord2f(0.f, 1.f);	glVertex2f(x, y + m_vSize[pTexture].y);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
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

size_t COpenGL::CreateTexture(const char* pSrcFile)
{   
	std::vector<unsigned char> image;
	unsigned w, h;
	if(lodepng::decode(image, w, h, pSrcFile)) return -1;
	return CreateTexture(image.data(), w, h, GL_RGBA);
}

size_t COpenGL::CreateTexture(unsigned char *image, unsigned w, unsigned h, GLenum format)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
		format, GL_UNSIGNED_BYTE, (void*) image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_vTexture.push_back(texture);
	SPoint pt;
	pt.x = w;
	pt.y = h;
	m_vSize.push_back(pt);
	return m_vTexture.size() - 1;
}
