#include "Utility.h"
#include "Direct3D.h"

CDirect3D::CDirect3D()
	:m_pDirect3D9(0)
	,m_pD3D9Device(0)
	,m_pSprite(0)
	,m_pFont(0)
	,m_bD3DFont(false)
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
	RemoveFontResource(TEXT(TRS_FONT_FILE));
	m_vTextures.clear();
	m_vSize.clear();
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
	D3DPRESENT_PARAMETERS D3DPresentParam;
	ZeroMemory(&D3DPresentParam, sizeof(D3DPRESENT_PARAMETERS));

	m_rWnd.right = lWindowWidth;
	m_rWnd.bottom = lWindowHeight;

	D3DDISPLAYMODE d3ddm;
	m_pDirect3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

	D3DPresentParam.BackBufferWidth            = lWindowWidth;					// 后台缓冲区的宽度
	D3DPresentParam.BackBufferHeight           = lWindowHeight;					// 后台缓冲区的高度
	D3DPresentParam.BackBufferFormat           = d3ddm.Format;					// 后台缓冲区的像素格式
	D3DPresentParam.BackBufferCount            = 1;								// 后台缓冲区的数量
	D3DPresentParam.MultiSampleType            = D3DMULTISAMPLE_NONE;			// 多重采样的类型
	D3DPresentParam.MultiSampleQuality         = 0;								// 多重采样的质量
	D3DPresentParam.SwapEffect                 = D3DSWAPEFFECT_DISCARD;			// 后台缓冲区的页面置换方式
	D3DPresentParam.hDeviceWindow              = hWnd;							// 窗口句柄
	D3DPresentParam.Windowed                   = TRUE;							// 窗口还是全屏
	D3DPresentParam.EnableAutoDepthStencil     = TRUE;							// 深度与模板缓存
	D3DPresentParam.AutoDepthStencilFormat     = D3DFMT_D24S8;					// 深度缓存与模板缓存的像素格式
	D3DPresentParam.Flags                      = 0;								// 附加的特性
	D3DPresentParam.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;		// 刷新频率
	D3DPresentParam.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;	// 刷新时间间隔(垂直同步)

	hr = m_pDirect3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, vp, &D3DPresentParam, &m_pD3D9Device);
	if (FAILED(hr))
		return false;

	AddFontResource(TEXT(TRS_FONT_FILE));
	if (FAILED(hr = D3DXCreateFont(m_pD3D9Device, 33, 0, FW_DONTCARE, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT(TRS_FONT_NAME), &m_pFont)))
			return false;
	if (FAILED(hr = D3DXCreateSprite(m_pD3D9Device, &m_pSprite))) 
		return false;

	FT_Face face;
	FT_Library library;
	if (FT_Init_FreeType(&library)) 
		return false;
	if (FT_New_Face(library, TRS_FONT_FILE, 0, &face)) 
		return false;
	FT_Set_Char_Size(face, FONT_SIZE << 6, FONT_SIZE << 6, 96, 96);

	for (unsigned char i = 0; i < 128; i++) {
		if (FT_Load_Glyph(face, FT_Get_Char_Index(face, i), FT_LOAD_DEFAULT))
			return false;
		FT_Glyph glyph;
		if (FT_Get_Glyph(face->glyph, &glyph))
			return false;

		FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1);
		FT_Bitmap& bitmap = ((FT_BitmapGlyph)glyph)->bitmap;

		m_xoff[i] = (float) (face->glyph->advance.x >> 6);
		CreateTexture(bitmap.buffer, bitmap.width, bitmap.rows);
		FT_Done_Glyph(glyph);
	}

	FT_Done_Face(face);
	FT_Done_FreeType(library);

	return true;
}

void CDirect3D::SpriteDrawText(const char* strText, SRect* rect, int Format, unsigned long color)
{
	if (GetAsyncKeyState(VK_F7) & 0x8000) m_bD3DFont = true;
	if (GetAsyncKeyState(VK_F8) & 0x8000) m_bD3DFont = false;
	if (!m_pFont || !strText) return;
	if (m_bD3DFont) {
		m_pFont->DrawTextA(m_pSprite, strText, -1, 
			rect?reinterpret_cast<RECT*>(rect):&m_rWnd, Format, color);
		return;
	}

	std::vector<float> len;
	CountLines(strText, len);

	SVector p = { 0.f, 0.f, 0.f };
	RECT* rct = rect ? reinterpret_cast<RECT*>(rect) : &m_rWnd;
	for (unsigned line = 0; line < len.size(); line++, strText++) {
		TextPosition(&p, len.data(), len.size(), rct, Format, line);
		while (*strText && *strText != '\n') {
			size_t index = *strText;
			p.y -= m_vSize[index].y / 2 + 10;
			if (index != ' ') SpriteDraw(index, &p, color);
			p.y += m_vSize[index].y / 2 + 10;
			p.x += m_xoff[index];
			strText++;
		}
	}
}

void CDirect3D::SpriteDraw(size_t pTexture, const SVector* pPosition, unsigned long color)
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
	m_pD3D9Device->Present(0, 0, 0, 0);
}

size_t CDirect3D::CreateTexture(const char* pSrcFile) 
{
	if (!m_pD3D9Device)
		return -1;

	IDirect3DTexture9* pTex;
	if (FAILED(D3DXCreateTextureFromFileExA(m_pD3D9Device,
		pSrcFile, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2,
		1, 0, D3DFMT_UNKNOWN,D3DPOOL_MANAGED,
		D3DX_FILTER_LINEAR, D3DX_FILTER_NONE, 0, 0, 0,
		&pTex))) 
		return -1;

	D3DSURFACE_DESC surfDesc;
	pTex->GetLevelDesc(0, &surfDesc);
	SPoint pt;
	pt.x = surfDesc.Width;
	pt.y = surfDesc.Height;
	m_vSize.push_back(pt);
	m_vTextures.push_back(pTex);
	return m_vTextures.size() - 1;
}

size_t CDirect3D::CreateTexture(unsigned char *image, unsigned w, unsigned h) 
{
	if (!m_pD3D9Device)
		return -1;

	IDirect3DTexture9* pTex;
	D3DXCreateTexture(m_pD3D9Device, w, h, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, &pTex);
	D3DLOCKED_RECT pValue; 
	unsigned int* pColor;
	pTex->LockRect(0, &pValue, 0, D3DLOCK_DISCARD);
	pColor = (unsigned int*) pValue.pBits;
	unsigned t;
	for(unsigned i = 0; i < h; i++) {
		for(unsigned j = 0; j < w; j++) {
			t = i * w + j;
			pColor[t] = (image[t] << 16) + (image[t] << 8) + image[t];
			pColor[t] += (image[t] << 24);
			//pColor[t] += image[t] ? 0xff000000 : 0;
		}
	}
	pTex->UnlockRect(0);
	SPoint pt;
	pt.x = w;
	pt.y = h;
	m_vSize.push_back(pt);
	m_vTextures.push_back(pTex);
	return m_vTextures.size() - 1;
}
