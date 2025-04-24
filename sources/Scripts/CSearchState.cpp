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

	// ���� �÷��̾��� ��ġ
	CGameObject* pPlayer = CLevelMgr::GetInst()->FindObjectByName(L"Player");
	Vec3 vPlayerPos = pPlayer->Transform()->GetRelativePos();
	Vec3 vEnemyPos = GetOwner()->GetOwner()->Transform()->GetRelativePos();

	float Distance = vPlayerPos.Distance(vPlayerPos, vEnemyPos);

	// �÷��̾ Ž�� ���� ���� ���Դ�.
	if (Distance < m_DetectRange)
	{
		// ���� ���� �÷��̾� ���̿� ���� �ִٸ� Ž���� ���Ѵ�.
		if (!FindWall(vPlayerPos, vEnemyPos))
		{
			// �÷��̾ �����Ѵ�
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
				// �÷��̾ �����Ѵ�
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

	// ���� �ð� ���� Player�� �߰����� ����.
	if (3.f < m_AccTime)
	{
		// ������ �ڸ��� ���ư���.
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
