#include "pch.h"
#include "CSearchState.h"

#include "CEnemyScript.h"
#include "CFatEnemyScript.h"
#include "CPlayerScript.h"

#include <Engine/CLevelMgr.h>

CSearchState::CSearchState()
	: CState(STATE_TYPE::SEARCHSTATE)
	, m_DetectRange(600.f)
	, m_AccTime(0.f)
{
}

CSearchState::~CSearchState()
{
}


void CSearchState::Enter()
{
	if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
	{
		CEnemyScript* pScript = dynamic_cast<CEnemyScript*>(m_Owner);
		pScript->PlaySearchFlip();
	}
	if (SCRIPT_TYPE::FATENEMYSCRIPT == m_Owner->GetScriptType())
	{
		CFatEnemyScript* pScript = dynamic_cast<CFatEnemyScript*>(m_Owner);
		pScript->PlaySearchFlip();
	}
}

void CSearchState::FinalTick()
{
	m_AccTime += DT;

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
			return;
		}
		else
		{
			CGameObject* pPlayer = CLevelMgr::GetInst()->FindObjectByName(L"Player");
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

	// 일정 시간 동안 Player를 발견하지 못함.
	if (3.f < m_AccTime)
	{
		// 원래의 자리로 돌아간다.
		if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
		{
			CEnemyScript* pScript = (CEnemyScript*)m_Owner;
			pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CReturnState"));

		}
		else if (SCRIPT_TYPE::FATENEMYSCRIPT == m_Owner->GetScriptType())
		{
			CFatEnemyScript* pScript = (CFatEnemyScript*)m_Owner;
			pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CReturnState"));
		}
		m_AccTime = 0.f;
	}
}

void CSearchState::Exit()
{
}
