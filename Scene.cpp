#include "Utility.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Input.h"
#include "GUI.h"
#include "xml.h"
#include <direct.h>

CScene::CScene() 
	:m_input(0)
{
}

CScene::~CScene() 
{
}

void CScene::onInit() 
{
	m_input = CInput::getInstance();
	UINT t; 
	rand_s(&t);	
	m_iNextP = t % 7 + 1; 
	m_iNextS = t % 4;

	// 七种基本体
	int sp[8][4] = {{0xf00, 0x4444}, 
					{0x740, 0x622, 0x170, 0x446}, 
					{0x710, 0x226, 0x470, 0x644}, 
					{0xe40, 0x4c40, 0x4e00, 0x4640}, 
					{0x6c0, 0x4620}, 
					{0xc60, 0x2640}, 
					{0x660, 0x660}, 
					{-1}};

	// Hue 25-285 Dist 50 Base D38047
	m_color[1] = 0xffd38047; 
	m_color[2] = 0xffd3b047; 
	m_color[3] = 0xff404094;
	m_color[4] = 0xff2d8677; 
	m_color[5] = 0xff5e388f; 
	m_color[6] = 0xffb03b72;
	m_color[7] = 0xff8fc241; 
	m_color[0] = 0xffd3c647;

	int *p, i, j, b;
	for (p = sp[0]; *p >= 0; p++) 
		if (*p == 0) 
			*p = p[-2];

	m_pPool = &m_pool[1];
	for (j = 1; j < 8; j++)
		for (i = 0; i < 4; i++)
			for (b = 0; b < 16; b++)
				m_map[j][i][b] = (sp[j-1][i] & 1) * j, sp[j-1][i] >>= 1;

	memset(m_pool, -1, sizeof(m_pool));

	m_pBg = CSceneManager::getRenderer()->CreateTexture("bg.png");
	m_pTile = CSceneManager::getRenderer()->CreateTexture("tile.png");
	m_rScore.left = 380; 
	m_rScore.top = 280;
	m_rScore.right = 620; 
	m_rScore.bottom = 550;
	// 平滑游戏控制
	m_input->KeepKeyState(VK_LEFT, true);
	m_input->KeepKeyState(VK_RIGHT, true);
	m_input->KeepKeyState(VK_DOWN, true);

	GetCurrentDirectory(MAX_PATH, m_strCurrent);
	if (GetFileAttributes("saves\\") == -1) 
		_mkdir("saves\\"); 
	strcat_s(m_strCurrent, "\\saves\\");
	SetCurrentDirectory(m_strCurrent);
	strcat_s(m_strCurrent, "*");

	NewGame();
}

void CScene::NewGame() 
{
	for (int i = 0; i < SCENE_WIDTH; i++)
		memset(&m_pPool[i], 0, sizeof(int[SCENE_HEIGHT+1]));

	NextTile();
	m_iScore = m_iLines = 0; 
	m_bOver = false;
	InitParams();
}

void CScene::InitParams() 
{
	UpdateScore(); 
	if (m_bOver) {
		m_mask = 0x7fffffff;
		m_score += "\n\nGame Over\nPress N to Start\nPress S to Save";
		m_score += "\nPress L to Load\nPress Q to Quit";
	} else {
		m_bLoad = m_bPaused = m_bDown = false; 
		m_mask = 0xffffffff;
		m_iAdjust = 0;
		m_iPY = SCENE_HEIGHT;
		m_bUpdate = true;
		m_iKeyTime = KEY_INTERVAL;
	}
}


void CScene::NextTile() 
{
	UINT t;
	m_iPattern = m_iNextP; 
	m_iStatus = m_iNextS;
	rand_s(&t);	
	m_iNextP = t % 7 + 1; 
	m_iNextS = t % 4;
	m_iX = (SCENE_WIDTH - 4) / 2; 
	m_iY = 1;
	m_iTime = DROP_INTERVAL;
	m_bUpdate = true;
	for (t = 0; m_map[m_iPattern][m_iStatus][t] == 0; t++);
	m_iY -= t / 4;
}

// ETO_CLEAN 清除指定方块
// ETO_DRAW  置入指定方块
// ETO_EMPTY 简单碰撞检测
bool CScene::Test(int sp[], int x, int y, int c) 
{
	int i, cx, cy;
	for (i = 0; i < 16; i++) if (sp[i]) {
		cx = x + (i & 3), cy = y + (i >> 2);
		if (m_pPool[cx][cy]) {
			if (c == ETO_CLEAN) {
				m_pPool[cx][cy] = 0; 
			} else return false;
		}
		if (c == ETO_DRAW) {
			m_pPool[cx][cy] = sp[i];
		}
	}
	return true;
}

// 详细碰撞检测：
// 返回值 < 0  方块位于边界外
// 返回值 == 0 方块位置正常
// 返回值 > 0  方块与场景方块碰撞
int CScene::FullCollision(int sp[], int x, int y) 
{
	int i, pool, mi = 8;
	for (i = 0; i < 16; i++) if (sp[i]) {
		pool = m_pPool[x + (i & 3)][y + (i >> 2)];
		if (pool && (pool < mi)) {
			mi = pool;
		}
	}
	return mi;
}

void CScene::onTick(int iElapsedTime) 
{
	// 游戏流程控制
	if (m_input->GetKeyState('Q')) PostQuitMessage(0);
	else if (m_input->GetKeyState('N')) NewGame();
	else if (m_input->GetKeyState('S')) SaveGame();
	else if (m_input->GetKeyState('L')) LoadSwitch(!m_bLoad);
	else if (m_input->GetKeyState(VK_ESCAPE)) m_bPaused = !m_bPaused;
	if (m_bPaused || m_bOver || m_bLoad) return;
	// 左右移动
	m_iTime -= iElapsedTime; m_iKeyTime -= iElapsedTime;
	if (m_input->GetKeyState(VK_LEFT) && m_iKeyTime < 0 && 
		Test(m_map[m_iPattern][m_iStatus], m_iX - 1, m_iY, ETO_EMPTY)) {
			m_iX--;
			m_bUpdate = true;
			m_iKeyTime = KEY_INTERVAL;
	} else if (m_input->GetKeyState(VK_RIGHT) && m_iKeyTime < 0 && 
		Test(m_map[m_iPattern][m_iStatus], m_iX + 1, m_iY, ETO_EMPTY)) {
			m_iX++;
			m_bUpdate = true;
			m_iKeyTime = KEY_INTERVAL;
	}
	// 方块旋转
	if (m_input->GetKeyState(VK_SPACE) || m_input->GetKeyState(VK_UP)) {
		int iNextS = (m_iStatus + 1) % 4;
		int iCol;
		int offset = 0; 
		while ((iCol = FullCollision(m_map[m_iPattern][iNextS], m_iX + offset, m_iY)) < 0) {
			m_iAdjust++;
			if (m_iX > SCENE_WIDTH / 2) offset--;
			else offset++;
			if (abs(offset) > 2) {
				offset = 0; 
				break;
			}
		}
		m_iX += offset;
		if (iCol == 8) {
			m_iStatus = iNextS;
			m_bUpdate = true;
		}
	}
	// 方块下落
	if (m_iTime < 0 || m_input->GetKeyState(VK_DOWN) && m_iKeyTime < 0) {
		m_iKeyTime = KEY_INTERVAL;
		// 方块下落一格
		if (Test(m_map[m_iPattern][m_iStatus], m_iX, m_iY + 1, ETO_EMPTY)) {
			m_iY++;
			m_iTime = DROP_INTERVAL;
		} else {
			// 方块落入场景
			Test(m_map[m_iPattern][m_iStatus], m_iX, m_iY, ETO_DRAW);
			int x, t, cnt = 0;
			// 消行检测与处理
			for (int y = SCENE_HEIGHT; y > 0; y--) {
				for (x = 0; m_pPool[x][y] > 0; x++);
				if (m_pPool[x][y] < 0) {
					cnt++;
					for (t = y++; t > 0; t--)
						for (x = 0; m_pPool[x][0] >= 0; x++)
							m_pPool[x][t] = m_pPool[x][t-1];
				}
			}
			// 分数计算
			if (cnt) {
				m_iLines += cnt;
				m_iScore += cnt * 15 - 5;
				UpdateScore();
			}
			// 下一方块及游戏结束检测
			NextTile();
			if (!Test(m_map[m_iPattern][m_iStatus], m_iX, m_iY, ETO_EMPTY)) {
				m_bOver = true; 
				InitParams();
			}
		}
	} else if (m_input->GetKeyInterval(VK_DOWN) < 300) {
		// 双击直接落位
		m_iY = m_iPY;
		m_iTime = 0;
		m_input->ResetKeyInterval(VK_DOWN);
	}
	// 预计下落位置更新
	if (m_bUpdate) {
		for (m_iPY = m_iY; m_iPY <= SCENE_HEIGHT; m_iPY++)
			if (!Test(m_map[m_iPattern][m_iStatus], m_iX, m_iPY + 1, ETO_EMPTY))
				break;
		m_bUpdate = false;
	}
}

void CScene::LoadSwitch(bool on)
{
	m_bLoad = on;
	m_input->KeepKeyState(VK_UP,    !on);
	m_input->KeepKeyState(VK_DOWN,  !on);
	if (!on) return;
	m_saves.clear();

	WIN32_FIND_DATA file;
	HANDLE hListFile = FindFirstFile(m_strCurrent, &file);
	if (hListFile == INVALID_HANDLE_VALUE) return;
	FindNextFile(hListFile, &file);
	while (FindNextFile(hListFile, &file)) 
		m_saves.push_back(file.cFileName);
	FindClose(hListFile);
}


void CScene::onGUI()
{
	static unsigned cur = 0;
	static unsigned beg = 0;
	static SRect rect = { 50, 60, 350, 600 };
	if (m_bLoad) {
		if (CGUI::List(&rect, m_saves, cur, beg) || CGUI::Button("Load", 440, 350, VK_RETURN)) {
			char str[MAX_PATH];
			strncpy_s(str, MAX_PATH, m_strCurrent, strlen(m_strCurrent) - 1);
			strcat_s(str, m_saves[cur].c_str());
			LoadGame(str);
			LoadSwitch(false);
		}
		if (CGUI::Button("Cancel", 440, 410, VK_ESCAPE)) {
			LoadSwitch(false);
			m_bPaused = false; // 游戏本体也捕捉ESC事件
		}
	}
}

void CScene::onRender() 
{
	CSceneManager::getRenderer()->SpriteDraw(m_pBg);
	SVector vPos = { 0.f, 0.f, 0.f };
	// 暂停界面
	if (m_bPaused && !m_bOver || m_bLoad) {
		static char str[8] = "Pause";
		static char strL[16] = "Loading...";
		for (int x = 0; m_pPool[x][0] >= 0; x++) {
			vPos.x = 50.f + x * 25;
			for (int y = 1; m_pPool[x][y] >= 0; y++) {
				vPos.y = 25.f + y * 25;
				CSceneManager::getRenderer()->SpriteDraw(m_pTile, &vPos, 
					m_color[((x+5) * (y+3)) % 7 + 1] & 0x4fffffff);
			}
		}
		CSceneManager::getRenderer()->SpriteDrawText(m_bLoad? strL : str, &m_rScore, DT_CENTER);
		return;
	}
	// 预计下落位置
	Test(m_map[m_iPattern][m_iStatus], m_iX, m_iY, ETO_DRAW);
	int* t = m_map[m_iPattern][m_iStatus];
	for (int x = 0; x < 4; x++) {
		vPos.x = 50.f + (m_iX + x) * 25;
		for (int y = 0; y < 4; y++) {
			if (!t[y * 4 + x]) continue;
			vPos.y = 25.f + (m_iPY + y) * 25;
			CSceneManager::getRenderer()->SpriteDraw(m_pTile, &vPos, 
				m_color[t[y * 4 + x]] & 0x4fffffff);
		}
	}
	// 已在场景中的方块
	for (int x = 0; m_pPool[x][0] >= 0; x++) {
		vPos.x = 50.f + x * 25;
		for (int y = 1; m_pPool[x][y] >= 0; y++) {
			if (!m_pPool[x][y]) continue;
			vPos.y = 25.f + y * 25;
			CSceneManager::getRenderer()->SpriteDraw(m_pTile, &vPos, 
				m_color[m_pPool[x][y]] & m_mask);
		}
	}
	// 下一方块
	t = m_map[m_iNextP][m_iNextS];
	for (int x = 0; x < 4; x++) {
		vPos.x = 450.f + x * 25;
		for (int y = 0; y < 4; y++) {
			if (!t[y * 4 + x]) continue;
			vPos.y = 150.f + y * 25;
			CSceneManager::getRenderer()->SpriteDraw(m_pTile, &vPos, 
				m_color[t[y * 4 + x]] & m_mask);
		}
	}
	Test(m_map[m_iPattern][m_iStatus], m_iX, m_iY, ETO_CLEAN);
	// 分数及提示信息
	CSceneManager::getRenderer()->SpriteDrawText(m_score.c_str(), &m_rScore, DT_CENTER);
}

void CScene::SaveGame() {
	std::ostringstream oss;
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
		oss << m_iScore << "_" << m_iLines << "_" << timeGetTime() << ".tsf";
		FILE* file = 0;
		fopen_s(&file, oss.str().c_str(), "wb");
		fwrite(&m_iScore, sizeof(int), 1, file);
		fwrite(&m_iLines, sizeof(int), 1, file);
		fwrite(&m_bOver, sizeof(bool), 1, file);
		fwrite(&m_iX, sizeof(int), 1, file);
		fwrite(&m_iY, sizeof(int), 1, file);
		fwrite(&m_iPattern, sizeof(int), 1, file);
		fwrite(&m_iStatus, sizeof(int), 1, file);
		fwrite(&m_iNextP, sizeof(int), 1, file);
		fwrite(&m_iNextS, sizeof(int), 1, file);
		fwrite(&m_iTime, sizeof(int), 1, file);
		fwrite(&m_pool, sizeof(m_pool), 1, file);
		fclose(file);
		return;
	}
	XMLDocument doc;
	XMLDeclaration *dec;
	XMLElement *node, *child;

	dec = doc.NewDeclaration();
	doc.InsertEndChild(dec);

	node = doc.NewElement("Progress");
	child = doc.NewElement("Score");
	child->SetText(m_iScore);
	node->InsertEndChild(child);
	child = doc.NewElement("Lines");
	child->SetText(m_iLines);
	node->InsertEndChild(child);
	child = doc.NewElement("Over");
	child->SetText((int) m_bOver);
	node->InsertEndChild(child);
	doc.InsertEndChild(node);

	node = doc.NewElement("Current");
	child = doc.NewElement("X");
	child->SetText(m_iX);
	node->InsertEndChild(child);
	child = doc.NewElement("Y");
	child->SetText(m_iY);
	node->InsertEndChild(child);
	child = doc.NewElement("Pattern");
	child->SetText(m_iPattern);
	node->InsertEndChild(child);
	child = doc.NewElement("Status");
	child->SetText(m_iStatus);
	node->InsertEndChild(child);
	child = doc.NewElement("NextPattern");
	child->SetText(m_iNextP);
	node->InsertEndChild(child);
	child = doc.NewElement("NextStatus");
	child->SetText(m_iNextS);
	node->InsertEndChild(child);
	child = doc.NewElement("Time");
	child->SetText(m_iTime);
	node->InsertEndChild(child);
	doc.InsertEndChild(node);

	node = doc.NewElement("Pool");
	for (int i = 0; i <= SCENE_HEIGHT; i++) {
		oss.str("");
		child = doc.NewElement("Line");
		for (int j = 0; j < SCENE_WIDTH; j++) {
			if (m_pPool[j][i]) oss << m_pPool[j][i];
			else oss << "-";
		}
		child->SetText(oss.str().c_str());
		node->InsertEndChild(child);
	}
	doc.InsertEndChild(node);

	oss.str("");
	oss << m_iScore << "_" << m_iLines << "_" << timeGetTime();

	if (GetAsyncKeyState(VK_CONTROL) & 0x8000) oss << ".xml";
	else oss << ".tst";

	doc.SaveFile(oss.str().c_str());
}

void CScene::LoadGame(const char* str) {
	if (str[strlen(str)-1] == 'f') {
		FILE* file = 0;
		fopen_s(&file, str, "rb");
		fread_s(&m_iScore, sizeof(int), sizeof(int), 1, file);
		fread_s(&m_iLines, sizeof(int), sizeof(int), 1, file);
		fread_s(&m_bOver, sizeof(bool), sizeof(bool), 1, file);
		fread_s(&m_iX, sizeof(int), sizeof(int), 1, file);
		fread_s(&m_iY, sizeof(int), sizeof(int), 1, file);
		fread_s(&m_iPattern, sizeof(int), sizeof(int), 1, file);
		fread_s(&m_iStatus, sizeof(int), sizeof(int), 1, file);
		fread_s(&m_iNextP, sizeof(int), sizeof(int), 1, file);
		fread_s(&m_iNextS, sizeof(int), sizeof(int), 1, file);
		fread_s(&m_iTime, sizeof(int), sizeof(int), 1, file);
		fread_s(&m_pool, sizeof(m_pool), sizeof(m_pool), 1, file);
		fclose(file);
	} else {
		XMLDocument doc;
		if (doc.LoadFile(str)) return;
		XMLElement *node, *child;
		const char* str;

		node = doc.FirstChildElement("Progress");
		m_iScore = atoi(node->FirstChildElement("Score")->GetText());
		m_iLines = atoi(node->FirstChildElement("Lines")->GetText());
		m_bOver = atoi(node->FirstChildElement("Over")->GetText()) != 0;

		node = doc.FirstChildElement("Current");
		m_iX = atoi(node->FirstChildElement("X")->GetText());
		m_iY = atoi(node->FirstChildElement("Y")->GetText());
		m_iPattern = atoi(node->FirstChildElement("Pattern")->GetText());
		m_iStatus = atoi(node->FirstChildElement("Status")->GetText());
		m_iNextP= atoi(node->FirstChildElement("NextPattern")->GetText());
		m_iNextS = atoi(node->FirstChildElement("NextStatus")->GetText());
		m_iTime = atoi(node->FirstChildElement("Time")->GetText());

		node = doc.FirstChildElement("Pool");
		child = node->FirstChildElement("Line");
		for (unsigned i = 0; i <= SCENE_HEIGHT; i++) {
			str = child->GetText();
			for (unsigned j = 0; j < SCENE_WIDTH; j++, str++) {
				if (*str == '-') 
					m_pPool[j][i] = 0;
				else 
					m_pPool[j][i] = *str - '0';
			}
			child = child->NextSiblingElement();
		}
	}
	InitParams();
}
