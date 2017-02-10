#include "Utility.h"
#include "Input.h"
#include "SceneManager.h"

CInput CInput::s_input;

CInput::CInput()
{

}

CInput::~CInput()
{

}
void CInput::onInit()
{
	m_pDown.x = m_pDown.y = 0;
	m_pUp.x = m_pUp.y = 0;
	m_pCur.x = m_pCur.y = 0;
	memset(m_keys, 0, sizeof(m_keys));
	memset(m_mask, 0, sizeof(m_mask));
	memset(m_last, 0, sizeof(m_last));
	memset(m_this, timeGetTime(), sizeof(m_this));
	KeepKeyState(VK_LBUTTON, true);
}

void CInput::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int off = 0;
	switch (uMsg) {
	case WM_KEYDOWN: 
		m_keys[wParam] = true;
		break;
	case WM_KEYUP: 
		m_keys[wParam] = false;
		m_last[wParam] = m_this[wParam];
		m_this[wParam] = timeGetTime();
		break;
	case WM_MOUSEMOVE:
		m_pCur = MAKEPOINTS(lParam);
		break;
	case WM_LBUTTONDOWN:
		m_pDown = MAKEPOINTS(lParam);
		m_keys[VK_LBUTTON] = true;
		break;
	case WM_LBUTTONUP:
		m_pUp = MAKEPOINTS(lParam);
		m_keys[VK_LBUTTON] = false;
		m_keys[KS_LBUTTON_CLICK] = true;
		break;
	case WM_LBUTTONDBLCLK:
		m_keys[KS_LBUTTON_DOUBLE_CLICK] = true;
		break;
	case WM_MOUSEWHEEL:
		off = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
		if (off > 0) m_keys[KS_WHEEL_UP] = true;
		else if (off < 0) m_keys[KS_WHEEL_DOWN] = true;
		break;
	case WM_KILLFOCUS: 
		CSceneManager::getInstance()->OnKillFocus();
		break;
	case WM_SETFOCUS: 
		CSceneManager::getInstance()->OnSetFocus();
		break;
	}
}
