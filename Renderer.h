#ifndef CRenderer_H_
#define CRenderer_H_

#include "Direct3D.h"

enum EAlignFormat {
	EAF_LEFT,
	EAF_CENTER,
	EAF_RIGHT
};

class CRenderer
{
	CRenderer();
	~CRenderer();
public:
	static CRenderer* getInstance() { return &s_renderer; }
	bool onInit();
	void PreRender();
	void PostRender();

	// ÆÁÄ»ÎÄ×Ö¡¢Í¼Æ¬
	void DrawText(const char* strText, SRect* rect = 0, int Format = EAF_LEFT, unsigned long color = 0xffffffff);
	void SpriteDraw(unsigned int pTexture, const SVector* pPosition = 0, unsigned long Color = 0xffffffff);
	unsigned int CreateTexture(const char* pSrcFile);

private:
	CDirect3D m_direct3d;
	static CRenderer s_renderer;
};
#endif