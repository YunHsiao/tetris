#ifndef CInput_H_
#define CInput_H_

class CInput
{
public:
	static CInput* getInstance() { return &s_input; }

	CInput();
	~CInput();

	void onInit();
	void onDestroy();
	void WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:

	static CInput s_input;
};
#endif
