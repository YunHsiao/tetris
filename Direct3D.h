#ifndef CDirect3D_H_
#define CDirect3D_H_

#include <d3d9.h>
#pragma comment(lib,"d3d9.lib")
#include <d3dx9.h>
#pragma comment(lib,"d3dx9.lib")
#include "Renderer.h"

class CDirect3D : public CRenderer
{
public:
	CDirect3D();
	~CDirect3D();

	bool onInit();
	void PreRender();
	void PostRender();

	void SpriteDrawText(const char* strText, SRect* rect, int Format, unsigned long color);
	void SpriteDraw(size_t pTexture, const SVector* pPosition, unsigned long color);
	size_t CreateTexture(const char* pSrcFile);

private:
	IDirect3D9* m_pDirect3D9;
	IDirect3DDevice9* m_pD3D9Device;
	ID3DXSprite* m_pSprite;
	ID3DXFont* m_pFont;
	std::vector<IDirect3DTexture9*> m_vTextures;
};
#endif