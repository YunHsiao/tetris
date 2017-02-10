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
	int primes[8][4] = {{ 0xf00, 0x4444 }, 
					{ 0xe80, 0xc440, 0x2e00, 0x4460 }, 
					{ 0xe20, 0x44c0, 0x8e00, 0x6440 }, 
					{ 0xe40, 0x4c40, 0x4e00, 0x4640 }, 
					{ 0x6c0, 0x4620 }, 
					{ 0xc60, 0x2640 }, 
					{ 0x660, 0x660 }, 
					{-1}};

	// Hue 25-285 Dist 50 Base D38047
	m_vColor[0] = 0xffffffff;
	m_vColor[1] = 0xff000000;
	m_vColor[2] = 0xffd38047; 
	m_vColor[3] = 0xffd3b047; 
	m_vColor[4] = 0xff404094;
	m_vColor[5] = 0xff2d8677; 
	m_vColor[6] = 0xff5e388f; 
	m_vColor[7] = 0xffb03b72;
	m_vColor[8] = 0xff8fc241;
	//m_vColor[8] = 0xffd3c647;
	m_color = &m_vColor[1];

	for (int* p = primes[0]; *p >= 0; p++) 
		if (*p == 0) *p = p[-2];
	
	// m_map[j][i]:
	// [0] ... [3]
	// [4] ... [7]
	// [8] ...[11]
	// [12]...[15]
	// 
	// m_map[1]:
	// - - - -		- - 1 -		- - - -		- - 1 -  
	// - - - -		- - 1 -		- - - -		- - 1 -  
	// 1 1 1 1		- - 1 -		1 1 1 1		- - 1 - 
	// - - - -		- - 1 -		- - - -		- - 1 - 
	// 
	// m_map[2]:
	// - - - -		- - - -		- - - -		- - - -  
	// - - - 2		- - 2 -		- - - -		- 2 2 -  
	// - 2 2 2		- - 2 -		- 2 2 2		- - 2 - 
	// - - - -		- - 2 2		- 2 - -		- - 2 - 
	// 
	// ... (primes.xlsx)
	// 

	for (int i = 1; i < 8; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 16; k++)
				m_map[i][j][k] = (primes[i-1][j] & 1) * i, primes[i-1][j] >>= 1;

	// m_pool:
	//  [0][0]  ......  [W+2][0]
	//  ......  ......   ......
	// [0][H+2] ...... [W+2][H+2]
	// 
	// m_pPool
	//     ↓
	// -1  0 ..  0 -1
	// .. .. .. .. ..
	// -1  0 ..  0 -1
	// -1 -1 .. -1 -1
	// 
	// -1为场景边界，0为有效区域
	// 

	m_pPool = &m_pool[1];
	memset(m_pool, -1, sizeof(m_pool));
	memset(m_poolPreview, -1, sizeof(m_pool));
	for (int i = 1; i < SCENE_WIDTH + 1; i++)
		memset(&m_poolPreview[i][1], 0, sizeof(char) * SCENE_HEIGHT);

	m_pBg = CSceneManager::getRenderer()->CreateTexture(TRS_TEXTURE_BG);
	m_pTile = CSceneManager::getRenderer()->CreateTexture(TRS_TEXTURE_TILE);
	m_pTilePreview = CSceneManager::getRenderer()->CreateTexture(TRS_TEXTURE_PREVIEW);
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
	m_iBeg = m_iCur = 0;

	NewGame();
}

void CScene::NewGame() 
{
	for (int i = 0; i < SCENE_WIDTH; i++)
		memset(&m_pPool[i], 0, sizeof(char) * (SCENE_HEIGHT + 1));

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
		m_bLoad = m_bPaused = false; 
		m_mask = 0xffffffff;
		m_iAdjust = 0;
		m_iPY = SCENE_HEIGHT;
		m_bUpdate = true;
		m_iKeyTime = KEY_INTERVAL;
		m_input->ResetKeyInterval(VK_DOWN);
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
bool CScene::Test(char sp[], int x, int y, int c) 
{
	int i, cx, cy;
	for (i = 0; i < 16; i++) if (sp[i]) {
		cx = x + (i & 3), cy = y + (i >> 2);
		if (m_pPool[cx][cy] < 0) return false;
		if (m_pPool[cx][cy]) {
			if (c == ETO_CLEAN)
				m_pPool[cx][cy] = 0; 
			else return false;
		}
		if (c == ETO_DRAW)
			m_pPool[cx][cy] = sp[i];
	}
	return true;
}

// 详细碰撞检测：
// 返回值 < 0  方块位于边界外
// 返回值 == 0 方块位置正常
// 返回值 > 0  方块与场景方块碰撞
int CScene::FullCollision(char sp[], int x, int y) 
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
				if (x >= SCENE_WIDTH) {
					cnt++;
					for (t = y++; t > 0; t--)
						for (x = 0; x < SCENE_WIDTH; x++)
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
	} else if (m_input->GetKeyInterval(VK_DOWN) < 300 && !m_bUpdate) {
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
	m_input->KeepKeyState(VK_DOWN,  !on);
	if (!on) return;

	WIN32_FIND_DATA file;
	HANDLE hListFile = FindFirstFile(m_strCurrent, &file);
	if (hListFile == INVALID_HANDLE_VALUE) return;
	FindNextFile(hListFile, &file);
	
	m_saves.clear();
	std::multimap<int, std::string> saves;
	while (FindNextFile(hListFile, &file))
		saves.insert(std::pair<int, std::string>(atoi(file.cFileName), file.cFileName));
	for (std::multimap<int, std::string>::iterator it(saves.begin()); it != saves.end(); it++)
		m_saves.push_back(it->second);
	FindClose(hListFile);
	if (!m_saves.size()) return;
	strncpy_s(m_filePath, MAX_PATH, m_strCurrent, strlen(m_strCurrent) - 1);
	strcat_s(m_filePath, m_saves[m_iCur].c_str());
	LoadGame(m_filePath, false); 
}


void CScene::onGUI()
{
	static bool bEvent = false;
	static SRect rect = { 50, 60, 350, 600 };
	if (m_bLoad) {
		if (CGUI::List(&rect, m_saves, m_iCur, m_iBeg, bEvent)) {
			strncpy_s(m_filePath, MAX_PATH, m_strCurrent, strlen(m_strCurrent) - 1);
			strcat_s(m_filePath, m_saves[m_iCur].c_str());
			LoadGame(m_filePath, false);
		}
		if (bEvent || CGUI::Button("Load", 440, 350, VK_RETURN)) {
			if (m_saves.size()) {
				LoadGame(m_filePath, true);
				LoadSwitch(false);
			}
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
		// 暂停场景绘制 (也可用于观察m_pool边界异常)
		for (int x = 0; x < SCENE_WIDTH + 2; x++) {
			vPos.x = 25.f + x * 25;
			for (int y = 1; y < SCENE_HEIGHT + 2; y++) {
				if (m_pool[x][y] < 0) continue;
				vPos.y = 25.f + y * 25;
				CSceneManager::getRenderer()->SpriteDraw(m_pTile, &vPos, 
					m_color[((x+4) * (y+3)) % 7 + 1] & 0x4fffffff);
			}
		}
		// 存档预览
		if (m_bLoad) {
			int colorIndex;
			for (int x = 0; x < SCENE_WIDTH + 2; x++) {
				vPos.x = 472.f + x * 4;
				for (int y = 0; y < SCENE_HEIGHT + 2; y++) {
					colorIndex = m_poolPreview[x][y];
					if (!colorIndex) continue;
					vPos.y = 152.f + y * 4;
					CSceneManager::getRenderer()->SpriteDraw(m_pTilePreview, 
						&vPos, m_color[colorIndex]);
				}
			}
		}
		CSceneManager::getRenderer()->SpriteDrawText(m_bLoad? strL : str,
			&m_rScore, DT_CENTER);
		return;
	}
	// 预计下落位置
	char* t = m_map[m_iPattern][m_iStatus];
	if (!m_bOver) {
		for (int x = 0; x < 4; x++) {
			vPos.x = 50.f + (m_iX + x) * 25;
			for (int y = 0; y < 4; y++) {
				if (!t[y * 4 + x]) continue;
				vPos.y = 25.f + (m_iPY + y) * 25;
				CSceneManager::getRenderer()->SpriteDraw(m_pTile, &vPos, 
					m_color[t[y * 4 + x]] & 0x4fffffff);
			}
		}
	}
	// 已在场景中的方块
	Test(m_map[m_iPattern][m_iStatus], m_iX, m_iY, ETO_DRAW);
	for (int x = 0; x < SCENE_WIDTH; x++) {
		vPos.x = 50.f + x * 25;
		for (int y = 1; y <= SCENE_HEIGHT; y++) {
			if (!m_pPool[x][y]) continue;
			vPos.y = 25.f + y * 25;
			CSceneManager::getRenderer()->SpriteDraw(m_pTile, &vPos, 
				m_color[m_pPool[x][y]] & m_mask);
		}
	}
	Test(m_map[m_iPattern][m_iStatus], m_iX, m_iY, ETO_CLEAN);
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
	// 分数及提示信息
	CSceneManager::getRenderer()->SpriteDrawText(m_score.c_str(), &m_rScore, DT_CENTER);
}

void CScene::SaveGame() {
	//long long va(toNumber(L"  \t  \n\t0x1e2b4f\t  \t\n "));
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
			if (m_pPool[j][i]) oss << (short) m_pPool[j][i];
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

void CScene::LoadGame(const char* str, bool official) {
	if (str[strlen(str)-1] == 'f') {
		FILE* file = 0;
		fopen_s(&file, str, "rb");
		if (official) {
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
			fseek(file, sizeof(int) * 9 + sizeof(bool), SEEK_SET);
			fread_s(&m_poolPreview, sizeof(m_poolPreview), sizeof(m_pool), 1, file);
			fclose(file);
			for (int i = 0; i < SCENE_WIDTH + 2; i++) 
				m_poolPreview[i][0] = -1;
			return;
		}
	} else {
		XMLDocument doc;
		if (doc.LoadFile(str)) return;
		XMLElement *node, *child;
		const char* str;

		if (official) {
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
		} else {
			node = doc.FirstChildElement("Pool");
			child = node->FirstChildElement("Line");
			child = child->NextSiblingElement();
			for (unsigned i = 1; i <= SCENE_HEIGHT; i++) {
				str = child->GetText();
				for (unsigned j = 1; j <= SCENE_WIDTH; j++, str++) {
					if (*str == '-') 
						m_poolPreview[j][i] = 0;
					else 
						m_poolPreview[j][i] = *str - '0';
				}
				child = child->NextSiblingElement();
			}
			return;
		}
	}
	InitParams();
}
