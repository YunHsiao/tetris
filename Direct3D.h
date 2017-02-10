#ifndef CDirect3D_H_
#define CDirect3D_H_

#define MAX_TEXTURE_STAGE 8

class CDirect3D
{
public:
	CDirect3D();
	~CDirect3D();

	bool onInit();
	void PreRender(DWORD flag = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL);
	HRESULT PostRender();

	// ÆÁÄ»ÎÄ×Ö¡¢Í¼Æ¬
	void DrawText(const char* strText, SRect* rect = 0, unsigned long Format = DT_LEFT, unsigned long color = 0xffffffff);
	void SpriteDraw(unsigned int pTexture, const SVector* pPosition = 0, unsigned long Color = 0xffffffff);
	unsigned int CreateTexture(const char* pSrcFile);

protected:
	void InitPresentParam(HWND hWnd, long lWindowWidth, long lWindowHeight, D3DPRESENT_PARAMETERS* D3DPresentParam);
	void InitDefaultRenderState();

private:
	IDirect3D9* m_pDirect3D9;
	IDirect3DDevice9* m_pD3D9Device;
	ID3DXSprite* m_pSprite;
	ID3DXFont* m_pFont;
	RECT m_rWnd;
	std::vector<IDirect3DTexture9*> m_vTextures;
};
#endif