#include "pch.h"
#include "CIdleState.h"

#include <Engine/CScript.h>
#include <Engine/CLevelMgr.h>

#include "CEnemyScript.h"
#include "CFatEnemyScript.h"
#include "CPlayerScript.h"

CIdleState::CIdleState()
	: CState(STATE_TYPE::IDLESTATE)
	, m_DetectRange(600.f)
{
}

CIdleState::~CIdleState()
{
}


void CIdleState::Enter()
{
	if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
	{
		CEnemyScript* pScript = dynamic_cast<CEnemyScript*>(m_Owner);
 		pScript->PlayIdleFlip();
	}
	if (SCRIPT_TYPE::FATENEMYSCRIPT == m_Owner->GetScriptType())
	{
		CFatEnemyScript* pScript = dynamic_cast<CFatEnemyScript*>(m_Owner);
		pScript->PlayIdleFlip();
	}
}

void CIdleState::FinalTick()
{
	// 현재 플레이어의 위치	
	CGameObject* pPlayer = CLevelMgr::GetInst()->FindObjectByName(L"Player");
	Vec3 vPlayerPos = pPlayer->Transform()->GetRelativePos();
	Vec3 vEnemyPos = GetOwner()->GetOwner()->Transform()->GetRelativePos();

	float Distance = vPlayerPos.Distance(vPlayerPos, vEnemyPos);

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
				if(pScript->IsEquippedGun())
					pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CAttackState"));
				else
					pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CFindState"));
					
			}
			else if (SCRIPT_TYPE::FATENEMYSCRIPT == m_Owner->GetScriptType())
			{
				CFatEnemyScript* pScript = (CFatEnemyScript*)m_Owner;
				pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CFindState"));
			}
			return;
		}
		// 벽 뒤에서 총소리를 내면
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

void CIdleState::Exit()
{
}