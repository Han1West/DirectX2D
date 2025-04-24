#include "pch.h"
#include "CPauseMenuScript.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>

#include "CPlayerScript.h"
#include "CLevelScript.h"

CPauseMenuScript::CPauseMenuScript()
	: CScript(SCRIPT_TYPE::PAUSEMENUSCRIPT)
{

}
CPauseMenuScript::~CPauseMenuScript()
{
}

void CPauseMenuScript::Begin()
{
}

void CPauseMenuScript::Tick()
{
	CPlayerScript* pScript = (CPlayerScript*)CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScripts()[0];
	if (pScript->IsPaued())
	{
		Active();
	}
}

void CPauseMenuScript::BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CPauseMenuScript::Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CPauseMenuScript::EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CPauseMenuScript::SaveComponent(FILE* _File)
{
}

void CPauseMenuScript::LoadComponent(FILE* _File)
{
}

void CPauseMenuScript::Active()
{
	if (KEY_TAP(KEY::W) || KEY_TAP(KEY::UP))
	{
		m_Idx -= 1;
		if (m_Idx < 0)
			m_Idx = 2;
	}

	if (KEY_TAP(KEY::S) || KEY_TAP(KEY::DOWN))
	{
		m_Idx += 1;
		if (2 < m_Idx)
			m_Idx = 0;
	}

	if (KEY_TAP(KEY::ENTER))
	{
		// ���� �簳
		if (m_Idx == 0)
		{
			CPlayerScript* pScript = (CPlayerScript*)CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScripts()[0];
			pScript->SetPause(false);

			CLevelScript* pLevelScript = (CLevelScript*)CLevelMgr::GetInst()->FindObjectByName(L"LevelInfo")->GetScripts()[0];
			pLevelScript->GetBGM()->SetVolume(0.4f, pLevelScript->GetBGMIdx());

			// PP����
			DestroyObject(CLevelMgr::GetInst()->FindObjectByName(L"HighlightPP"));
		}

		// é�� �����
		if (m_Idx == 1)
		{
			CLevelScript* pLevelScript = (CLevelScript*)CLevelMgr::GetInst()->FindObjectByName(L"LevelInfo")->GetScripts()[0];
			CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();

			wstring LevelPath = L"";

			int Round = pLevelScript->GetStageRound();

			wstring strRound = to_wstring(Round);

			wstring ContentPath = CPathMgr::GetInst()->GetContentPath();
			

			if (pLevelScript->IsHome())
				LevelPath = L"Level\\PlayerHouse_Floor2.lv";
			else
			{
				LevelPath = L"Level\\Stage" + strRound + L"_Floor1.lv";
			}

			ChangeLevelState(LEVEL_STATE::STOP);
			CLevel* LoadedLevel = pCurLevel->g_LevelLoad(ContentPath + LevelPath);
			pLevelScript->GetBGM()->Stop();

			vector<CGameObject*> pNextObjVector2 = LoadedLevel->GetLayer(0)->GetParentObjects();
			// ���� ������ ���� Ŭ���� ������ �Ѱ� �ش�. 
			for (UINT i = 0; i < pNextObjVector2.size(); ++i)
			{
				if (pNextObjVector2[i]->GetName() == L"LevelInfo")
				{
					CLevelScript* CurLevelScript = (CLevelScript*)CLevelMgr::GetInst()->FindObjectByName(L"LevelInfo")->GetScripts()[0];
					CLevelScript* NextLevelScript = (CLevelScript*)pNextObjVector2[i]->GetScripts()[0];
					NextLevelScript->SetStageRound(Round);
					
				}
			}


			ChangeLevel(LoadedLevel, LEVEL_STATE::PLAY);


		}

		// ����ȭ������
		if (m_Idx == 2)
		{
			CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
			CLevelScript* pLevelScript = (CLevelScript*)CLevelMgr::GetInst()->FindObjectByName(L"LevelInfo")->GetScripts()[0];
			pLevelScript->GetBGM()->Stop();

			ChangeLevelState(LEVEL_STATE::STOP);
			wstring ContentPath = CPathMgr::GetInst()->GetContentPath();

			CLevel* Main = pCurLevel->g_LevelLoad(ContentPath + L"Level\\Main.lv");
			ChangeLevel(Main, LEVEL_STATE::PLAY);
		}

	}

}
