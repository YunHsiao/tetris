#ifndef CScene_H_
#define CScene_H_

#define SCENE_WIDTH 12
#define SCENE_HEIGHT 22
#define DROP_INTERVAL 800

enum ETestOperation {
	ETO_EMPTY,
	ETO_DRAW,
	ETO_CLEAN
};

class CScene
{
public:
	CScene();
	~CScene();
	void onInit();
	void onTick(int iElapsedTime);
	void onRender();
	void WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	inline void Pause() { m_bPaused = true; }
	inline void Resume() { m_bPaused = false; }
	inline void Toggle() { m_bPaused = !m_bPaused; }
	void NewGame();
	void SaveGame();
	void LoadGame();
private:
	// For Simulation
	bool Test(int sp[], int x, int y, int c);
	int FullCollision(int sp[], int x, int y);
	void NextTile();
	void InitParams();
	int m_pool[SCENE_WIDTH+2][SCENE_HEIGHT+2], (*m_pPool)[SCENE_HEIGHT+2], m_map[8][4][16];
	int m_iX, m_iY, m_iPY, m_iPattern, m_iStatus, m_iTime, m_iNextP, m_iNextS;
	int m_iAdjust, m_iScore, m_iLines, m_pBg, m_pTile;
	unsigned long m_lLastDown;
	bool m_bPaused, m_bDown, m_bLost, m_bOver, m_bUpdate;

	// For Rendering
	inline void UpdateScore() 
	{ m_score = "Score: " + toString(m_iScore) + "\nLine Cleared: " + toString(m_iLines); }
	SRect m_rScore;
	std::string m_score;
	unsigned long m_color[8], m_mask;
};
#endif
