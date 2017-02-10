#include "Utility.h"
#include "GDI.h"
#include "Window.h"

CGDI::CGDI()
:m_DC(NULL)
,m_font(NULL)
,m_hWnd(NULL)
,m_screenDC(NULL)
,m_hb(NULL)
{
}

CGDI::~CGDI()
{
	for (auto it(m_vTextures.begin()); it != m_vTextures.end(); it++)
		Safe_Delete_Object(*it);
	for (auto it(m_vTextureDC.begin()); it != m_vTextureDC.end(); it++)
		Safe_Delete_DC(*it);
	m_vTextures.clear();
	m_vTextureDC.clear();
	m_vSizes.clear();
	Safe_Delete_Object(m_font);
	Safe_Delete_Object(m_hb);
	Safe_Delete_DC(m_DC);
}

bool CGDI::onInit()
{
	m_hWnd = CWindow::getInstance()->getHWND();
	HDC hDC = GetDC(m_hWnd);
	m_DC = CreateCompatibleDC(hDC);
	ReleaseDC(m_hWnd, hDC);
	m_font = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, ANSI_CHARSET, 0, 0, 
		ANTIALIASED_QUALITY, 0, TEXT("Kristen ITC"));
	if (!m_font) m_font = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, ANSI_CHARSET, 0, 0, 
		ANTIALIASED_QUALITY, 0, TEXT("ËÎÌו"));
	if (!m_font) return false;
	m_rWnd.right = CWindow::getInstance()->getWinWidth();
	m_rWnd.bottom = CWindow::getInstance()->getWinHeight();
	SetBkMode(m_DC, TRANSPARENT);
	SelectObject(m_DC, GetStockObject(DC_BRUSH));
	m_hb = (HBITMAP) LoadImage(NULL, TEXT("bg.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	return true;
}

void CGDI::SpriteDrawText(const char* strText, SRect* rect, int Format, unsigned long color)
{
	float a = ((color >> 24) & 0xff) / 256.f;
	short r = (color >> 16) & 0xff, g = (color >> 8) & 0xff, b = color & 0xff;
	r = (short) (r * a), g = (short) (g * a), b = (short) (b * a);
	SelectObject(m_DC, m_font);
	SetTextColor(m_DC, RGB(r, g, b));
	DrawText(m_DC, strText, -1, rect?reinterpret_cast<RECT*>(rect):&m_rWnd, Format);
}

void CGDI::SpriteDraw(unsigned int pTexture, const SVector* pPosition, unsigned long color) 
{
	if (pTexture >= m_vTextures.size()) return;
	int x, y; if (pPosition) x = (int) pPosition->x, y = (int) pPosition->y;
	else x = y = 0;
	BitBlt(m_DC, x, y, m_vSizes[pTexture].x, m_vSizes[pTexture].y, 
		m_vTextureDC[pTexture], 0, 0, SRCCOPY);

	float a = ((color >> 24) & 0xff) / 256.f;
	short r = (color >> 16) & 0xff, g = (color >> 8) & 0xff, b = color & 0xff;
	r = (short) (r * a), g = (short) (g * a), b = (short) (b * a);
	SetDCBrushColor(m_DC, RGB(r, g, b));
	BitBlt(m_DC, x, y, m_vSizes[pTexture].x, m_vSizes[pTexture].y, 
		m_vTextureDC[pTexture], 0, 0, MERGECOPY);
}

void CGDI::PreRender() {}
void CGDI::PostRender()
{
	SelectObject(m_DC, m_hb);
	m_screenDC = GetDC(m_hWnd);
	BitBlt(m_screenDC, 0, 0, m_rWnd.right, m_rWnd.bottom, m_DC, 0, 0, SRCCOPY);	
	ReleaseDC(m_hWnd, m_screenDC);
}

unsigned int CGDI::CreateTexture(const char* pSrcFile) 
{
	HBITMAP hb = (HBITMAP) LoadImageA(NULL, pSrcFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (!hb) return -1;
	BITMAP bmp;
	GetObject(hb, sizeof(BITMAP), &bmp);
	POINT pt; pt.x = bmp.bmWidth, pt.y = bmp.bmHeight;

	HDC hDC = GetDC(m_hWnd), textureDC;
	textureDC = CreateCompatibleDC(hDC);
	ReleaseDC(m_hWnd, hDC);
	SelectObject(textureDC, hb);
	m_vTextures.push_back(hb);
	m_vSizes.push_back(pt);
	m_vTextureDC.push_back(textureDC);
	return m_vTextures.size() - 1;
}
