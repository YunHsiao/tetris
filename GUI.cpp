#include "Utility.h"
#include "GUI.h"
#include "Input.h"
#include "SceneManager.h"

#define BUTTON_WIDTH 120
#define BUTTON_HEIGHT 50
#define LIST_ITEM_HEIGHT 40

const CInput* CGUI::s_input = 0;
const POINTS* CGUI::s_pCur = 0;
const POINTS* CGUI::s_pDown = 0;
const POINTS* CGUI::s_pUp = 0;
size_t CGUI::s_pButton[3] = { 0, 0, 0 };

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
	bool bEvent = false;
	unsigned state = 0;
	SVector p = { (float) x, (float) y, 0.f };
	SRect rect = {x, y, x + BUTTON_WIDTH, y + BUTTON_HEIGHT};
	bool bCur = InRect(&rect, s_pCur);
	bool bDown = InRect(&rect, s_pDown);
	bool bUp = InRect(&rect, s_pUp);
	if (bCur) {
		state = 1;
		if (s_input->GetKeyState(VK_LBUTTON) && bDown) {
			state = 2;
		}
	}
	// ×ó¼üµ¥»÷
	if (s_input->GetKeyState(KS_LBUTTON_CLICK)) {
		if (bDown && bUp)
			bEvent = true;
	}
	if (s_input->GetKeyState(hotkey))
		bEvent = true;

	rect.top += 10;
	CSceneManager::getRenderer()->SpriteDraw(s_pButton[state], &p);
	CSceneManager::getRenderer()->SpriteDrawText(str, &rect, DT_CENTER);
	return bEvent;
}

bool CGUI::InRect(const SRect* rect, const POINTS* p)
{
	return (p->x >= rect->left) && (p->x <= rect->right)
		&& (p->y >= rect->top) && (p->y <= rect->bottom);
}

bool CGUI::List(const SRect* rect, std::vector<std::string>& v, unsigned& cur, unsigned& beg, bool& bEvent)
{
	bool bChanged = false;
	size_t cnt = v.size();
	unsigned visible = (rect->bottom - rect->top) / LIST_ITEM_HEIGHT;

	if (s_input->GetKeyState(VK_UP)) {
		if (cur > 0) {
			cur--;
			bChanged = true;
		}
		if (cur < beg) beg = cur;
	}
	else if (s_input->GetKeyState(VK_DOWN)) {
		if (cur < cnt - 1) {
			cur++;
			bChanged = true;
		}
		if (cur >= (beg + visible)) beg = cur - visible + 1;
	} 

	// ¹öÂÖÊÂ¼þ
	if (s_input->GetKeyState(KS_WHEEL_UP) && beg > 0) beg--;
	else if (s_input->GetKeyState(KS_WHEEL_DOWN) && beg < cnt - visible) beg++;
	// ×ó¼üµ¥»÷
	if (s_input->GetKeyState(KS_LBUTTON_CLICK)) {
		if (InRect(rect, s_pUp) && InRect(rect, s_pDown)) 
			cur = beg + (s_pUp->y - rect->top) / LIST_ITEM_HEIGHT;
		bChanged = true;
	}
	// ×ó¼üË«»÷
	if (s_input->GetKeyState(KS_LBUTTON_DOUBLE_CLICK)) {
		if (InRect(rect, s_pUp) && InRect(rect, s_pDown)) {
			cur = beg + (s_pUp->y - rect->top) / LIST_ITEM_HEIGHT;
			bEvent = true;
		} 
	} else if (s_input->GetKeyState(VK_RETURN)) {
		bEvent = true;
	} else bEvent = false;

	SRect r = *rect;
	r.bottom = r.top + LIST_ITEM_HEIGHT;
	size_t limit = min(beg + visible, cnt);
	for (size_t i = beg; i < limit; i++) {
		CSceneManager::getRenderer()->SpriteDrawText(v[i].c_str(), &r, 
			DT_CENTER | DT_VCENTER, i == cur ? 0xffffffff : 0x7fffffff);
		r.top += LIST_ITEM_HEIGHT;
		r.bottom += LIST_ITEM_HEIGHT;
	}
	return bChanged;
}
