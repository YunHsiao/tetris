#ifndef CScene_H_
#define CScene_H_

#define SCENE_WIDTH 12		// 场景宽度
#define SCENE_HEIGHT 22		// 场景高度
#define DROP_INTERVAL 800	// 倒计时上限
#define KEY_VALID 650		// 倒计时在此上限内按键有效

enum ETestOperation {
	ETO_EMPTY,				 // 简单碰撞检测
	ETO_DRAW,				 // 置入指定方块
	ETO_CLEAN				 // 清除指定方块
};

class CInput;
class CScene
{
public:
	CScene();
	~CScene();
	void onInit();
	void onTick(int iElapsedTime);
	void onGUI();
	void onRender();
	inline void OnKillFocus() { if (!m_bPaused) { m_bPaused = true; m_bLost = true; } }
	inline void OnSetFocus() { if (m_bLost) { m_bPaused = false; m_bLost = false; } }
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
	bool m_bPaused, m_bLost, m_bOver, m_bUpdate, m_bDown;

	// For Rendering
	inline void UpdateScore() 
	{ 
		std::ostringstream oss;
		oss << "Score: " << m_iScore << "\nLine Cleared: " << m_iLines;
		m_score = oss.str(); 
	}
	SRect m_rScore;
	std::string m_score;
	CInput* m_input;
	unsigned long m_color[8], m_mask;
};
#endif
