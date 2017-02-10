#include "Utility.h"
#include "GDI.h"
#include "lodepng.h"

CGDI::CGDI()
:m_hWnd(NULL)
{
}

CGDI::~CGDI()
{
	for (auto it(m_vTexture.begin()); it != m_vTexture.end(); it++)
		Safe_Delete_Object(*it);
	for (auto it(m_vDC.begin()); it != m_vDC.end(); it++)
		Safe_Delete_DC(*it);
	m_vTexture.clear();
	m_vDC.clear();
	m_vSize.clear();
}

bool CGDI::onInit()
{
	m_hWnd = CWindow::getInstance()->getHWND();
	m_rWnd.right = CWindow::getInstance()->getWinWidth();
	m_rWnd.bottom = CWindow::getInstance()->getWinHeight();
	CreateTexture("bg.png");
	SetBkMode(m_vDC[0], TRANSPARENT);
	SelectObject(m_vDC[0], GetStockObject(DC_BRUSH));

	FT_Face face;
	FT_Library library;
	if (FT_Init_FreeType(&library)) 
		return false;
	if (FT_New_Face(library, "ITCKRIST.TTF", 0, &face)) 
		return false;
	FT_Set_Char_Size(face, FONT_SIZE << 6, FONT_SIZE << 6, 96, 96);

	m_font = m_vDC.size() - 1;
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
			size_t index = m_font + *strText;
			p.y -= m_vSize[index].y;
			if (*strText != ' ') SpriteDraw(index, &p, color);
			p.y += m_vSize[index].y;
			p.x += m_xoff[*strText++];
		}
	}
}

void CGDI::SpriteDraw(size_t pTexture, const SVector* pPosition, unsigned long color)
{
	if (pTexture >= m_vTexture.size()) 
		return;
	int x(0), y(0);
	if (pPosition) {
		x = (int) pPosition->x;
		y = (int) pPosition->y;
	}

	float a = ((color >> 24) & 0xff) / 256.f;
	short r, g, b;
	r = (color >> 16) & 0xff;
	g = (color >> 8) & 0xff;
	b = color & 0xff;
	r = (short) (r * a);
	g = (short) (g * a);
	b = (short) (b * a);
	SetDCBrushColor(m_vDC[0], RGB(r, g, b));
	BitBlt(m_vDC[0], x, y, m_vSize[pTexture].x, m_vSize[pTexture].y, 
		m_vDC[pTexture], 0, 0, MERGECOPY);
}

void CGDI::PreRender() {}
void CGDI::PostRender()
{
	SelectObject(m_vDC[0], m_vTexture[0]);
	HDC screenDC = GetDC(m_hWnd);
	BitBlt(screenDC, 0, 0, m_rWnd.right, m_rWnd.bottom, m_vDC[0], 0, 0, SRCCOPY);
	ReleaseDC(m_hWnd, screenDC);
}

size_t CGDI::CreateTexture(const char* pSrcFile)
{
	std::vector<unsigned char> image;
	unsigned w, h;
	if(lodepng::decode(image, w, h, pSrcFile)) return -1;
	return CreateTexture(image.data(), w, h, true);
}

size_t CGDI::CreateTexture(unsigned char *image, unsigned w, unsigned h, bool bRGB) 
{
	HDC hDC = GetDC(m_hWnd);
	HDC textureDC = CreateCompatibleDC(hDC);
	ReleaseDC(m_hWnd, hDC);
	BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), w, -(int)h, 1, 32, BI_RGB,
		w * h * 4, 0, 0, 0, 0 } };
	unsigned int* ptr;
	HBITMAP hb = CreateDIBSection(textureDC, &bi, DIB_RGB_COLORS, (void**) &ptr, 0, 0);
	if (!hb) return -1;
	if (bRGB) {
		unsigned size = w * h * 4;
		for (unsigned i = 0; i < size; i += 4, ptr++) 
			*ptr = (image[i] << 16) + (image[i+1] << 8) 
				+ (image[i+2]) + (image[i+3] << 24);
	} else {
		unsigned size = w * h;
		for (unsigned i = 0; i < size; i++, ptr++) 
			*ptr = (image[i] << 16) + (image[i] << 8) 
			+ image[i] + ((image[i] ? 0xff : 0) << 24);
	}

	SPoint pt;
	pt.x = w;
	pt.y = h;
	SelectObject(textureDC, hb);
	m_vTexture.push_back(hb);
	m_vSize.push_back(pt);
	m_vDC.push_back(textureDC);
	return m_vTexture.size() - 1;
}
