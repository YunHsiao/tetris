#include "Utility.h"
#include "GDI.h"
#include "Window.h"
#include "lodepng.h"

CGDI::CGDI()
:m_font(NULL)
,m_hWnd(NULL)
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
	Safe_Delete_Object(m_font);
}

bool CGDI::onInit()
{
	m_hWnd = CWindow::getInstance()->getHWND();
	m_font = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, ANSI_CHARSET, 0, 0, 
		ANTIALIASED_QUALITY, 0, TEXT("Kristen ITC"));
	if (!m_font) m_font = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, ANSI_CHARSET, 0, 0, 
		ANTIALIASED_QUALITY, 0, TEXT("ËÎÌו"));
	if (!m_font) return false;
	m_rWnd.right = CWindow::getInstance()->getWinWidth();
	m_rWnd.bottom = CWindow::getInstance()->getWinHeight();
	CreateTexture("bg.png");
	SetBkMode(m_vDC[0], TRANSPARENT);
	SelectObject(m_vDC[0], GetStockObject(DC_BRUSH));
	return true;
}

void CGDI::SpriteDrawText(const char* strText, SRect* rect, int Format, unsigned long color)
{
	float a = ((color >> 24) & 0xff) / 256.f;
	short r = (color >> 16) & 0xff;
	short g = (color >> 8) & 0xff;
	short b = color & 0xff;
	r = (short) (r * a);
	g = (short) (g * a);
	b = (short) (b * a);
	SelectObject(m_vDC[0], m_font);
	SetTextColor(m_vDC[0], RGB(r, g, b));
	DrawText(m_vDC[0], strText, -1, rect?reinterpret_cast<RECT*>(rect):&m_rWnd, Format);
}

void CGDI::SpriteDraw(unsigned int pTexture, const SVector* pPosition, unsigned long color) 
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

unsigned int CGDI::CreateTexture(const char* pSrcFile) 
{
	std::vector<unsigned char> image;
	unsigned w, h;
	if(lodepng::decode(image, w, h, pSrcFile)) return -1;

	HDC hDC = GetDC(m_hWnd);
	HDC textureDC = CreateCompatibleDC(hDC);
	ReleaseDC(m_hWnd, hDC);
	BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), w, -(int)h, 1, 32, BI_RGB,
		w * h * 4, 0, 0, 0, 0 } };
	unsigned int* ptr;
	HBITMAP hb = CreateDIBSection(textureDC, &bi, DIB_RGB_COLORS, (void**) &ptr, 0, 0);
	if (!hb) return -1;
	for (unsigned i = 0; i < image.size(); i += 4, ptr++) 
		*ptr = (image[i] << 16) + (image[i+1] << 8) 
			+ (image[i+2]) + (image[i+3] << 24);

	POINT pt;
	pt.x = w;
	pt.y = h;
	SelectObject(textureDC, hb);
	m_vTexture.push_back(hb);
	m_vSize.push_back(pt);
	m_vDC.push_back(textureDC);
	return m_vTexture.size() - 1;
}
