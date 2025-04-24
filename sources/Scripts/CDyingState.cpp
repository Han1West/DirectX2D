#include "pch.h"
#include "CDyingState.h"

#include <Engine/CLevelMgr.h>

#include "CEnemyScript.h"
#include "CFatEnemyScript.h"
#include "CPlayerScript.h"

CDyingState::CDyingState()
	: CState(STATE_TYPE::DYINGSTATE)
{

}

CDyingState::~CDyingState()
{

}

void CDyingState::Enter()
{
	CGameObject* pPlayer = CLevelMgr::GetInst()->FindObjectByName(L"Player");
	CPlayerScript* pScript = (CPlayerScript*)pPlayer->GetScripts()[0];
	WEAPON_TYPE PlayerWeapon = pScript->GetPlayerWeaponType();
	int KillingCount = pScript->GetCurrentKillingCount();

	int FlipIdx = 0;

	switch (PlayerWeapon)
	{
	case WEAPON_TYPE::LONG_PIPE:
	case WEAPON_TYPE::BAT:
		if (KillingCount == 2)
			FlipIdx = 2;
		if (KillingCount == 1)
			FlipIdx = 3;
		if (KillingCount == 0)
			FlipIdx = 4;
		break;
	case WEAPON_TYPE::KATANA:
		FlipIdx = 0;
		break;
	case WEAPON_TYPE::AXE:
		FlipIdx = 1;
		break;
	}


	float Theta = pPlayer->Transform()->GetRelativeRotation().z;

	Vec3 vRot = m_Owner->GetOwner()->Transform()->GetRelativeRotation();
	vRot.z = Theta;
	m_Owner->GetOwner()->Transform()->SetRelativeRotation(vRot);

	if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
	{
		CEnemyScript* pScript = dynamic_cast<CEnemyScript*>(m_Owner);
		pScript->PlayDyingFlip(FlipIdx);
	}
	if (SCRIPT_TYPE::FATENEMYSCRIPT == m_Owner->GetScriptType())
	{
		vRot.z = Theta - 160.f;
		m_Owner->GetOwner()->Transform()->SetRelativeRotation(vRot);
		CFatEnemyScript* pScript = dynamic_cast<CFatEnemyScript*>(m_Owner);
		pScript->PlayDyingFlip();
	}
}

void CDyingState::FinalTick()
{
}

void CDyingState::Exit()
{
	CGameObject* pPlayer = CLevelMgr::GetInst()->FindObjectByName(L"Player");
	CPlayerScript* pScript = (CPlayerScript*)pPlayer->GetScripts()[0];
	WEAPON_TYPE PlayerWeapon = pScript->GetPlayerWeaponType();
	int KillingCount = pScript->GetCurrentKillingCount();

	int FlipIdx = 0;

	switch (PlayerWeapon)
	{
	case WEAPON_TYPE::NONE:
		FlipIdx = 2;
		break;
	case WEAPON_TYPE::M16:
	case WEAPON_TYPE::SMG:
		FlipIdx = 1;
		break;
	case WEAPON_TYPE::DOUBLE_BARREL:
	case WEAPON_TYPE::PUMP_SHOTGUN:
		FlipIdx = 0;
		break;
	}

	if (SCRIPT_TYPE::FATENEMYSCRIPT == m_Owner->GetScriptType())
	{
		CFatEnemyScript* pScript = dynamic_cast<CFatEnemyScript*>(m_Owner);
		pScript->PlayDeadFlip(FlipIdx);
	}
}


