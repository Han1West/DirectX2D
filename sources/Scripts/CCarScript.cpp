#include "pch.h"
#include "CCarScript.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CPathMgr.h>
#include <Engine/CSound.h>

#include "CPlayerScript.h"
#include "CLevelScript.h"

CCarScript::CCarScript()
	: CScript(SCRIPT_TYPE::CARSCRIPT)
	, m_PlayerRide(false)
	, m_AccTime(0.f)
	, m_bLevelClear(false)
	, m_bCloseSound(false)
	, m_bEngineSound(false)
{
}

CCarScript::~CCarScript()
{
}

void CCarScript::Begin()
{
	GetOwner()->FlipbookPlayer()->Play(0, 1, false);
	CLevelScript* pScript = (CLevelScript*)CLevelMgr::GetInst()->FindObjectByName(L"LevelInfo")->GetScripts()[0];

	// 집이라면 바로 다음 라운드로 넘어 갈 수 있다.
	if (pScript->IsHome())
		m_bLevelClear = true;
}

void CCarScript::Tick()
{
	CPlayerScript* pPlayerScript = (CPlayerScript*)CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScripts()[0];
	if (pPlayerScript->IsPaued())
		return;

	CLevelScript* pScript = (CLevelScript*)CLevelMgr::GetInst()->FindObjectByName(L"LevelInfo")->GetScripts()[0];
	if(!pScript->IsHome())
		m_bLevelClear = pScript->IsLevelClear();

	if (m_PlayerRide)
	{
		m_AccTime += DT;
		if (m_AccTime < 0.5f)
		{
			// 소리 재생
			if (!m_bCloseSound)
			{
				Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\etc\\CarClose.wav", L"Sound\\etc\\CarClose.wav");
				pSound->Play(1, 0.5f, false);
				m_bCloseSound = true;
			}

			GetOwner()->FlipbookPlayer()->Play(2, 3, false);
		}
			
		if (0.5f < m_AccTime)
		{
			// 소리 재생
			if (!m_bEngineSound)
			{
				Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\etc\\CarEngine.wav", L"Sound\\etc\\CarEngine.wav");
				pSound->Play(1, 0.5f, false);
				m_bEngineSound = true;
			}

			GetOwner()->FlipbookPlayer()->Play(3, 1, false);
		}

		if (1.f < m_AccTime)
		{
			m_AccTime = 0.f;
			ChangeStage();
		}
			
	}

}

void CCarScript::ChangeStage()
{
	CLevelScript* pScript = (CLevelScript*)CLevelMgr::GetInst()->FindObjectByName(L"LevelInfo")->GetScripts()[0];
	CPlayerScript* pPlayerScript = (CPlayerScript*)CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScripts()[0];
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();

	wstring ContentPath = CPathMgr::GetInst()->GetContentPath();
	bool bHome = pScript->IsHome();
	int Rounds = pScript->GetStageRound();
	wstring strRounds = to_wstring(Rounds + 1);

	wstring NextRound = L"Level\\Stage" + strRounds + L"_Floor1.lv";

	ChangeLevelState(LEVEL_STATE::STOP);

	// 현재 브금을 멈춘다.
	Ptr<CSound> CurBgm = pScript->GetBGM();
	CurBgm->Stop();

	// 집이라면 다음 스테이지로 넘어간다.
	if (bHome)
	{
		// 다음 레벨을 불러온다.
		CLevel* pLoadedLevel = pCurLevel->g_LevelLoad(ContentPath + NextRound);

		vector<CGameObject*> pNextObjVector = pLoadedLevel->GetLayer(0)->GetParentObjects();
		vector<CGameObject*> pNextPlayervector = pLoadedLevel->GetLayer(2)->GetParentObjects();

		// 다음 레벨의 Rounds 설정
		for (UINT i = 0; i < pNextObjVector.size(); ++i)
		{
			if (pNextObjVector[i]->GetName() == L"LevelInfo")
			{
				CLevelScript* NextLevelScript = (CLevelScript*)pNextObjVector[i]->GetScripts()[0];
				NextLevelScript->SetStageRound(Rounds + 1);
			}
		}

		// 다음 레벨의 Player 설정
		for (UINT i = 0; i < pNextPlayervector.size(); ++i)
		{
			if (pNextObjVector[i]->GetName() == L"Player")
			{
				CPlayerScript* NextPlayerScript = (CPlayerScript*)pNextObjVector[i]->GetScripts()[0];
				NextPlayerScript->SetActionKey(pPlayerScript->GetCurrentActionedKey());
			}
		}

		ChangeLevel(pLoadedLevel, LEVEL_STATE::PLAY);
	}

	// 아니라면 집으로 돌아간다.
	else
	{

		CLevel* pLoadedLevel = nullptr;
		// 다음 레벨을 불러온다.
		if (Rounds == 3)
		{
			pLoadedLevel = pCurLevel->g_LevelLoad(ContentPath + L"Level\\End.lv");
		}
		else
		{
			pLoadedLevel = pCurLevel->g_LevelLoad(ContentPath + L"Level\\PlayerHouse_Floor2.lv");
		}
		
		

		vector<CGameObject*> pNextObjVector = pLoadedLevel->GetLayer(0)->GetParentObjects();
		vector<CGameObject*> pNextPlayervector = pLoadedLevel->GetLayer(2)->GetParentObjects();

		// 다음 레벨의 Rounds 설정
		for (UINT i = 0; i < pNextObjVector.size(); ++i)
		{
			if (pNextObjVector[i]->GetName() == L"LevelInfo")
			{
				CLevelScript* NextLevelScript = (CLevelScript*)pNextObjVector[i]->GetScripts()[0];
				NextLevelScript->SetStageRound(Rounds);
			}
		}

		// 다음 레벨의 Player 설정
		for (UINT i = 0; i < pNextPlayervector.size(); ++i)
		{
			if (pNextObjVector[i]->GetName() == L"Player")
			{
				CPlayerScript* NextPlayerScript = (CPlayerScript*)pNextObjVector[i]->GetScripts()[0];
				NextPlayerScript->SetActionKey(pPlayerScript->GetCurrentActionedKey());
			}
		}

		ChangeLevel(pLoadedLevel, LEVEL_STATE::PLAY);
	}
}

void CCarScript::BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	// 차문이 열린다
	if (_OtherCollider->GetName() == L"Player" && m_bLevelClear)
	{
		// 소리 재생
		Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\etc\\CarOpen.wav", L"Sound\\etc\\CarOpen.wav");
		pSound->Play(1, 0.5f, false);
		GetOwner()->FlipbookPlayer()->Play(1, 3, false);
	}
}

void CCarScript::Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	// 차문이 닫힌다
	if (_OtherCollider->GetName() == L"Player" && m_bLevelClear)
	{

		Vec3 vPlayerPos = _OtherObject->Transform()->GetRelativePos();
		Vec3 vCarPos = Transform()->GetRelativePos();
		Vec3 vCarRot = Transform()->GetRelativeRotation();

		if (vCarRot.z == 0.f)
		{
			if (vCarPos.y - 30.f < vPlayerPos.y && vPlayerPos.y < vCarPos.y)
			{
				m_PlayerRide = true;
			}
		}
		else
		{
			if (vPlayerPos.x < vCarPos.x + 30.f && vCarPos.x < vPlayerPos.x)
			{
				m_PlayerRide = true;
			}
		}		
	}
}

void CCarScript::EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	// 차문이 닫힌다.
	if (_OtherCollider->GetName() == L"Player" && m_bLevelClear)
	{
		// 소리 재생
		Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\etc\\CarClose.wav", L"Sound\\etc\\CarClose.wav");
		pSound->Play(1, 0.5f, false);
		GetOwner()->FlipbookPlayer()->Play(2, 3, false);
	}
}

void CCarScript::SaveComponent(FILE* _File)
{
}

void CCarScript::LoadComponent(FILE* _File)
{
}
