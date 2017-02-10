#include "Utility.h"
#include "SceneManager.h"
#include "Window.h"
#include "Direct3D.h"

CSceneManager CSceneManager::s_director;

CSceneManager::CSceneManager()
{
}

CSceneManager::~CSceneManager()
{
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
			if (current - lasts >= 1000) lasts = current, m_FPS = toString(cnt), cnt = 0;
			else cnt++;
			onTick( elapsed );
			onRender();
		}
	}
}

void CSceneManager::onInit() {
	CWindow::getInstance()->setWinSize(640, 640); //设置窗口的大小
	CWindow::getInstance()->onInit();
	CDirect3D::getInstance()->onInit();
	m_scene.onInit();
	CWindow::getInstance()->showWindow();
}

void CSceneManager::onTick(int iElapsedTime) {
	m_scene.onTick(iElapsedTime);
}

void CSceneManager::onRender() {
	CDirect3D::getInstance()->PreRender(D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL);
	CDirect3D::getInstance()->DrawText(m_FPS.c_str(), -1, 0, DT_RIGHT);
	m_scene.onRender();
	CDirect3D::getInstance()->PostRender();
}

void CSceneManager::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	
}
