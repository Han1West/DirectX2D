#include "pch.h"
#include "CReturnState.h"

#include "CEnemyScript.h"
#include "CFatEnemyScript.h"
#include "CPlayerScript.h"

#include <Engine/CLevelMgr.h>

CReturnState::CReturnState()
	: CState(STATE_TYPE::RETURNSTATE)
	, m_DetectRange(600.f)
	, m_Speed(300.f)
	, m_DestPos()
	, m_lPath{}
	, m_iter()
{
}

CReturnState::~CReturnState()
{
}


void CReturnState::Enter()
{
	Vec3 vEnemyPos = m_Owner->GetOwner()->Transform()->GetRelativePos();

	if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
	{
		CEnemyScript* pScript = dynamic_cast<CEnemyScript*>(m_Owner);
		pScript->PlayWalkFlip();
		m_DestPos = pScript->GetSpawnPos();
	}
	if (SCRIPT_TYPE::FATENEMYSCRIPT == m_Owner->GetScriptType())
	{
		CFatEnemyScript* pScript = dynamic_cast<CFatEnemyScript*>(m_Owner);
		pScript->PlayWalkFlip();
		m_DestPos = pScript->GetSpawnPos();
	}

	m_lPath = FindPath(vEnemyPos, m_DestPos);
	m_iter = m_lPath.begin();
}

void CReturnState::FinalTick()
{

	Vec3 Destination = {};

	// 현재 플레이어의 위치
	CGameObject* pPlayer = CLevelMgr::GetInst()->FindObjectByName(L"Player");
	Vec3 vPlayerPos = pPlayer->Transform()->GetRelativePos();
	Vec3 vEnemyPos = GetOwner()->GetOwner()->Transform()->GetRelativePos();
	Vec3 vEnemyRot = GetOwner()->GetOwner()->Transform()->GetRelativeRotation();

	float Distance = vPlayerPos.Distance(vPlayerPos, vEnemyPos);
	
	if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
	{
		CEnemyScript* pScript = (CEnemyScript*)m_Owner;

		// 만약 현재 무기가 없다면(KnockOut에서 들어옴) -> 무기를 우선으로 찾는다.
		if (pScript->GetWeaponType() == WEAPON_TYPE::NONE)
		{
			CGameObject* DestWeapon = FindWeaponAround(vEnemyPos);

			// 주변에 무기가 없다
			if (nullptr == DestWeapon)
			{
				if (pScript->IsGivenUP())
					return;
				pScript->PlaySpecialFlip(4);
				pScript->SetGiveup(true);
				return;
			}

			Vec3 WeaponPos = DestWeapon->Transform()->GetRelativePos();

			Vec2 Dir = Vec2(WeaponPos.x - vEnemyPos.x, WeaponPos.y - vEnemyPos.y);
			Dir.Normalize();

			// 무기와 겹쳤다.
			if ((WeaponPos.x - 10.f < vEnemyPos.x && vEnemyPos.x < WeaponPos.x + 10.f)
				&& (WeaponPos.y - 10.f < vEnemyPos.y && vEnemyPos.y < WeaponPos.y + 10.f))
			{
				pScript->EquipWeapon(DestWeapon);
			}
			else
			{
				vEnemyPos.x += m_Speed * Dir.x * DT;
				vEnemyPos.y += m_Speed * Dir.y * DT;

				vEnemyRot.z = atan2(Dir.y, Dir.x);

				m_Owner->GetOwner()->Transform()->SetRelativePos(vEnemyPos);
				m_Owner->GetOwner()->Transform()->SetRelativeRotation(vEnemyRot);
			}
		}
		else
		{
			// 자신의 원래 자리로 돌아간다.
			if (m_lPath.empty() || m_iter == prev(m_lPath.end()))
			{
				// 리스폰시 위치와 상태로 돌아간다.
				if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
				{
					CEnemyScript* pScript = (CEnemyScript*)m_Owner;
					pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), CStateMgr::GetStateName(pScript->GetStartState())));
				}
				else if (SCRIPT_TYPE::FATENEMYSCRIPT == m_Owner->GetScriptType())
				{
					CFatEnemyScript* pScript = (CFatEnemyScript*)m_Owner;
					pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), CStateMgr::GetStateName(pScript->GetStartState())));
				}
				return;
			}

			Vec2 vDestPos = *m_iter;
			// 목적지로 이동
			MoveToward(GetOwner()->GetOwner(), vDestPos, m_Speed);

			//GetOwner()->GetOwner()->Transform()->SetRelativePos(vEnemyPos);
			Vec3 vNewEnemyPos = GetOwner()->GetOwner()->Transform()->GetRelativePos();

			// 목적지에 도착하면 다음 목적지로 갱신
			if ((vDestPos.x - 5.f < vEnemyPos.x && vEnemyPos.x < vDestPos.x + 5.f)
				&& (vDestPos.y - 5.f < vEnemyPos.y && vEnemyPos.y < vDestPos.y + 5.f))
				++m_iter;
			// 플레이어가 탐지 범위 내에 들어왔다.
			if (Distance < m_DetectRange)
			{
				// 만약 적과 플레이어 사이에 벽이 있다면 탐지를 못한다.
				if (!FindWall(vPlayerPos, vEnemyPos))
				{
					// 플레이어를 추적한다
					if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
					{
						CEnemyScript* pScript = (CEnemyScript*)m_Owner;
						pScript->SetDestination(vPlayerPos);
						if (pScript->IsEquippedGun())
							pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CAttackState"));
						else
							pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CFindState"));
					}
					else if (SCRIPT_TYPE::FATENEMYSCRIPT == m_Owner->GetScriptType())
					{
						CFatEnemyScript* pScript = (CFatEnemyScript*)m_Owner;
						pScript->SetDestination(vPlayerPos);
						pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CFindState"));
					}
				}
				else
				{
					CPlayerScript* pPlayerScript = (CPlayerScript*)pPlayer->GetScripts()[0];
					if (pPlayerScript->IsMakeGunSound())
					{
						// 플레이어를 추적한다
						if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
						{
							CEnemyScript* pScript = (CEnemyScript*)m_Owner;
							pScript->SetDestination(vPlayerPos);
							pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CTraceState"));

						}
						else if (SCRIPT_TYPE::FATENEMYSCRIPT == m_Owner->GetScriptType())
						{
							CFatEnemyScript* pScript = (CFatEnemyScript*)m_Owner;
							pScript->SetDestination(vPlayerPos);
							pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CTraceState"));
						}
						return;
					}
				}
			}
		}
	}


	else
	{
		// 자신의 원래 자리로 돌아간다.
		if (m_lPath.empty() || m_iter == prev(m_lPath.end()))
		{
			// 리스폰시 위치와 상태로 돌아간다.
			if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
			{
				CEnemyScript* pScript = (CEnemyScript*)m_Owner;
				pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), CStateMgr::GetStateName(pScript->GetStartState())));
			}
			else if (SCRIPT_TYPE::FATENEMYSCRIPT == m_Owner->GetScriptType())
			{
				CFatEnemyScript* pScript = (CFatEnemyScript*)m_Owner;
				pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), CStateMgr::GetStateName(pScript->GetStartState())));
			}
			return;
		}

		Vec2 vDestPos = *m_iter;
		// 목적지로 이동
		MoveToward(GetOwner()->GetOwner(), vDestPos, m_Speed);

		//GetOwner()->GetOwner()->Transform()->SetRelativePos(vEnemyPos);
		Vec3 vNewEnemyPos = GetOwner()->GetOwner()->Transform()->GetRelativePos();

		// 목적지에 도착하면 다음 목적지로 갱신
		if ((vDestPos.x - 5.f < vEnemyPos.x && vEnemyPos.x < vDestPos.x + 5.f)
			&& (vDestPos.y - 5.f < vEnemyPos.y && vEnemyPos.y < vDestPos.y + 5.f))
			++m_iter;

		// 플레이어가 탐지 범위 내에 들어왔다.
		if (Distance < m_DetectRange)
		{
			// 만약 적과 플레이어 사이에 벽이 있다면 탐지를 못한다.
			if (!FindWall(vPlayerPos, vEnemyPos))
			{
				// 플레이어를 추적한다
				if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
				{
					CEnemyScript* pScript = (CEnemyScript*)m_Owner;
					pScript->SetDestination(vPlayerPos);
					if (pScript->IsEquippedGun())
						pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CAttackState"));
					else
						pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CFindState"));
				}
				else if (SCRIPT_TYPE::FATENEMYSCRIPT == m_Owner->GetScriptType())
				{
					CFatEnemyScript* pScript = (CFatEnemyScript*)m_Owner;
					pScript->SetDestination(vPlayerPos);
					pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CFindState"));
				}
			}
			else
			{
				CPlayerScript* pPlayerScript = (CPlayerScript*)pPlayer->GetScripts()[0];
				if (pPlayerScript->IsMakeGunSound())
				{
					// 플레이어를 추적한다
					if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
					{
						CEnemyScript* pScript = (CEnemyScript*)m_Owner;
						pScript->SetDestination(vPlayerPos);
						pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CTraceState"));

					}
					else if (SCRIPT_TYPE::FATENEMYSCRIPT == m_Owner->GetScriptType())
					{
						CFatEnemyScript* pScript = (CFatEnemyScript*)m_Owner;
						pScript->SetDestination(vPlayerPos);
						pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CTraceState"));
					}
					return;
				}
			}
		}
	}
}

void CReturnState::Exit()
{
}
