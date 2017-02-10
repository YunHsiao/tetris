#include "Utility.h"
#include "Renderer.h"

CRenderer::CRenderer() 
{
	m_rWnd.left = 0;
	m_rWnd.top = 0;
}

CRenderer::~CRenderer() {}

SVector* CRenderer::TextPosition(SVector* p, float* len, size_t size, RECT* rct, int Format, unsigned line)
{
	float y(24.f), ymargin(5.f);

	if (Format & DT_CENTER) p->x = rct->left + (rct->right - rct->left - len[line]) / 2.f;
	else if (Format & DT_RIGHT) p->x = rct->right - len[line] - 1.f;
	else p->x = rct->left + 1.f;

	if (Format & DT_VCENTER) p->y = rct->top + (rct->bottom - rct->top - size * y 
		- (size - 1) * ymargin) / 2.f + (line + 1) * y + line * ymargin;
	else if (Format & DT_BOTTOM) p->y = rct->bottom - (size - line) * y 
		- (size - line - 1) * ymargin;
	else p->y = rct->top + (line + 1) * y + line * ymargin;

	return p;
}

void CRenderer::CountLines(const char* str, std::vector<float>& len) 
{
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
}