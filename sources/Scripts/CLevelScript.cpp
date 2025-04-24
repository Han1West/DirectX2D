#include "pch.h"
#include "CLevelScript.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CSound.h>

#include "CPlayerScript.h"

CLevelScript::CLevelScript()
	: CScript(SCRIPT_TYPE::LEVELSCRIPT)
	, m_CurFloor(0)
	, m_IsStart(false)
	, m_IsFinal(false)
	, m_bFloorClear(false)
	, m_bLevelClear(false)
	, m_bHome(false)
	, m_bAlreadyClear(false)
	, m_RelativePath()
	, m_NextLevelPath()
	, m_StageRound(0)
	, m_BgmPosition(0)
	, m_BGM(nullptr)
{
}

CLevelScript::~CLevelScript()
{
}

void CLevelScript::Begin()
{
	m_RelativePath =  CLevelMgr::GetInst()->GetCurrentLevel()->GetRelativePath();
	if(m_bHome)
		m_BGM = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\bgm\\Home.mp3", L"Sound\\bgm\\Home.mp3");
	else if(m_StageRound == 1 && !m_bLevelClear)
		m_BGM = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\bgm\\Stage1.mp3", L"Sound\\bgm\\Stage1.mp3");
	else if (m_StageRound == 2 && !m_bLevelClear)
		m_BGM = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\bgm\\Stage2.mp3", L"Sound\\bgm\\Stage2.mp3");
	else if (m_StageRound == 3 && !m_bLevelClear)
		m_BGM = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\bgm\\Stage3.mp3", L"Sound\\bgm\\Stage3.mp3");

	// 다시하기 를 위한 임시 레벨을 저장한다.
	SaveTempLevel();


	if (nullptr == m_BGM)
		return;

	if(m_IsStart)
		m_CurBGMIdx = m_BGM->Play(0, 0.4f, false);

}

void CLevelScript::Tick()
{

}

void CLevelScript::BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CLevelScript::Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CLevelScript::EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CLevelScript::SaveComponent(FILE* _File)
{
	fwrite(&m_CurFloor, sizeof(int), 1, _File);
	fwrite(&m_IsStart, sizeof(bool), 1, _File);
	fwrite(&m_IsFinal, sizeof(bool), 1, _File);
	fwrite(&m_bHome,sizeof(bool), 1, _File);
	SaveWString(m_NextLevelPath, _File);
	SaveWString(m_PrevLevelPath, _File);
}

void CLevelScript::LoadComponent(FILE* _File)
{
	fread(&m_CurFloor, sizeof(int), 1, _File);
	fread(&m_IsStart, sizeof(bool), 1, _File);
	fread(&m_IsFinal, sizeof(bool), 1, _File);
	fread(&m_bHome, sizeof(bool), 1, _File);
	LoadWString(m_NextLevelPath, _File);
	LoadWString(m_PrevLevelPath, _File);
}


void CLevelScript::CheckFloorClear()
{
	vector<CGameObject*> vecEnemy = CLevelMgr::GetInst()->GetCurrentLevel()->GetLayer(3)->GetParentObjects();
	vector<CGameObject*> vecLeftEnemy = {};

	for (UINT i = 0; i < vecEnemy.size(); ++i)
	{
		if (vecEnemy[i]->GetName() == L"Enemy" || vecEnemy[i]->GetName() == L"FatEnemy" || vecEnemy[i]->GetName() == L"Biker")
		{
			vecLeftEnemy.push_back(vecEnemy[i]);
		}
	}


	if (vecLeftEnemy.size() == 0)
	{
		m_bFloorClear = true;                        
		if (m_IsFinal)
		{
			m_bLevelClear = true;
			Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\etc\\Complete.wav", L"Sound\\etc\\Complete.wav");
			pSound->Play(1, 0.5f, true);

			// 브금을 클리어 브금으로 바꿔준다.			
			if (m_BGM == nullptr)
				return;
			m_BGM->Stop();
			//m_BGM->RemoveChannel(m_BGM->GetCurrentChannel());
			m_BGM = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\bgm\\StageClear.mp3", L"Sound\\bgm\\StageClear.mp3");
			m_CurBGMIdx = m_BGM->Play(0, 0.4f, false, 30000);
		}

		// 스테이지 클리어 UI 생성
		if (!m_bAlreadyClear)
		{
			Ptr<CPrefab> ClearUIPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\StageClearUI.pref", L"Prefab\\StageClearUI.pref");
			Instantiate(ClearUIPrefab, Vec3(0.f, 0.f, 0.f), 10);
		}
	}
}

void CLevelScript::SaveTempLevel()
{
	CPlayerScript* pPlayerScript = (CPlayerScript*)CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScripts()[0];
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	wstring ContentPath = CPathMgr::GetInst()->GetContentPath();

	// 현재 플레이어의 상태를 저장한다.
	m_TempInfo.CurMask = pPlayerScript->GetMask();
	m_TempInfo.CurWeapon = pPlayerScript->GetPlayerWeaponType();
	m_TempInfo.EquippedGun = pPlayerScript->IsEquippedGun();
	m_TempInfo.CurWeaponRounds = pPlayerScript->GetCurRounds();

	// 레벨 저장
	pCurLevel->g_LevelSave(ContentPath + L"ProgressLevel\\" + L"Temp.lv", pCurLevel);
}
