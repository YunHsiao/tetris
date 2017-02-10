#ifndef CSceneManager_H_
#define CSceneManager_H_
#include "Scene.h"

class CSceneManager
{
public:
	static CSceneManager* getInstance() { return &s_director; }
	CSceneManager();
	~CSceneManager();

	void Go();

	void onInit();
	void onTick(int iElapsedTime);
	void onRender();
	void WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	static CSceneManager s_director;
	CScene m_scene;
	std::string m_FPS;
};
#endif
