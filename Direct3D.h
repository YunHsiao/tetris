#ifndef CDirect3D_H_
#define CDirect3D_H_

#define MAX_TEXTURE_STAGE 8

class CDirect3D
{
public:
	static CDirect3D* getInstance() { return &s_D3D9Dirver; }

private:
	CDirect3D();
	~CDirect3D();

public:
	bool onInit();

	void ResetDevice(bool bForceReset = false);
	void ResetBackBufferSize(int iWindowWidth, int iWindowHeight);

	void PreRender(DWORD flag = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL);
	HRESULT PostRender();

	inline LPDIRECT3DDEVICE9 GetD3D9Device() { return m_pD3D9Device; }
	inline int GetWindowWidth() { return m_D3DPresentParam.BackBufferWidth; }
	inline int GetWindowHeight() { return m_D3DPresentParam.BackBufferHeight; }

public:
	// ‰÷»æ◊¥Ã¨
	void SetWireframeRenderMode();
	void SetSolidRenderMode();
	DWORD GetRenderMode();
	void SetShaderMode(DWORD mode);
	void SetCullMode(DWORD mode);
	DWORD GetCullMode();

	void DepthEnable(BOOL bEnable);
	void DepthWriteEnable(BOOL bEnable);
	void AlphaBlendEnable(BOOL bEnable);
	void SetSrcRenderBlendFactor(DWORD dFactor);
	void SetDestRenderBlendFactor(DWORD dFactor);

	void AlphaTestEnable(BOOL bEnable);
	void AlphaFunction(DWORD dFactor);
	void AlphaReference(DWORD dFactor);

	// ƒ£∞Â◊¥Ã¨
	void StencilEnable(BOOL bEnable);
	void SetStencilReference(DWORD dValue);
	void SetStencilMask(DWORD dMask);
	void SetStencilWriteMask(DWORD dMask);
	void SetStencilFunc(DWORD dFunc);
	void SetStencilPass(DWORD dOperation);
	void SetStencilFail(DWORD dOperation);
	void SetStencilZFail(DWORD dOperation);

	// ŒÌ–ß
	void EnableFog(BOOL bEnable);
	void SetLinearFog(float fStart, float fEnd, DWORD color);
	void SetExponentialFog(float fDensity, DWORD color);

	// ∂®“Âª∫≥Â«¯
	void SetD3DFVF(DWORD FVF);
	HRESULT CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle = NULL);
	HRESULT CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle = NULL);
	void SetStreamSource(UINT uiStreamNum, IDirect3DVertexBuffer9* pVertexBuffer, UINT uiOffset, UINT uiStride);
	void SetIndices(IDirect3DIndexBuffer9* pIndexBuffer);
	void DrawPrimitive(D3DPRIMITIVETYPE type, UINT uiStartVertex, UINT uiDrawCount);
	void DrawIndexedPrimitive(D3DPRIMITIVETYPE type, UINT uiBaseVertexIndex, UINT uiMinVertexIndex, UINT uiVertexCount, UINT uiStartIndex, UINT uiDrawCount);

	// …Ë÷√±‰ªª
	void SetTransform(D3DTRANSFORMSTATETYPE type, const D3DXMATRIX* mat);
	void GetTransform(D3DTRANSFORMSTATETYPE type, D3DXMATRIX* mat);

	// …Ë÷√π‚’’
	void D3DLightEnable(BOOL bEnable);
	BOOL GetD3DLightEnable();
	void SetLightEnvironment(DWORD LEColor = 0xFF000000);
	void SetLight(int iIndex, const D3DLIGHT9* light);
	void LightEnable(int iIndex, BOOL bEnable);
	BOOL GetLightEnable(int iIndex);

	// ≤ƒ÷ 
	void SetMaterial(const D3DMATERIAL9* pMatrerial);

	// Ã˘Õº
	void SetTexture(DWORD Stage, LPDIRECT3DTEXTURE9 pTexture);
	void SetTextureAddressMode(D3DTEXTUREADDRESS eMode);
	void SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE dTexStageStateType, DWORD dValue);
	void SetTextureColorOpStageState(DWORD Stage, DWORD dValue);
	void SetTextureColorArg1StageState(DWORD Stage, DWORD dValue);
	void SetTextureColorArg2StageState(DWORD Stage, DWORD dValue);
	void SetTextureAlphaOpStageState(DWORD Stage, DWORD dValue);
	void SetTextureAlphaArg1StageState(DWORD Stage, DWORD dValue);
	void SetTextureAlphaArg2StageState(DWORD Stage, DWORD dValue);

	// ∆¡ƒªŒƒ◊÷°¢Õº∆¨
	void DrawText(const CHAR *strText, int iCount = -1, LPRECT rect = 0, DWORD Format = DT_LEFT, D3DCOLOR color = 0xffffffff);
	void SpriteDraw(IDirect3DTexture9* pTexture, const RECT *pSrcRect = 0, const D3DXVECTOR3 *pCenter = 0, const D3DXVECTOR3 *pPosition = 0, D3DCOLOR Color = 0xffffffff);
	void SpriteSetTransform(const D3DXMATRIX* pTransform);

	// Ãÿ–ß
	HRESULT CompileEffectFromFile(char* strEffectFile, ID3DXEffect** ppEffect);

protected:
	void InitPresentParam(HWND hWnd, long lWindowWidth, long lWindowHeight);
	void InitDefaultRenderState();
	void SetDefaultSamplerState();

private:
	IDirect3D9 *m_pDirect3D9;
	IDirect3DDevice9 *m_pD3D9Device;
	ID3DXSprite* m_pSprite;
	ID3DXFont *m_pFont;
	RECT m_rWnd;

	D3DPRESENT_PARAMETERS m_D3DPresentParam;

private:
	static CDirect3D s_D3D9Dirver;
};
#endif