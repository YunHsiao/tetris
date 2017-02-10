#include "Utility.h"
#include "Window.h"
#include "Input.h"

CWindow CWindow::s_wnd;

CWindow::CWindow()
:m_lWidth(640)
,m_lHeight(480)
{
	m_hWnd = 0;
	m_hInstance = 0;
	ZeroMemory(&m_wc, sizeof(m_wc));
	SetRectEmpty(&m_winRect);
}
CWindow::~CWindow()
{
	UnregisterClass(m_wc.lpszClassName, m_wc.hInstance);
}
void CWindow::onInit()
{
	m_wc.cbClsExtra = 0;
	m_wc.cbWndExtra = 0;
	m_wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	m_wc.hCursor = LoadCursor(0, IDC_ARROW);
	m_wc.hIcon = (HICON) LoadImage(NULL, TEXT("res\\tetris.ico"), IMAGE_ICON, 
		0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED);
	m_wc.hInstance = (HINSTANCE)GetModuleHandle(NULL);
	m_wc.lpfnWndProc = WndProc;
	m_wc.lpszClassName = TEXT("test");
	m_wc.lpszMenuName = NULL;
	m_wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;

	RegisterClass(&m_wc);

	m_hWnd = CreateWindow(m_wc.lpszClassName, TEXT("Tetris"), WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, m_hInstance, NULL);

	RECT rectWindow;
	RECT rectClient;
	GetWindowRect(m_hWnd, &rectWindow);
	GetClientRect(m_hWnd, &rectClient);

	int iWidth = (rectWindow.right - rectWindow.left) - (rectClient.right - rectClient.left) + m_lWidth;
	int iHeight = (rectWindow.bottom - rectWindow.top) - (rectClient.bottom - rectClient.top) + m_lHeight;
	int iPosX = (::GetSystemMetrics(SM_CXSCREEN) - iWidth) / 2;
	int iPosY = (::GetSystemMetrics(SM_CYSCREEN) -  iHeight) / 2;
	m_winRect.right = m_lWidth;
	m_winRect.bottom = m_lHeight;
	MoveWindow(m_hWnd, iPosX, iPosY, iWidth, iHeight, FALSE);

	if (m_hWnd) m_hInstance = m_wc.hInstance;
	else MessageBox(0, TEXT("CreateWindow Error"), TEXT(" "), 0);
}

void CWindow::showWindow() {
	ShowWindow(m_hWnd, SW_SHOWNORMAL);
	UpdateWindow(m_hWnd);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CInput::getInstance()->WndProc(hWnd, uMsg, wParam, lParam);

	switch (uMsg)
	{
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
