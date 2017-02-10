#ifndef CInput_H_
#define CInput_H_

class CInput
{
public:
	static CInput* getInstance() { return &s_input; }

	CInput();
	~CInput();

	void onInit();
	void WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void PostRender() { memset(m_keys, 0, sizeof(m_keys)); }

	const POINTS* GetMouseDownPos() const { return &m_pDown; }
	const POINTS* GetMouseUpPos() const { return &m_pUp; }
	const POINTS* GetMouseCurrentPos() const { return &m_pCur; }
	void ResetKeyInterval(unsigned key) { m_last[key] = 0; m_this[key] = m_this[0]; }
	unsigned long GetKeyInterval(unsigned key) const { return m_this[key] - m_last[key]; }
	void ResetKeyState(unsigned key) { m_keys[key] = false; }
	bool GetKeyState(unsigned key) const { return m_keys[key]; }
	bool PopKeyState(unsigned key) { bool s = m_keys[key]; m_keys[key] = false; return s; } 
private:
	POINTS m_pDown, m_pUp, m_pCur;
	bool m_keys[256];
	unsigned long m_last[256], m_this[256];
	static CInput s_input;
};
#endif
