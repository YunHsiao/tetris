#include "Utility.h"
#include "GUI.h"
#include "Input.h"
#include "SceneManager.h"

#define BUTTON_WIDTH 120
#define BUTTON_HEIGHT 50

const CInput* CGUI::s_input = 0;
const POINTS* CGUI::s_pCur = 0;
const POINTS* CGUI::s_pDown = 0;
const POINTS* CGUI::s_pUp = 0;
unsigned CGUI::s_pButton[3] = { 0, 0, 0 };

void CGUI::onInit()
{
	s_input = CInput::getInstance();
	s_pCur = s_input->GetMouseCurrentPos();
	s_pDown = s_input->GetMouseDownPos();
	s_pUp = s_input->GetMouseUpPos();
	s_pButton[0] = CSceneManager::getRenderer()->CreateTexture("button0.png");
	s_pButton[1] = CSceneManager::getRenderer()->CreateTexture("button1.png");
	s_pButton[2] = CSceneManager::getRenderer()->CreateTexture("button2.png");
}

bool CGUI::Button(const char* str, short x, short y, unsigned hotkey)
{
	unsigned state = 0;
	SVector p = { (float) x, (float) y, 0.f };
	SRect rect = {x, y, x + BUTTON_WIDTH, y + BUTTON_HEIGHT};
	bool bEvent = false;
	bool bCur = InRect(&rect, s_pCur);
	bool bDown = InRect(&rect, s_pDown);
	bool bUp = InRect(&rect, s_pUp);
	if (bCur) {
		state = 1;
		if (s_input->GetKeyState(VK_LBUTTON) && bDown) {
			state = 2;
		}
	}
	if (s_input->GetKeyState(VK_RBUTTON)) {
		if (bDown && bUp)
			bEvent = true;
	}
	if (s_input->GetKeyState(hotkey))
		bEvent = true;

	CSceneManager::getRenderer()->SpriteDraw(s_pButton[state], &p);
	CSceneManager::getRenderer()->SpriteDrawText(str, &rect, DT_CENTER | DT_VCENTER);
	return bEvent;
}

bool CGUI::InRect(const SRect* rect, const POINTS* p)
{
	return (p->x >= rect->left) && (p->x <= rect->right)
		&& (p->y >= rect->top) && (p->y <= rect->bottom);
}
