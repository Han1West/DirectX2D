#include "pch.h"
#include "CMainMenuScript.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>

CMainMenuScript::CMainMenuScript()
	: CScript(SCRIPT_TYPE::MAINMENUSCRIPT)
{
}

CMainMenuScript::~CMainMenuScript()
{
}

void CMainMenuScript::Begin()
{
	m_BGM = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\bgm\\Main.mp3", L"Sound\\bgm\\Main.mp3");
	m_BGM->Play(0, 0.4f, false, 0.0001);
}

void CMainMenuScript::Tick()
{
	if (KEY_TAP(KEY::W) || KEY_TAP(KEY::UP))
	{
		m_CurIdx -= 1;
		if (m_CurIdx < 0)
			m_CurIdx = 1;
	}

	if (KEY_TAP(KEY::S) || KEY_TAP(KEY::DOWN))
	{
		m_CurIdx += 1;
		if (1 < m_CurIdx)
			m_CurIdx = 0;
	}

	if (KEY_TAP(KEY::ENTER))
	{
		// 게임 스타트
		if (m_CurIdx == 0)
		{
			m_BGM->Stop();
			ChangeLevelState(LEVEL_STATE::STOP);
			CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
			wstring ContentPath = CPathMgr::GetInst()->GetContentPath();

			CLevel* pLoadedLevel = pCurLevel->g_LevelLoad(ContentPath + L"Level\\PlayerHouse_Floor2.lv");
			ChangeLevel(pLoadedLevel, LEVEL_STATE::PLAY);
		}
		// 종료
		else
		{
			PostQuitMessage(0);
		}
	}
}

void CMainMenuScript::BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CMainMenuScript::Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CMainMenuScript::EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CMainMenuScript::SaveComponent(FILE* _File)
{
}

void CMainMenuScript::LoadComponent(FILE* _File)
{
}
