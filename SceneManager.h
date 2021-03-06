#ifndef CSceneManager_H_
#define CSceneManager_H_
#include "Scene.h"
#include "Renderer.h"

class CSceneManager
{
	CSceneManager();
	~CSceneManager();
public:
	static CSceneManager* getInstance() { return &s_director; }
	static CRenderer* getRenderer() { return s_pRenderer; }

	void Go();

	void onInit();
	void onTick(int iElapsedTime);
	void onRender();

	inline void OnKillFocus() { s_scene.OnKillFocus(); }
	inline void OnSetFocus() { s_scene.OnSetFocus(); }
private:
	static CSceneManager s_director;
	static CRenderer* s_pRenderer;
	static CScene s_scene;
	static std::string s_FPS, s_renderer;
};
#endif
