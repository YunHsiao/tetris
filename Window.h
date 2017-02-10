#ifndef CWindow_H_
#define CWindow_H_

class CWindow
{
public:
	static CWindow* getInstance() { return &s_wnd; }

	CWindow();
	~CWindow();

	HWND getHWND() const { return m_hWnd; }
	HINSTANCE getHinstance() const { return m_hInstance; }

	void setWinSize(long lWidth, long lHeight) { m_lWidth = lWidth; m_lHeight = lHeight; }
	long getWinWidth() const { return m_lWidth; }
	long getWinHeight() const { return m_lHeight; }

	RECT getWinRect() const { return m_winRect; }

	void onInit();
	void onDestroy();
	void showWindow();
	inline void HideCursor(bool bHide) {
		if (bHide) while (::ShowCursor(false) >= 0);
		else while (::ShowCursor(true) < 0);
	}
private:
	static CWindow s_wnd;
private:
	HWND m_hWnd;
	HINSTANCE m_hInstance;
	WNDCLASS m_wc;

	long m_lWidth, m_lHeight;
	RECT m_winRect;
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
