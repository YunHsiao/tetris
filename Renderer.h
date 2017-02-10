#ifndef CRenderer_H_
#define CRenderer_H_

class CRenderer
{
public:
	CRenderer();
	virtual ~CRenderer();
	virtual bool onInit() = 0;
	virtual void PreRender() = 0;
	virtual void PostRender() = 0;
	
	virtual void SpriteDrawText(const char* strText, SRect* rect = 0, int Format = DT_LEFT, 
		unsigned long color = 0xffffffff) = 0;
	virtual void SpriteDraw(unsigned int pTexture, const SVector* pPosition = 0, 
		unsigned long color = 0xffffffff) = 0;
	virtual unsigned int CreateTexture(const char* pSrcFile) = 0;
protected:
	RECT m_rWnd;
};
#endif