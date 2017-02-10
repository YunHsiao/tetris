#include "Utility.h"
#include "SceneManager.h"

INT WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR	 lpCmdLine, 
				   INT       nShowCmd)
{
	CSceneManager::getInstance()->Go();
	return 0;
}