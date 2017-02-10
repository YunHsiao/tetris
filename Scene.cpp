#include "Utility.h"
#include "Scene.h"
#include "Renderer.h"

CScene::CScene() 
{
}

CScene::~CScene() 
{
}

void CScene::onInit() {
	UINT t; rand_s(&t);	m_iNextP = t % 7 + 1; m_iNextS = t % 4;
	int sp[8][4] = {{15,4369},{23,785,116,547},{71,275,113,802},
					{39,305,114,562},{54,561},{99,306},{51,51},{-1}};
	m_color[1] = 0xffd38047; m_color[2] = 0xffd3b047; m_color[3] = 0xff404094;
	m_color[4] = 0xff2d8677; m_color[5] = 0xff5e388f; m_color[6] = 0xffb03b72;
	m_color[7] = 0xff8fc241; m_color[0] = 0xffd3c647; // Hue 25-285 Dist 50 Base D38047
	int *p, i, j, b;
	for (p = sp[0]; *p >= 0; p++) if (*p == 0) *p = p[-2];
	m_pPool = &m_pool[1];
	for (j = 1; j < 8; j++)
		for (i = 0; i < 4; i++)
			for (b = 0; b < 16; b++)
				m_map[j][i][b] = (sp[j-1][i] & 1) * j, sp[j-1][i] >>= 1;
	memset(m_pool, -1, sizeof(m_pool));

	m_pBg = CRenderer::getInstance()->CreateTexture("bg.png");
	m_pTile = CRenderer::getInstance()->CreateTexture("tile.png");
	m_rScore.left = 415; m_rScore.top = 300;
	m_rScore.right = 585; m_rScore.bottom = 550;
	NewGame();
}

void CScene::NewGame() {
	for (int i = 0; i < SCENE_WIDTH; i++)
		memset(&m_pPool[i], 0, sizeof(int[SCENE_HEIGHT+1]));
	NextTile();
	m_iScore = m_iLines = 0; 
	m_bOver = false;
	InitParams();
}

void CScene::InitParams() {
	UpdateScore(); 
	if (m_bOver) {
		m_mask = 0x7fffffff;
		m_score += "\n\nGame Over\nPress N to Start\nPress S to Save\nPress L to Load";
	} else {
		m_bPaused = m_bDown = false; 
		m_mask = 0xffffffff;
		m_iAdjust = 0;
		m_iPY = SCENE_HEIGHT;
		m_bUpdate = true;
	}
}


void CScene::NextTile() {
	UINT t;
	m_iPattern = m_iNextP; m_iStatus = m_iNextS;
	rand_s(&t);	m_iNextP = t % 7 + 1; m_iNextS = t % 4;
	m_iX = (SCENE_WIDTH - 2) / 2; m_iY = 1;
	m_iTime = DROP_INTERVAL;
	m_bUpdate = true;
	m_lLastDown = 0;
}

bool CScene::Test(int sp[], int x, int y, int c) {
	int i, cx, cy;
	for (i = 0; i < 16; i++) if (sp[i]) {
		cx = x + (i & 3), cy = y + (i >> 2);
		if (m_pPool[cx][cy])
			if (c == ETO_CLEAN) m_pPool[cx][cy] = 0; else return false;
		if (c == ETO_DRAW) m_pPool[cx][cy] = sp[i];
	}
	return true;
}

int CScene::FullCollision(int sp[], int x, int y) {
	int i, pool, mi = 8;
	for (i = 0; i < 16; i++) if (sp[i]) {
		pool = m_pPool[x + (i & 3)][y + (i >> 2)];
		if (pool && (pool < mi)) mi = pool;
	}
	return mi;
}

void CScene::onTick(int iElapsedTime) {
	if (m_bPaused || m_bOver) return;
	m_iTime -= iElapsedTime;
	if (m_bDown || m_iTime < 0) {
		m_bDown = false;
		if (Test(m_map[m_iPattern][m_iStatus], m_iX, m_iY + 1, ETO_EMPTY))
			m_iY++, m_iTime = DROP_INTERVAL;
		else {
			Test(m_map[m_iPattern][m_iStatus], m_iX, m_iY, ETO_DRAW);
			int x, t, cnt = 0;
			for (int y = SCENE_HEIGHT; y > 0; y--) {
				for (x = 0; m_pPool[x][y] > 0; x++);
				if (m_pPool[x][y] < 0) {
					cnt++;
					for (t = y++; t > 0; t--)
						for (x = 0; m_pPool[x][0] >= 0; x++)
							m_pPool[x][t] = m_pPool[x][t-1];
				}
			}
			if (cnt) {
				m_iLines += cnt;
				m_iScore += cnt * 15 - 5;
				UpdateScore();
			}
			NextTile();
			if (!Test(m_map[m_iPattern][m_iStatus], m_iX, m_iY, ETO_EMPTY)) {
				m_bOver = true; InitParams();
			}
		}
	}
	if (m_bUpdate) {
		for (m_iPY = m_iY; m_iPY <= SCENE_HEIGHT; m_iPY++)
			if (!Test(m_map[m_iPattern][m_iStatus], m_iX, m_iPY + 1, ETO_EMPTY))
				break;
		m_bUpdate = false;
	}
}

void CScene::onRender() {
	CRenderer::getInstance()->SpriteDraw(m_pBg);
	SVector vPos = { 0.f, 0.f, 0.f };
	if (m_bPaused && !m_bOver) {
		static char str[8] = "Pause";
		for (int x = 0; m_pPool[x][0] >= 0; x++) {
			vPos.x = 50.f + x * 25;
			for (int y = 1; m_pPool[x][y] >= 0; y++) {
				vPos.y = 25.f + y * 25;
				CRenderer::getInstance()->SpriteDraw(m_pTile, &vPos, 
					m_color[((x+5) * (y+3)) % 7 + 1] & 0x2fffffff);
			}
		}
		CRenderer::getInstance()->DrawText(str, &m_rScore, DT_CENTER);
		return;
	}
	Test(m_map[m_iPattern][m_iStatus], m_iX, m_iY, ETO_DRAW);
	for (int x = 0; m_pPool[x][0] >= 0; x++) {
		vPos.x = 50.f + x * 25;
		for (int y = 1; m_pPool[x][y] >= 0; y++) {
			if (!m_pPool[x][y]) continue;
			vPos.y = 25.f + y * 25;
			CRenderer::getInstance()->SpriteDraw(m_pTile, &vPos, 
				m_color[m_pPool[x][y]] & m_mask);
		}
	}
	int* t = m_map[m_iPattern][m_iStatus];
	for (int x = 0; x < 4; x++) {
		vPos.x = 50.f + (m_iX + x) * 25;
		for (int y = 0; y < 4; y++) {
			if (!t[y * 4 + x]) continue;
			vPos.y = 25.f + (m_iPY + y) * 25;
			CRenderer::getInstance()->SpriteDraw(m_pTile, &vPos, 
				m_color[t[y * 4 + x]] & 0x4fffffff);
		}
	}
	t = m_map[m_iNextP][m_iNextS];
	for (int x = 0; x < 4; x++) {
		vPos.x = 450.f + x * 25;
		for (int y = 0; y < 4; y++) {
			if (!t[y * 4 + x]) continue;
			vPos.y = 150.f + y * 25;
			CRenderer::getInstance()->SpriteDraw(m_pTile, &vPos, 
				m_color[t[y * 4 + x]] & m_mask);
		}
	}
	CRenderer::getInstance()->DrawText(m_score.c_str(), &m_rScore, DT_CENTER);
	Test(m_map[m_iPattern][m_iStatus], m_iX, m_iY, ETO_CLEAN);
}

void CScene::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_KEYDOWN: 
		if (wParam == 'Q') PostQuitMessage(0);
		else if (wParam == 'N') NewGame();
		else if (wParam == 'S') SaveGame();
		else if (wParam == 'L') LoadGame();
		if (!m_bOver) {
			if (wParam == VK_LEFT && Test(m_map[m_iPattern][m_iStatus], 
				m_iX - 1, m_iY, ETO_EMPTY)) m_iX--, m_bUpdate = true;
			else if (wParam == VK_RIGHT && Test(m_map[m_iPattern][m_iStatus], 
				m_iX + 1, m_iY, ETO_EMPTY)) m_iX++, m_bUpdate = true;
			if (wParam == VK_SPACE || wParam == VK_UP) {
				int iNextS = (m_iStatus + 1) % 4, iCol; bool bAdjust = false;
				while ((iCol = FullCollision(m_map[m_iPattern][iNextS], m_iX, m_iY)) < 0) 
					bAdjust = true, m_iAdjust++, m_iX--;
				if (iCol == 8) m_iStatus = iNextS;
				if (!bAdjust && m_iAdjust) m_iX += m_iAdjust, m_iAdjust = 0;
				m_bUpdate = true;
			} else if (wParam == VK_DOWN) m_bDown = true;
			else if (wParam == VK_ESCAPE) Toggle();
		}
		break;
	case WM_KEYUP: 
		if (wParam == VK_DOWN) {
			unsigned long lLastDown = timeGetTime();
			if ((lLastDown - m_lLastDown) < 300) m_iY = m_iPY, m_iTime = 0;
			m_lLastDown = lLastDown;
		}
		break;
	case WM_KILLFOCUS: if (!m_bPaused) { Pause(); m_bLost = true; } break;
	case WM_SETFOCUS: if (m_bLost) { Resume(); m_bLost = false; } break;
	}
}

void CScene::SaveGame() {
	std::ostringstream oss;
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
}

void CScene::LoadGame() {
	char str[64];
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = str;
	ofn.lpstrFile[0] = '\0';
	ofn.lpstrFilter = TEXT("Tetris Save File\0*.tsf\0");
	ofn.lpstrInitialDir = TEXT(".\\");
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (!GetOpenFileName(&ofn)) return;

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
	InitParams();
}