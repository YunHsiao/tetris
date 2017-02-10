#ifndef CInput_H_
#define CInput_H_

#define KS_LBUTTON_CLICK		VK_OEM_RESET
#define KS_LBUTTON_DOUBLE_CLICK	VK_OEM_JUMP
#define KS_WHEEL_UP				VK_OEM_PA1
#define KS_WHEEL_DOWN			VK_OEM_PA2

class CInput
{
public:
	static CInput* getInstance() { return &s_input; }

	CInput();
	~CInput();

	void onInit();
	void WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void PostRender() { for (int i(0); i < 256; i++) m_keys[i] = m_keys[i] && m_mask[i]; }

	const POINTS* GetMouseDownPos() const { return &m_pDown; }
	const POINTS* GetMouseUpPos() const { return &m_pUp; }
	const POINTS* GetMouseCurrentPos() const { return &m_pCur; }

	void ResetKeyInterval(unsigned key) { m_last[key] = 0; m_this[key] = m_this[0]; }
	unsigned long GetKeyInterval(unsigned key) const { return m_this[key] - m_last[key]; }

	// 几个特殊用法：
	// KS_LBUTTON_CLICK			左键单击
	// KS_LBUTTON_DOUBLE_CLICK	左键双击
	// KS_WHEEL_UP				滚轮向上
	// KS_WHEEL_DOWN			滚轮向下
	bool GetKeyState(unsigned key) const { return m_keys[key]; }
	void KeepKeyState(unsigned key, bool keep) { m_mask[key] = keep; }
	// OBSOLETE
	//void ResetKeyState(unsigned key) { m_keys[key] = false; }
	//bool PopKeyState(unsigned key) { bool s = m_keys[key]; m_keys[key] = false; return s; } 
private:
	POINTS m_pDown, m_pUp, m_pCur;
	bool m_keys[256], m_mask[256];
	unsigned long m_last[256], m_this[256];
	static CInput s_input;
};
#endif
