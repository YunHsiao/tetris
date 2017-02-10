#ifndef CScene_H_
#define CScene_H_

class CScene
{
public:
	void onInit();
	void onTick(int fElapsedTime);
	void onRender();
	void WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:

};
#endif
