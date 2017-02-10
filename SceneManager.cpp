#include "Utility.h"
#include "SceneManager.h"
#include "Window.h"
#include "Direct3D.h"
#include "GDI.h"

CSceneManager CSceneManager::s_director;
CRenderer* CSceneManager::s_pRenderer = NULL;
std::string CSceneManager::s_FPS;
std::string CSceneManager::s_renderer;
CScene CSceneManager::s_scene;

CSceneManager::CSceneManager()
{
}

CSceneManager::~CSceneManager()
{
	Safe_Delete(s_pRenderer);
}

void CSceneManager::Go() {
	onInit();
	MSG msg;
	DWORD dwCurrent(0), dwDuration(0);
	ZeroMemory(&msg, sizeof(msg));
	unsigned long last = timeGetTime(), lasts = last, current = 0;
	int cnt = 0, elapsed = 0;
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			current = timeGetTime();
			elapsed = current - last;
			last = current;
			if (current - lasts >= 1000) lasts = current, s_FPS = toString(cnt), cnt = 0;
			else cnt++;
			onTick( elapsed );
			onRender();
			Sleep(10);
		}
	}
}

void CSceneManager::onInit() {
	CWindow::getInstance()->setWinSize(640, 640);
	CWindow::getInstance()->onInit();
	if ((GetAsyncKeyState(VK_CONTROL) & 0x8000))
		s_pRenderer = new CGDI(), s_renderer = "GDI";
	else s_pRenderer = new CDirect3D(), s_renderer = "Direct3D";
	s_pRenderer->onInit();
	s_scene.onInit();
	CWindow::getInstance()->showWindow();
}

void CSceneManager::onTick(int iElapsedTime) {
	s_scene.onTick(iElapsedTime);
}

void CSceneManager::onRender() {
	s_pRenderer->PreRender();
	s_scene.onRender();
	s_pRenderer->SpriteDrawText(s_FPS.c_str(), 0, DT_RIGHT);
	s_pRenderer->SpriteDrawText(s_renderer.c_str(), 0, DT_LEFT);
	s_pRenderer->PostRender();
}

void CSceneManager::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	s_scene.WndProc(hWnd, uMsg, wParam, lParam);
}
