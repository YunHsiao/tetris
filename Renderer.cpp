#include "Utility.h"
#include "Renderer.h"
#include "Window.h"

CRenderer CRenderer::s_renderer;

CRenderer::CRenderer() {}
CRenderer::~CRenderer() {}

bool CRenderer::onInit()
{
	return m_direct3d.onInit();
}

void CRenderer::DrawText(const char* strText, SRect* rect, int Format, unsigned long color)
{
	m_direct3d.DrawText(strText, rect, Format, color);
}

void CRenderer::SpriteDraw(unsigned int pTexture, const SVector* pPosition, unsigned long Color) 
{
	m_direct3d.SpriteDraw(pTexture, pPosition, Color);
}

unsigned int CRenderer::CreateTexture(const char* pSrcFile) 
{
	return m_direct3d.CreateTexture(pSrcFile);
}

void CRenderer::PreRender()
{
	m_direct3d.PreRender();
}

void CRenderer::PostRender()
{
	m_direct3d.PostRender();
}
