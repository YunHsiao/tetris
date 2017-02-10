#include "Utility.h"
#include "GDI.h"
#include "lodepng.h"

CGDI::CGDI()
:m_hWnd(NULL)
,m_framebuffer(0)
,m_fb(0)
{
}

CGDI::~CGDI()
{
	Safe_Delete_DC(m_DC);
	Safe_Delete_Object(m_hb);
	Safe_Delete_Array(m_fb);
	for (unsigned i = 0; i < m_vBuffer.size(); i++) 
		Safe_Delete_Array(m_vBuffer[i]);
	m_vSize.clear();
	m_vBuffer.clear();
}

bool CGDI::onInit()
{
	m_hWnd = CWindow::getInstance()->getHWND();
	m_rWnd.right = CWindow::getInstance()->getWinWidth();
	m_rWnd.bottom = CWindow::getInstance()->getWinHeight();
	m_width = (unsigned) m_rWnd.right;
	m_height = (unsigned) m_rWnd.bottom;

	HDC hDC = GetDC(m_hWnd);
	m_DC = CreateCompatibleDC(hDC);
	ReleaseDC(m_hWnd, hDC);
	BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), m_width, -(int)m_height, 1, 32, BI_RGB,
		m_width * m_height * 4, 0, 0, 0, 0 } };
	m_hb = CreateDIBSection(m_DC, &bi, DIB_RGB_COLORS, (void**) &m_framebuffer, 0, 0);
	SelectObject(m_DC, m_hb);
	m_fb = new unsigned char[m_width * m_height * 4];

	FT_Face face;
	FT_Library library;
	if (FT_Init_FreeType(&library)) 
		return false;
	if (FT_New_Face(library, "ITCKRIST.TTF", 0, &face)) 
		return false;
	FT_Set_Char_Size(face, FONT_SIZE << 6, FONT_SIZE << 6, 96, 96);

	for (unsigned char i = 0; i < 128; i++) {
		if (FT_Load_Glyph(face, FT_Get_Char_Index(face, i), FT_LOAD_DEFAULT))
			return false;
		FT_Glyph glyph;
		if (FT_Get_Glyph(face->glyph, &glyph))
			return false;

		FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1);
		FT_Bitmap& bitmap = ((FT_BitmapGlyph)glyph)->bitmap;

		m_xoff[i] = (float) (face->glyph->advance.x >> 6);
		CreateTexture(bitmap.buffer, bitmap.width, bitmap.rows, false);
		FT_Done_Glyph(glyph);
	}

	FT_Done_Face(face);
	FT_Done_FreeType(library);
	return true;
}

void CGDI::SpriteDrawText(const char* strText, SRect* rect, int Format, unsigned long color)
{
	std::vector<float> len;
	CountLines(strText, len);

	SVector p = { 0.f, 0.f, 0.f };
	RECT* rct = rect ? reinterpret_cast<RECT*>(rect) : &m_rWnd;
	for (unsigned line = 0; line < len.size(); line++, strText++) {
		TextPosition(&p, len.data(), len.size(), rct, Format, line);
		while (*strText && *strText != '\n') {
			size_t index = *strText;
			p.y -= m_vSize[index].y / 2 + 10;
			if (index != ' ') SpriteDraw(index, &p, color);
			p.y += m_vSize[index].y / 2 + 10;
			p.x += m_xoff[index];
			strText++;
		}
	}
}

void CGDI::SpriteDraw(size_t pTexture, const SVector* pPosition, unsigned long color)
{
	if (pTexture >= m_vBuffer.size()) 
		return;
	int x(0), y(0);
	if (pPosition) {
		x = (int) pPosition->x;
		y = (int) pPosition->y;
	}

	unsigned char ma, mr, mg, mb;
	ma = (color >> 24) & 0xff;
	mr = (color >> 16) & 0xff;
	mg = (color >> 8) & 0xff;
	mb = color & 0xff;

	unsigned char* tex = m_vBuffer[pTexture];
	int w = (int) m_vSize[pTexture].x;
	int h = (int) m_vSize[pTexture].y;
	unsigned ti, si;
	float a;
	for (int i = 0; i < h; i++) {
		if ((unsigned) (i + y) >= m_height) continue;
		for (int j = 0; j < w; j++) {
			if ((unsigned) (j + x) >= m_width) continue;
			ti = i * w * 4 + j * 4;
			si = (i + y) * m_width * 4 + (j + x) * 4;
			a = (tex[ti + 3] & ma) / 255.f;

			m_fb[si]   = (unsigned char) (m_fb[si]   * (1 - a) + (tex[ti]   & mr) * a);
			m_fb[si+1] = (unsigned char) (m_fb[si+1] * (1 - a) + (tex[ti+1] & mg) * a);
			m_fb[si+2] = (unsigned char) (m_fb[si+2] * (1 - a) + (tex[ti+2] & mb) * a);
		}
	}
}

void CGDI::PreRender() {}
void CGDI::PostRender()
{
	unsigned t;
	for (unsigned i = 0; i < m_height; i++) {
		for (unsigned j = 0; j < m_width; j++) {
			t = i * m_width * 4 + j * 4;
			m_framebuffer[i * m_width + j] = 
				(m_fb[t] << 16) + (m_fb[t + 1] << 8) + (unsigned)m_fb[t + 2];
		}
	}
	HDC screenDC = GetDC(m_hWnd);
	BitBlt(screenDC, 0, 0, m_rWnd.right, m_rWnd.bottom, m_DC, 0, 0, SRCCOPY);
	ReleaseDC(m_hWnd, screenDC);
}

size_t CGDI::CreateTexture(const char* pSrcFile)
{
	std::vector<unsigned char> image;
	unsigned w, h;
	if(lodepng::decode(image, w, h, pSrcFile)) return -1;
	return CreateTexture(image.data(), w, h, true);
}

size_t CGDI::CreateTexture(unsigned char *image, unsigned w, unsigned h, bool bRGBA) 
{
	unsigned char *ptr = new unsigned char[w * h * 4];
	if (bRGBA) {
		memcpy_s(ptr, w * h * 4, image, w * h * 4);
	} else {
		unsigned t;
		for (unsigned i = 0; i < h; i++) {
			for (unsigned j = 0; j < w; j++) {
				t = i * w * 4 + j * 4;
				ptr[t] = ptr[t + 1] = ptr[t + 2] = image[i * w + j];
				ptr[t + 3] = ptr[t] ? 0xff : 0;
			}
		}
	}

	SPoint pt;
	pt.x = w;
	pt.y = h;
	m_vSize.push_back(pt);
	m_vBuffer.push_back(ptr);
	return m_vBuffer.size() - 1;
}
