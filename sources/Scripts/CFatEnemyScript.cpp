#include "pch.h"
#include "CFatEnemyScript.h"

#include <Engine/CLevelMgr.h>

#include "CWeaponScript.h"
#include "CMeleeAttackScript.h"
#include "CPlayerScript.h"
#include "CBulletScript.h"
#include "CBloodScript.h"
#include "CCameraScript.h"
#include "CLevelScript.h"

#include "CState.h"
#include "CStateMgr.h"

CFatEnemyScript::CFatEnemyScript()
	: CScript(SCRIPT_TYPE::FATENEMYSCRIPT)
	, m_CurState(nullptr)
	, m_AccTime(0.f)
	, m_BleedTime(0.f)
	, m_SpawnTime(0.f)
	, m_HP(20)
	, m_Bleeding(false)
	, m_DeadBleeding(true)
	, m_MakePool(true)
	, m_bDead(false)
	, m_SpawnPos()
	, m_DestPos()
	, m_StartState(STATE_TYPE::NONE)
{
}

CFatEnemyScript::~CFatEnemyScript()
{
	if (m_CurState)
	{
		delete m_CurState;
		m_CurState = nullptr;
	}
}

void CFatEnemyScript::Begin()
{
	if (m_CurState)
	{
		m_CurState->SetOwner(this);
		m_CurState->Enter();
		m_StartState = m_CurState->GetStateType();
	}

	m_SpawnPos = Transform()->GetRelativePos();
}

void CFatEnemyScript::Tick()
{
	CPlayerScript* pPlayerScript = (CPlayerScript*)CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScripts()[0];
	if (pPlayerScript->IsPaued())
		return;

	if (nullptr != m_CurState)
		m_CurState->FinalTick();

	if (m_CurState->GetStateType() == STATE_TYPE::DYINGSTATE)
	{
		m_AccTime += DT;

		if (1.f < m_AccTime)
		{
			m_CurState = CStateMgr::ChangeState(m_CurState, L"CDeadState");
			m_AccTime = 0.f;
		}
	}

	if (m_Bleeding)
	{
		m_BleedTime += DT;
		if (0.2 < m_BleedTime)
			m_HP -= 1;

		if (m_HP <= 0)
		{
			m_CurState = CStateMgr::ChangeState(m_CurState, L"CDyingState");
			m_Bleeding = false;
			m_BleedTime = 0.f;
		}
	}

	if (m_CurState->GetStateType() == STATE_TYPE::DEADSTATE)
	{
		CLevelScript* pScript = (CLevelScript*)CLevelMgr::GetInst()->FindObjectByName(L"LevelInfo")->GetScripts()[0];
		if (m_DeadBleeding)
		{
			if (!pScript->IsAlreadyClear())
			{
				MakeBlood();
			}
		}
		m_SpawnTime += DT;

		if (1.f < m_SpawnTime)
		{
			if (!m_bDead)
			{
				if (0.3f < m_SpawnTime)
					m_bDead = true;
			}
			if (m_MakePool)
			{
				GetOwner()->SetName(L"DeadBody");				
				if (!pScript->IsAlreadyClear())
				{
					pScript->CheckFloorClear();
					MakePool();
				}

				m_MakePool = false;
			}
		}
	}
}

void CFatEnemyScript::BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	if (m_CurState == nullptr)
		return;

	if (m_CurState->GetStateType() == STATE_TYPE::DEADSTATE
		|| m_CurState->GetStateType() == STATE_TYPE::DYINGSTATE)
	{
		return;
	}

	if (_OtherCollider->GetName() == L"Bullet")
	{
		CPlayerScript* pScript = (CPlayerScript*)CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScripts()[0];

		if(pScript->GetPlayerWeaponType() == WEAPON_TYPE::PUMP_SHOTGUN || pScript->GetPlayerWeaponType() == WEAPON_TYPE::DOUBLE_BARREL)
			m_CurState = CStateMgr::ChangeState(m_CurState, L"CDyingState");
		else
		{
			m_HP -= 1;
			m_Bleeding = true;
		}
	}

	if (_OtherCollider->GetName() == L"MeleeAttack")
	{
		CMeleeAttackScript* pScript = (CMeleeAttackScript*)_OtherObject->GetScripts()[0];
		WEAPON_TYPE Type = pScript->GetCurWeapon();
		CPlayerScript* pPlayerScript = (CPlayerScript*)CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScripts()[0];
		CCameraScript* pCameraScript = (CCameraScript*)CLevelMgr::GetInst()->FindObjectByName(L"MainCamera")->GetScripts()[0];
		
		// ¸ÇÁÖ¸Ô
		if (Type == WEAPON_TYPE::NONE && pPlayerScript->GetMask() == MASK_TYPE::TONY_TIGER)
		{
			// ¼Ò¸® Àç»ý
			Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\attack\\Hit.wav", L"Sound\\attack\\Hit.wav");
			pSound->Play(1, 0.5f, false);
			m_CurState = CStateMgr::ChangeState(m_CurState, L"CDyingState");
			pCameraScript->SetShake(true);
		}
	}
}

void CFatEnemyScript::Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CFatEnemyScript::EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CFatEnemyScript::SaveComponent(FILE* _File)
{
	CStateMgr::SaveState(m_CurState, _File);
}

void CFatEnemyScript::LoadComponent(FILE* _File)
{
	CStateMgr::LoadState(m_CurState, _File);
}

void CFatEnemyScript::MakeBlood()
{
	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 EnemyPos = Transform()->GetRelativePos();
	float SpawnTheta = Transform()->GetRelativeRotation().z;

	Vec2 Dir = Vec2(cos(SpawnTheta), sin(SpawnTheta));
	Dir.Normalize();

	Vec3 SpawnPos = Vec3(EnemyPos.x + Dir.x * 10.f, EnemyPos.y + Dir.y * 10.f, 0.f);

	Ptr<CPrefab> BloodSquirtPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\SquirtBlood.pref", L"Prefab\\SquirtBlood.pref");
	CBloodScript* pScript = (CBloodScript*)BloodSquirtPrefab->GetProtoObject()->GetScripts()[0];
	pScript->SetSpread(true);
	pScript->SetBase(true);
	pScript->SetDir(Dir);

	Instantiate(BloodSquirtPrefab, vPos, 3);
	m_DeadBleeding = false;
}

void CFatEnemyScript::MakePool()
{
	Vec3 vPos = Transform()->GetRelativePos();

	Ptr<CPrefab> BloodPPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\BloodPool.pref", L"Prefab\\BloodPool.pref");
	BloodPPrefab->GetProtoObject()->FlipbookPlayer()->Play(0, 40, false);

	Instantiate(BloodPPrefab, vPos, 3);
}


void CFatEnemyScript::PlayIdleFlip()
{
	GetOwner()->FlipbookPlayer()->Play(6, 10, false);
}

void CFatEnemyScript::PlayWalkFlip()
{
	GetOwner()->FlipbookPlayer()->Play(8, 10, true);
}

void CFatEnemyScript::PlayAttackFlip()
{
	GetOwner()->FlipbookPlayer()->Play(0, 10, false);
}

void CFatEnemyScript::PlaySearchFlip()
{
	GetOwner()->FlipbookPlayer()->Play(7, 10, true);
}

void CFatEnemyScript::PlayDyingFlip()
{
	GetOwner()->FlipbookPlayer()->Play(5, 10, false);
}

void CFatEnemyScript::PlayDeadFlip(int _Idx)
{
	switch (_Idx)
	{
	case 0:
		// ¼¦°Ç
		GetOwner()->FlipbookPlayer()->Play(2, 10, false);
		break;
	case 1:
		// ¿¬¹ßÃÑ
		GetOwner()->FlipbookPlayer()->Play(4, 10, false);
		break;
	case 2:
		// Åä´Ï
		GetOwner()->FlipbookPlayer()->Play(1, 10, false);
		break;
	}

}
