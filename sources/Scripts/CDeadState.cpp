#include "pch.h"
#include "CDeadState.h"

#include <Engine/CLevelMgr.h>
#include "CEnemyScript.h"
#include "CFatEnemyScript.h"
#include "CPlayerScript.h"

CDeadState::CDeadState()
	: CState(STATE_TYPE::DEADSTATE)
{
}

CDeadState::~CDeadState()
{
}

void CDeadState::Enter()
{
	CGameObject* pPlayer = CLevelMgr::GetInst()->FindObjectByName(L"Player");
	CPlayerScript* pScript = (CPlayerScript*)pPlayer->GetScripts()[0];
	WEAPON_TYPE PlayerWeapon = pScript->GetPlayerWeaponType();
 
	float PlayerTheta = pPlayer->Transform()->GetRelativeRotation().z * (180.f / XM_PI);
	float OwnerTheta = GetOwner()->GetOwner()->Transform()->GetRelativeRotation().z * (180.f / XM_PI);

	if (PlayerTheta > 180.f)
		PlayerTheta -= 360.f;
	if (PlayerTheta < -180.f)
		PlayerTheta += 360.f;

	if (OwnerTheta > 180.f)
		OwnerTheta -= 360.f;
	if (OwnerTheta < -180.f)
		OwnerTheta += 360.f;

	float angleDiff = PlayerTheta - OwnerTheta;

	int FlipIdx = 0;
	switch (PlayerWeapon)
	{
	case WEAPON_TYPE::NONE:
		FlipIdx = 5;
		break;
	case WEAPON_TYPE::KNIFE:
		FlipIdx = 4;
		break;
	case WEAPON_TYPE::LONG_PIPE:
	case WEAPON_TYPE::BAT:
		FlipIdx = 1;
		break;
	case WEAPON_TYPE::KATANA:
	case WEAPON_TYPE::AXE:
		FlipIdx = 0;
		break;
	case WEAPON_TYPE::M16:
	case WEAPON_TYPE::SMG:
		FlipIdx = 2;
		break;
	case WEAPON_TYPE::DOUBLE_BARREL:
	case WEAPON_TYPE::PUMP_SHOTGUN:
		if (-60.f <= angleDiff && angleDiff <= 60.f)
			FlipIdx = 6;
		else
			FlipIdx = 3;
		break;
	default:
		break;
	}

	float Theta = GetThetaWithPlayer();

	Vec3 vRot = m_Owner->GetOwner()->Transform()->GetRelativeRotation();
	vRot.z = Theta;
	m_Owner->GetOwner()->Transform()->SetRelativeRotation(vRot);

	if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
	{
		CEnemyScript* pScript = dynamic_cast<CEnemyScript*>(m_Owner);
		if (pScript->IsDeadByDoor())
			FlipIdx = 5;
		pScript->PlayDeadFlip(FlipIdx);
	}
}

void CDeadState::FinalTick()
{
}

void CDeadState::Exit()
{
}