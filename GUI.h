#ifndef CGUI_H
#define CGUI_H

class CInput;
class CGUI {
public:
	static void onInit();
	static bool Button(const char* str, short x, short y, unsigned hotkey = 0);
private:
	CGUI();
	~CGUI();
	static bool InRect(const SRect* rect, const POINTS* p);
	static const CInput* s_input;
	static const POINTS *s_pCur, *s_pDown, *s_pUp;
	static bool s_bUpMsg;
	static unsigned s_pButton[3];
};
#endif
