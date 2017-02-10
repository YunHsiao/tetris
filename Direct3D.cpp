#include "Utility.h"
#include "Direct3D.h"
#include "Window.h"

CDirect3D::CDirect3D():
m_pDirect3D9(NULL),
m_pD3D9Device(NULL),
m_pSprite(NULL),
m_pFont(NULL)
{
}

CDirect3D::~CDirect3D()
{
	Safe_Release(m_pSprite);
	Safe_Release(m_pFont);
	Safe_Release(m_pD3D9Device);
	Safe_Release(m_pDirect3D9);
	for (auto it(m_vTextures.begin()); it != m_vTextures.end(); it++)
		Safe_Release(*it);
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

	// D3D���
	D3DCAPS9 caps;
	m_pDirect3D9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

	// �жϵ�ǰ�Կ��Ƿ�֧��Ӳ�����㴦��
	int vp = 0;
	if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// ��ʼ����ز���
	D3DPRESENT_PARAMETERS D3DPresentParam;
	ZeroMemory(&D3DPresentParam, sizeof(D3DPRESENT_PARAMETERS));

	m_rWnd.right = lWindowWidth;
	m_rWnd.bottom = lWindowHeight;

	D3DDISPLAYMODE d3ddm;
	m_pDirect3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

	D3DPresentParam.BackBufferWidth            = lWindowWidth;					// ��̨�������Ŀ��
	D3DPresentParam.BackBufferHeight           = lWindowHeight;					// ��̨�������ĸ߶�
	D3DPresentParam.BackBufferFormat           = d3ddm.Format;					// ��̨�����������ظ�ʽ
	D3DPresentParam.BackBufferCount            = 1;								// ��̨������������
	D3DPresentParam.MultiSampleType            = D3DMULTISAMPLE_NONE;			// ���ز���������
	D3DPresentParam.MultiSampleQuality         = 0;								// ���ز���������
	D3DPresentParam.SwapEffect                 = D3DSWAPEFFECT_DISCARD;			// ��̨��������ҳ���û���ʽ
	D3DPresentParam.hDeviceWindow              = hWnd;							// ���ھ��
	D3DPresentParam.Windowed                   = TRUE;							// ���ڻ���ȫ��
	D3DPresentParam.EnableAutoDepthStencil     = TRUE;							// �����ģ�建��
	D3DPresentParam.AutoDepthStencilFormat     = D3DFMT_D24S8;					// ��Ȼ�����ģ�建������ظ�ʽ
	D3DPresentParam.Flags                      = 0;								// ���ӵ�����
	D3DPresentParam.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;		// ˢ��Ƶ��
	D3DPresentParam.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;	// ˢ��ʱ����(��ֱͬ��)

	hr = m_pDirect3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, vp, &D3DPresentParam, &m_pD3D9Device);
	if (FAILED(hr))
		return false;

	hr = D3DXCreateFont(m_pD3D9Device, 30, 0, FW_DONTCARE, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Kristen ITC"), &m_pFont);
	if (FAILED(hr)) {
		hr = D3DXCreateFont(m_pD3D9Device, 30, 0, FW_DONTCARE, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("����"), &m_pFont);
		if (FAILED(hr))
			return false;
	}
	if (FAILED(hr = D3DXCreateSprite(m_pD3D9Device, &m_pSprite))) 
		return false;

	return true;
}

void CDirect3D::InitPresentParam(HWND hWnd, long lWindowWidth, long lWindowHeight, D3DPRESENT_PARAMETERS* D3DPresentParam)
{
}

void CDirect3D::SpriteDrawText(const char* strText, SRect* rect, int Format, unsigned long color)
{
	if (!m_pFont || !strText) return;
	m_pFont->DrawTextA(m_pSprite, strText, -1, rect?reinterpret_cast<RECT*>(rect):&m_rWnd, Format, color);
}

void CDirect3D::SpriteDraw(unsigned int pTexture, const SVector* pPosition, unsigned long color) 
{
	if (!m_pSprite || m_vTextures.size() <= pTexture) return;
	m_pSprite->Draw(m_vTextures[pTexture], 0, 0, reinterpret_cast<const D3DXVECTOR3*>(pPosition), color);
}

void CDirect3D::PreRender()
{
	if (NULL == m_pD3D9Device)
		return;

	m_pD3D9Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 
		0xff000000, 1.0f, 0);
	m_pD3D9Device->BeginScene();
	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
}

void CDirect3D::PostRender()
{
	if (!m_pD3D9Device || !m_pSprite)
		return;
	
	m_pSprite->End();
	m_pD3D9Device->EndScene();
	m_pD3D9Device->Present(NULL, NULL, NULL, NULL);
}

unsigned int CDirect3D::CreateTexture(const char* pSrcFile) {
	if (!m_pD3D9Device)
		return -1;

	IDirect3DTexture9* pTex = NULL;
	if (FAILED(D3DXCreateTextureFromFileExA(m_pD3D9Device,
		pSrcFile,D3DX_DEFAULT_NONPOW2,D3DX_DEFAULT_NONPOW2,
		1,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,
		D3DX_FILTER_LINEAR,D3DX_FILTER_NONE,0,NULL,NULL,
		&pTex))) 
		return -1;
	m_vTextures.push_back(pTex);
	return m_vTextures.size() - 1;
}
