#include "Utility.h"
#include "Direct3D.h"
#include "Window.h"

CDirect3D CDirect3D::s_D3D9Dirver;

CDirect3D::CDirect3D():
m_pDirect3D9(NULL),
m_pD3D9Device(NULL),
m_pSprite(NULL),
m_pFont(NULL)
{
	ZeroMemory(&m_D3DPresentParam, sizeof(D3DPRESENT_PARAMETERS));
	m_rWnd.left = 0;
	m_rWnd.top = 0;
}

CDirect3D::~CDirect3D()
{
	Safe_Release(m_pSprite);
	Safe_Release(m_pFont);
	Safe_Release(m_pD3D9Device);
	Safe_Release(m_pDirect3D9);
}

bool CDirect3D::onInit()
{
	HWND hWnd = CWindow::getInstance()->getHWND();
	long lWindowWidth = CWindow::getInstance()->getWinWidth();
	long lWindowHeight =CWindow::getInstance()->getWinHeight();
	HRESULT hr;
	m_pDirect3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (NULL == m_pDirect3D9)
		return false;

	// D3D检测
	D3DCAPS9 caps;
	m_pDirect3D9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

	// 判断当前显卡是否支持硬件顶点处理
	int vp = 0;
	if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// 初始化相关参数
	InitPresentParam(hWnd, lWindowWidth, lWindowHeight);

	hr = m_pDirect3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, vp, &m_D3DPresentParam, &m_pD3D9Device);
	if (FAILED(hr))
		return false;

	hr = D3DXCreateFont(m_pD3D9Device, 30, 0, FW_DONTCARE, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("微软雅黑"), &m_pFont);
	if (FAILED(hr)) {
		hr = D3DXCreateFont(m_pD3D9Device, 30, 0, FW_DONTCARE, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("宋体"), &m_pFont);
		if (FAILED(hr))
			return false;
	}
	if (FAILED(hr = D3DXCreateSprite(m_pD3D9Device, &m_pSprite))) 
		return false;

	// 初始化渲染状态
	InitDefaultRenderState();
	return true;
}

void CDirect3D::InitPresentParam(HWND hWnd, long lWindowWidth, long lWindowHeight)
{
	m_rWnd.right = lWindowWidth;
	m_rWnd.bottom = lWindowHeight;

	D3DDISPLAYMODE d3ddm;
	m_pDirect3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

	m_D3DPresentParam.BackBufferWidth            = lWindowWidth;					// 后台缓冲区的宽度
	m_D3DPresentParam.BackBufferHeight           = lWindowHeight;					// 后台缓冲区的高度
	m_D3DPresentParam.BackBufferFormat           = d3ddm.Format;					// 后台缓冲区的像素格式
	m_D3DPresentParam.BackBufferCount            = 1;								// 后台缓冲区的数量
	m_D3DPresentParam.MultiSampleType            = D3DMULTISAMPLE_NONE;				// 多重采样的类型
	m_D3DPresentParam.MultiSampleQuality         = 0;								// 多重采样的质量
	m_D3DPresentParam.SwapEffect                 = D3DSWAPEFFECT_DISCARD;			// 后台缓冲区的页面置换方式
	m_D3DPresentParam.hDeviceWindow              = hWnd;							// 窗口句柄
	m_D3DPresentParam.Windowed                   = TRUE;							// 窗口还是全屏
	m_D3DPresentParam.EnableAutoDepthStencil     = TRUE;							// 深度与模板缓存
	m_D3DPresentParam.AutoDepthStencilFormat     = D3DFMT_D24S8;					// 深度缓存与模板缓存的像素格式
	m_D3DPresentParam.Flags                      = 0;								// 附加的特性
	m_D3DPresentParam.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;			// 刷新频率
	m_D3DPresentParam.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;	// 刷新时间间隔(垂直同步)
}

void CDirect3D::DrawText(const CHAR *strText, int iCount, LPRECT rect, DWORD Format, D3DCOLOR color)
{
	if (!m_pFont || !strText) return;
	m_pFont->DrawTextA(m_pSprite, strText, iCount, rect?rect:&m_rWnd, Format, color);
}

void CDirect3D::SpriteDraw(IDirect3DTexture9* pTexture, const RECT *pSrcRect, const D3DXVECTOR3 *pCenter, const D3DXVECTOR3 *pPosition, D3DCOLOR Color) 
{
	if (!m_pSprite || !pTexture) return;
	m_pSprite->Draw(pTexture, pSrcRect, pCenter, pPosition, Color);
}

void CDirect3D::SpriteSetTransform(const D3DXMATRIX* pTransform)
{
	if (!m_pSprite || !pTransform) return;
	m_pSprite->SetTransform(pTransform);
}

void CDirect3D::InitDefaultRenderState()
{
	// 默认关闭灯光
	D3DLightEnable(FALSE);

	// 设置着色模式为Gouraud
	SetShaderMode(D3DSHADE_GOURAUD);

	// 设置采样模式
	SetDefaultSamplerState();

	// 归一化变换后法向量
	m_pD3D9Device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// 设置环境光
	SetLightEnvironment(0xFF808080);
}

void CDirect3D::SetDefaultSamplerState()
{
	if (NULL == m_pD3D9Device)
		return;

	//for (int i = 0; i < MAX_TEXTURE_STAGE; ++i)
	//{
	//	m_pD3D9Device->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	//	m_pD3D9Device->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	//	m_pD3D9Device->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
	//	m_pD3D9Device->SetSamplerState(i, D3DSAMP_MAXANISOTROPY, 8);
	//}

	for (int i = 0; i < MAX_TEXTURE_STAGE; ++i)
	{
		m_pD3D9Device->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		m_pD3D9Device->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_pD3D9Device->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	}

	SetTextureAddressMode(D3DTADDRESS_WRAP);
}

void CDirect3D::ResetDevice(bool bForceReset)
{
	if (NULL == m_pD3D9Device)
		return;

	HRESULT hr = m_pD3D9Device->TestCooperativeLevel();
	while (D3D_OK != hr || bForceReset)
	{
		bForceReset = false;
		while (D3DERR_DEVICENOTRESET != hr && D3D_OK != hr)
		{
			Sleep(1000);
			hr = m_pD3D9Device->TestCooperativeLevel();
		}

		hr = m_pD3D9Device->Reset(&m_D3DPresentParam);
	}

	InitDefaultRenderState();
}

void CDirect3D::ResetBackBufferSize(int iWindowWidth, int iWindowHeight)
{
	m_D3DPresentParam.BackBufferWidth = iWindowWidth;
	m_D3DPresentParam.BackBufferHeight = iWindowHeight;
	ResetDevice(true);
}

void CDirect3D::PreRender(DWORD flag)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->Clear(0, NULL, flag, 0xff000000, 1.0f, 0);
	m_pD3D9Device->BeginScene();
	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
}

HRESULT CDirect3D::PostRender()
{
	if (!m_pD3D9Device || !m_pSprite)
		return E_FAIL;
	
	m_pSprite->End();
	m_pD3D9Device->EndScene();
	return m_pD3D9Device->Present(NULL, NULL, NULL, NULL);
}

//-----------------------------------------------------------------------------

void CDirect3D::SetWireframeRenderMode()
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
}

void CDirect3D::SetSolidRenderMode()
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

DWORD CDirect3D::GetRenderMode()
{
	if (NULL == m_pD3D9Device)
		return 0;

	DWORD dRenderMode;
	m_pD3D9Device->GetRenderState(D3DRS_FILLMODE, &dRenderMode);
	return dRenderMode;
}

void CDirect3D::SetShaderMode(DWORD mode)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetRenderState(D3DRS_SHADEMODE, mode);
}

void CDirect3D::SetCullMode(DWORD mode)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetRenderState(D3DRS_CULLMODE, mode);
}

DWORD CDirect3D::GetCullMode()
{
	if (NULL == m_pD3D9Device)
		return 0;

	DWORD dCullMode;
	m_pD3D9Device->GetRenderState(D3DRS_CULLMODE, &dCullMode);
	return dCullMode;
}

void CDirect3D::DepthEnable(BOOL bEnable)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetRenderState(D3DRS_ZENABLE, bEnable);
}

void CDirect3D::DepthWriteEnable(BOOL bEnable)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetRenderState(D3DRS_ZWRITEENABLE, bEnable);
}

void CDirect3D::AlphaBlendEnable(BOOL bEnable)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetRenderState(D3DRS_ALPHABLENDENABLE, bEnable);
}

void CDirect3D::SetSrcRenderBlendFactor(DWORD dFactor)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetRenderState(D3DRS_SRCBLEND, dFactor);
}

void CDirect3D::SetDestRenderBlendFactor(DWORD dFactor)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetRenderState(D3DRS_DESTBLEND, dFactor);
}

void CDirect3D::AlphaTestEnable(BOOL bEnable)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetRenderState(D3DRS_ALPHATESTENABLE, bEnable);
}

void CDirect3D::AlphaFunction(DWORD dFactor)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetRenderState(D3DRS_ALPHAFUNC, dFactor);
}

void CDirect3D::AlphaReference(DWORD dFactor)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetRenderState(D3DRS_ALPHAREF, dFactor);
}

//-----------------------------------------------------------------------------

void CDirect3D::StencilEnable(BOOL bEnable)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetRenderState(D3DRS_STENCILENABLE, bEnable);
}

void CDirect3D::SetStencilReference(DWORD dValue)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetRenderState(D3DRS_STENCILREF, dValue);
}

void CDirect3D::SetStencilMask(DWORD dMask)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetRenderState(D3DRS_STENCILMASK, dMask);
}

void CDirect3D::SetStencilWriteMask(DWORD dMask)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetRenderState(D3DRS_STENCILWRITEMASK, dMask);
}

void CDirect3D::SetStencilFunc(DWORD dFunc)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetRenderState(D3DRS_STENCILFUNC, dFunc);
}

void CDirect3D::SetStencilPass(DWORD dOperation)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetRenderState(D3DRS_STENCILPASS, dOperation);
}

void CDirect3D::SetStencilFail(DWORD dOperation)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetRenderState(D3DRS_STENCILFAIL, dOperation);
}

void CDirect3D::SetStencilZFail(DWORD dOperation)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetRenderState(D3DRS_STENCILZFAIL, dOperation);
}

void CDirect3D::EnableFog(BOOL bEnable)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetRenderState(D3DRS_FOGENABLE, bEnable);
}

void CDirect3D::SetLinearFog(float fStart, float fEnd, DWORD color)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
	m_pD3D9Device->SetRenderState(D3DRS_FOGSTART, *(DWORD*)(&fStart));
	m_pD3D9Device->SetRenderState(D3DRS_FOGEND, *(DWORD*)(&fEnd));
	m_pD3D9Device->SetRenderState(D3DRS_FOGCOLOR, color);
}

void CDirect3D::SetExponentialFog(float fDensity, DWORD color)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_EXP);
	m_pD3D9Device->SetRenderState(D3DRS_FOGDENSITY, *(DWORD*)(&fDensity));
	m_pD3D9Device->SetRenderState(D3DRS_FOGCOLOR, color);
}

//-----------------------------------------------------------------------------

void CDirect3D::SetD3DFVF(DWORD FVF)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetFVF(FVF);
}

HRESULT CDirect3D::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle)
{
	if (NULL == m_pD3D9Device)
		return D3DERR_INVALIDDEVICE;

	return m_pD3D9Device->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);
}

HRESULT CDirect3D::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle)
{
	if (NULL == m_pD3D9Device)
		return D3DERR_INVALIDDEVICE;

	return m_pD3D9Device->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);
}

void CDirect3D::SetStreamSource(UINT uiStreamNum, IDirect3DVertexBuffer9* pVertexBuffer, UINT uiOffset, UINT uiStride)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetStreamSource(uiStreamNum, pVertexBuffer, uiOffset, uiStride);
}

void CDirect3D::SetIndices(IDirect3DIndexBuffer9* pIndexBuffer)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetIndices(pIndexBuffer);
}

void CDirect3D::DrawPrimitive(D3DPRIMITIVETYPE type, UINT uiStartVertex, UINT uiDrawCount)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->DrawPrimitive(type, uiStartVertex, uiDrawCount);
}

void CDirect3D::DrawIndexedPrimitive(D3DPRIMITIVETYPE type, UINT uiBaseVertexIndex, UINT uiMinVertexIndex, UINT uiVertexCount, UINT uiStartIndex, UINT uiDrawCount)
{
	if (NULL == m_pD3D9Device || !uiDrawCount)
		return;

	m_pD3D9Device->DrawIndexedPrimitive(type, uiBaseVertexIndex, uiMinVertexIndex, uiVertexCount, uiStartIndex, uiDrawCount);
}

//-----------------------------------------------------------------------------

void CDirect3D::SetTransform(D3DTRANSFORMSTATETYPE type, const D3DXMATRIX* mat)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetTransform(type, mat);
}

void CDirect3D::GetTransform(D3DTRANSFORMSTATETYPE type, D3DXMATRIX* mat)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->GetTransform(type, mat);
}

//-----------------------------------------------------------------------------

void CDirect3D::D3DLightEnable(BOOL bEnable)
{
	if (NULL == m_pD3D9Device)
		return;

	if (GetD3DLightEnable() == bEnable)
		return;

	m_pD3D9Device->SetRenderState(D3DRS_LIGHTING, bEnable);
	m_pD3D9Device->SetRenderState(D3DRS_SPECULARENABLE, bEnable);
}

BOOL CDirect3D::GetD3DLightEnable()
{
	if (NULL == m_pD3D9Device)
		return FALSE;

	DWORD bEnable;
	m_pD3D9Device->GetRenderState(D3DRS_LIGHTING, &bEnable);
	return bEnable;
}

void CDirect3D::SetLightEnvironment(DWORD LEColor)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetRenderState(D3DRS_AMBIENT, LEColor);
}

void CDirect3D::SetLight(int iIndex, const D3DLIGHT9* light)
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->SetLight(iIndex, light);
}

void CDirect3D::LightEnable(int iIndex, BOOL bEnable)
{
	if (NULL == m_pD3D9Device)
		return;

	if (GetLightEnable(iIndex) == bEnable)
		return;

	m_pD3D9Device->LightEnable(iIndex, bEnable);
}

BOOL CDirect3D::GetLightEnable(int iIndex)
{
	if (NULL == m_pD3D9Device)
		return FALSE;

	BOOL bEnable;
	m_pD3D9Device->GetLightEnable(iIndex, &bEnable);
	return bEnable;
}

//-----------------------------------------------------------------------------

void CDirect3D::SetMaterial(const D3DMATERIAL9* pMatrerial)
{
	if (NULL == m_pD3D9Device || NULL == pMatrerial)
		return;

	m_pD3D9Device->SetMaterial(pMatrerial);
}

//-----------------------------------------------------------------------------

void CDirect3D::SetTexture(DWORD Stage, LPDIRECT3DTEXTURE9 pTexture)
{
	if (NULL == m_pD3D9Device)
		return;

	if (0 > Stage || MAX_TEXTURE_STAGE <= Stage)
		return;

	m_pD3D9Device->SetTexture(Stage, pTexture);
}

void CDirect3D::SetTextureAddressMode(D3DTEXTUREADDRESS eMode)
{
	if (NULL == m_pD3D9Device)
		return;

	for (int i = 0; i < MAX_TEXTURE_STAGE; ++i)
	{
		m_pD3D9Device->SetSamplerState(i, D3DSAMP_ADDRESSU, eMode);
		m_pD3D9Device->SetSamplerState(i, D3DSAMP_ADDRESSV, eMode);
	}
}

void CDirect3D::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE dTexStageStateType, DWORD dValue)
{
	if (NULL == m_pD3D9Device)
		return;

	if (0 > Stage || MAX_TEXTURE_STAGE <= Stage)
		return;

	m_pD3D9Device->SetTextureStageState(Stage, dTexStageStateType, dValue);
}

void CDirect3D::SetTextureColorOpStageState(DWORD Stage, DWORD dValue)
{
	SetTextureStageState(Stage, D3DTSS_COLOROP, dValue);
}

void CDirect3D::SetTextureColorArg1StageState(DWORD Stage, DWORD dValue)
{
	SetTextureStageState(Stage, D3DTSS_COLORARG1, dValue);
}

void CDirect3D::SetTextureColorArg2StageState(DWORD Stage, DWORD dValue)
{
	SetTextureStageState(Stage, D3DTSS_COLORARG2, dValue);
}

void CDirect3D::SetTextureAlphaOpStageState(DWORD Stage, DWORD dValue)
{
	SetTextureStageState(Stage, D3DTSS_ALPHAOP, dValue);
}

void CDirect3D::SetTextureAlphaArg1StageState(DWORD Stage, DWORD dValue)
{
	SetTextureStageState(Stage, D3DTSS_ALPHAARG1, dValue);
}

void CDirect3D::SetTextureAlphaArg2StageState(DWORD Stage, DWORD dValue)
{
	SetTextureStageState(Stage, D3DTSS_ALPHAARG2, dValue);
}

//-----------------------------------------------------------------------------

HRESULT CDirect3D::CompileEffectFromFile(char* strEffectFile, ID3DXEffect** ppEffect)
{
	if (NULL == m_pD3D9Device || NULL == ppEffect)
		return E_FAIL;

	ID3DXBuffer* pEffectError = NULL;
	HRESULT hr = D3DXCreateEffectFromFileA(m_pD3D9Device, strEffectFile, NULL, NULL, 0, 0, ppEffect, &pEffectError);
	if (NULL != pEffectError)
		::MessageBoxA(0, (char*)pEffectError->GetBufferPointer(), "Error", MB_OK);

	Safe_Release(pEffectError);
	return hr;
}