#include "Utility.h"
#include "OpenGL.h"
#include "Window.h"
#include "lodepng.h"
#include "ft2build.h"
#include FT_GLYPH_H

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
	glDeleteTextures(128, m_textures);
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

	FT_Set_Char_Size(face, 18 << 6, 18 << 6, 96, 96);
	m_font = glGenLists(128);
	glGenTextures(128, m_textures);
	for (unsigned char i = 0; i < 128; i++) {
		if (FT_Load_Glyph(face, FT_Get_Char_Index(face, i), FT_LOAD_DEFAULT))
			return false;
		FT_Glyph glyph;
		if (FT_Get_Glyph(face->glyph, &glyph))
			return false;

		FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1);
		FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
		FT_Bitmap& bitmap = bitmap_glyph->bitmap;

		unsigned width = 1, height = 1;
		while (width < bitmap.width) width <<= 1;
		while (height < bitmap.rows) height <<= 1;
		GLubyte* expanded_data = new GLubyte[2 * width * height];

		for (unsigned j = 0; j < height; j++) {
			for (unsigned i = 0; i < width; i++) {
				expanded_data[2 * (i + j * width)] = 
					expanded_data[2 * (i + j * width) + 1] =
					(i >= bitmap.width || j >= bitmap.rows) ?
					0 : bitmap.buffer[i + bitmap.width * j];
			}
		}

		glBindTexture(GL_TEXTURE_2D, m_textures[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
			0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data);
		delete[] expanded_data;

		glNewList(m_font + i, GL_COMPILE);
		glBindTexture(GL_TEXTURE_2D, m_textures[i]);
		glPushMatrix();
		glTranslatef(0, (float) -bitmap_glyph->top, 0);
		float x = (float) bitmap.width / width;
		float y = (float) bitmap.rows / height;
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2i(0, 0);
		glTexCoord2f(x, 0); glVertex2i(bitmap.width, 0);
		glTexCoord2f(x, y); glVertex2i(bitmap.width, bitmap.rows);
		glTexCoord2f(0, y); glVertex2i(0, bitmap.rows);
		glEnd();
		glPopMatrix();
		m_xoff[i] = (float) (face->glyph->advance.x >> 6);
		glTranslatef(m_xoff[i], 0, 0);
		glEndList();
	}

	FT_Done_Face(face);
	FT_Done_FreeType(library);
	/**/
	return true;
}

float* COpenGL::TextPosition(float* p, std::vector<float>& len, RECT* rct, int Format, unsigned line)
{
	float y(24.f), ymargin(5.f);

	if (Format & DT_CENTER) p[0] = rct->left + (rct->right - rct->left - len[line]) / 2.f;
	else if (Format & DT_RIGHT) p[0] = rct->right - len[line] - 1.f;
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

	std::vector<float> len;
	const char* str = strText;
	float xoff = 0;
	for (; *str; str++) {
		if (*str =='\n') {
			len.push_back(xoff);
			xoff = 0;
		} else {
			xoff += m_xoff[*str];
		}
	}
	if (xoff) len.push_back(xoff);

	float p[2];
	RECT* rct = rect ? reinterpret_cast<RECT*>(rect) : &m_rWnd;
	str = strText;
	for (unsigned line = 0; line < len.size(); line++, str++) {
		glPushMatrix();
		TextPosition(p, len, rct, Format, line);
		glTranslatef(p[0], p[1], 0.f);
		while (*str && *str != '\n')
			glCallList(m_font + *str++);
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

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (void*)image.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_vTexture.push_back(texture);
	POINT pt;
	pt.x = w;
	pt.y = h;
	m_vSize.push_back(pt);
	return m_vTexture.size() - 1;
}
