#include "pch.h"
#include "CUIScript.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CConstBuffer.h>
#include <Engine/CDevice.h>

#include "CPlayerScript.h"
#include "CSelectMaskScript.h"
#include "CLevelScript.h"
#include "CMainMenuScript.h"

CUIScript::CUIScript()
	: CScript(SCRIPT_TYPE::UISCRIPT)
	, m_AccTime(0.f)
	, m_ClearAccTime(0.f)
{
}

CUIScript::~CUIScript()
{
}

void CUIScript::Begin()
{
	m_UIScale = Transform()->GetRelativeScale();
}

void CUIScript::Tick()
{
	if (GetOwner()->GetName() == L"StageClearUI")
	{
		m_ClearAccTime += DT;

		if (2.f < m_ClearAccTime)
			DestroyObject(GetOwner());
	}
	else if (GetOwner()->GetName() == L"ArrowUI")
	{
		SetActiveArrowUI();
	}
	else if (GetOwner()->GetName() == L"GameStartUI" || GetOwner()->GetName() == L"ExitUI")
	{
		MainMenuUI();
	}
	else
	{
		if (GetOwner()->GetName() == L"RoundUI")
		{
			SetCurNumFlip();
		}
		if (GetOwner()->GetName() == L"RndsUI")
		{
			SetActive();
		}
		if (GetOwner()->GetName() == L"MaskUI")
		{
			SetCurMaskFlip();
		}
		if (GetOwner()->GetName() == L"RestartUI" || GetOwner()->GetName() == L"DeadUI")
		{
			SetDeadActive();
		}
		if (GetOwner()->GetName() == L"RTUIBackground")
		{
			SetDeadBackgroundActive();
			return;
		}
		if (GetOwner()->GetName() == L"GotoCarUI")
		{
			SetClearUI();			
		}
		

		Vec3 vRot = Transform()->GetRelativeRotation();

		float OscillationSpeed = 2.f;
		float OscillationAmplitued = 0.2f;

		m_AccTime += DT;

		vRot.z = OscillationAmplitued * sin(OscillationSpeed * m_AccTime);

		Transform()->SetRelativeRotation(vRot);
	}
}

void CUIScript::BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CUIScript::Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CUIScript::EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CUIScript::SaveComponent(FILE* _File)
{
}

void CUIScript::LoadComponent(FILE* _File)
{
}

void CUIScript::SetCurNumFlip()
{
	CPlayerScript* pPlayerScript = (CPlayerScript*)CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScripts()[0];

	bool bEquipGun = pPlayerScript->IsEquippedGun();
	bool bDead = pPlayerScript->IsPlayerDead();

	if (bDead)
	{
		GetOwner()->Transform()->SetRelativeScale(Vec3(0.f, 0.f, 0.f));
	}
	else
	{
		if (!bEquipGun)
		{
			GetOwner()->FlipbookPlayer()->Play(32, 1, false);
			GetOwner()->Transform()->SetRelativeScale(Vec3(230.f, 230.f, 0.f));
			GetOwner()->Transform()->SetRelativePos(Vec3(-456.f, -290.f, 0.f));
		}
		else
		{
			int CurRounds = pPlayerScript->GetCurRounds();
			if (CurRounds == 0)
			{
				GetOwner()->FlipbookPlayer()->Play(33, 1, false);
				GetOwner()->Transform()->SetRelativeScale(Vec3(230.f, 230.f, 0.f));
				GetOwner()->Transform()->SetRelativePos(Vec3(-456.f, -290.f, 0.f));
			}
			else
			{
				GetOwner()->FlipbookPlayer()->Play(CurRounds - 1, 1, false);
				GetOwner()->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 0.f));
				GetOwner()->Transform()->SetRelativePos(Vec3(-544.f, -290.f, 0.f));
			}
		}
	}
}

void CUIScript::SetActive()
{
	CPlayerScript* pPlayerScript = (CPlayerScript*)CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScripts()[0];

	bool bEquipGun = pPlayerScript->IsEquippedGun();
	bool bDead = pPlayerScript->IsPlayerDead();

	if (!bEquipGun || bDead)
		GetOwner()->Transform()->SetRelativeScale(Vec3(0.f, 0.f, 0.f));
	else
	{
		int CurRounds = pPlayerScript->GetCurRounds();

		if (CurRounds == 0)
			GetOwner()->Transform()->SetRelativeScale(Vec3(0.f, 0.f, 0.f));	
		else
			GetOwner()->Transform()->SetRelativeScale(Vec3(190.f, 42.f, 0.f));
		
	}
}

void CUIScript::SetDeadActive()
{
	CPlayerScript* pPlayerScript = (CPlayerScript*)CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScripts()[0];
	bool bDead = pPlayerScript->IsPlayerDead();

	if (bDead)
	{
		GetOwner()->Transform()->SetRelativeScale(Vec3(251, 43.f, 0.f));
	}
}

void CUIScript::SetDeadBackgroundActive()
{
	CPlayerScript* pPlayerScript = (CPlayerScript*)CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScripts()[0];
	CLevelScript* pLevelScript = (CLevelScript*)CLevelMgr::GetInst()->FindObjectByName(L"LevelInfo")->GetScripts()[0];
	
	bool bDead = pPlayerScript->IsPlayerDead();
	bool IsLevelClear = pLevelScript->IsLevelClear();

	if (bDead)
	{
		GetOwner()->Transform()->SetRelativeScale(Vec3(360.f, 100.f, 0.f));
	}
	

	if (IsLevelClear)
	{
		GetOwner()->Transform()->SetRelativeScale(Vec3(360.f, 100.f, 0.f));
	}
}

void CUIScript::SetCurMaskFlip()
{
	CSelectMaskScript* pMaskScript = (CSelectMaskScript*)CLevelMgr::GetInst()->FindObjectByName(L"SelectMask")->GetScripts()[0];

	int CurIdx = pMaskScript->GetCurIndex();

	GetOwner()->FlipbookPlayer()->Play(CurIdx, 1, false);

	
	if (!pMaskScript->IsActive())
	{
		DestroyObject(GetOwner());
	}

}

void CUIScript::SetClearUI()
{
	CLevelScript* pLevelScript = (CLevelScript*)CLevelMgr::GetInst()->FindObjectByName(L"LevelInfo")->GetScripts()[0];
	bool IsLevelClear = pLevelScript->IsLevelClear();

	if (IsLevelClear)
	{
		GetOwner()->Transform()->SetRelativeScale(Vec3(221.f, 42.f, 0.f));
	}

}

void CUIScript::SetActiveArrowUI()
{
	CLevelScript* pLevelScript = (CLevelScript*)CLevelMgr::GetInst()->FindObjectByName(L"LevelInfo")->GetScripts()[0];
	bool IsFloorClear = pLevelScript->IsFloorClear();
	bool IsAlreadyClear = pLevelScript->IsAlreadyClear();

	if (IsFloorClear && !IsAlreadyClear)
	{
		GetOwner()->Transform()->SetRelativeScale(Vec3(50.f, 50.f, 0.f));
	}
	if (IsAlreadyClear)
	{
		GetOwner()->Transform()->SetRelativeScale(Vec3(0.f, 0.f, 0.f));
	}

	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vRot = Transform()->GetRelativeRotation();

	float Theta = vRot.z * (180.f / XM_PI);
	// 각도를 0 ~ 360 사이로 정규화한다.
	Theta = fmod(Theta, 360.f);

	if (Theta < 0)
		Theta += 360.f;

	m_AccTime += DT;

	if (Theta == 0.f || Theta == 180.f)
	{
		vPos.y += 0.005f * sin(m_AccTime * 2.f);
	}
	else
	{
		vPos.x += 0.005f * sin(m_AccTime * 2.f);
	}

	Transform()->SetRelativePos(vPos);
}

void CUIScript::MainMenuUI()
{
	CMainMenuScript* pMenuScript = (CMainMenuScript*)CLevelMgr::GetInst()->FindObjectByName(L"MainMenu")->GetScripts()[0];
	
	int CurIdx = pMenuScript->GetIdx();
	bool bStartSelected = false;
	bool bExitSelected = false;

	if (GetOwner()->GetName() == L"GameStartUI" && CurIdx == 0)
	{
		bStartSelected = true;
		Vec3 vRot = Transform()->GetRelativeRotation();
		Vec3 vScale = Transform()->GetRelativeScale();

		float OscillationSpeed = 2.f;
		float OscillationAmplitued = 0.2f;

		m_AccTime += DT;

		vRot.z = OscillationAmplitued * sin(OscillationSpeed * m_AccTime);

		vScale.x = m_UIScale.x + 100.f;
		vScale.y = m_UIScale.y + 50.f;

		Transform()->SetRelativeRotation(vRot);
		Transform()->SetRelativeScale(vScale);
	}
	else if(GetOwner()->GetName() == L"GameStartUI" && CurIdx == 1)
	{
		Vec3 vRot = Transform()->GetRelativeRotation();
		Vec3 vScale = Transform()->GetRelativeScale();

		vRot.z = 0.f;

		vScale.x = m_UIScale.x;
		vScale.y = m_UIScale.y;

		Transform()->SetRelativeRotation(vRot);
		Transform()->SetRelativeScale(vScale);
	}

	if (GetOwner()->GetName() == L"ExitUI" && CurIdx == 1)
	{
		bExitSelected = true;
		Vec3 vRot = Transform()->GetRelativeRotation();
		Vec3 vScale = Transform()->GetRelativeScale();

		float OscillationSpeed = 2.f;
		float OscillationAmplitued = 0.2f;

		m_AccTime += DT;

		vRot.z = OscillationAmplitued * sin(OscillationSpeed * m_AccTime);

		vScale.x = m_UIScale.x + 100.f;
		vScale.y = m_UIScale.y + 50.f;

		Transform()->SetRelativeRotation(vRot);
		Transform()->SetRelativeScale(vScale);
	}
	else if(GetOwner()->GetName() == L"ExitUI" && CurIdx == 0)
	{
		Vec3 vRot = Transform()->GetRelativeRotation();
		Vec3 vScale = Transform()->GetRelativeScale();

		vRot.z = 0.f;

		vScale.x = m_UIScale.x;
		vScale.y = m_UIScale.y;

		Transform()->SetRelativeRotation(vRot);
		Transform()->SetRelativeScale(vScale);
	}

	BindingSelect(bStartSelected, bExitSelected);
}

void CUIScript::BindingSelect(bool _StartSelect, bool _ExitSelect)
{
	MainUIData MainData = {};

	MainData.bStartSelected = _StartSelect;
	MainData.bExitSelected = _ExitSelect;

	CConstBuffer* pCB = CDevice::GetInst()->GetCB(CB_TYPE::MAIN);

	pCB->SetData(&MainData);
	pCB->Binding();
}
