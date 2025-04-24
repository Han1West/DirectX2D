#include "pch.h"
#include "CTraceState.h"

#include "CEnemyScript.h"
#include "CFatEnemyScript.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>

CTraceState::CTraceState()
	: CState(STATE_TYPE::TRACESTATE)
	, m_Speed(200.f)
	, m_DetectRange(600.f)
{
}

CTraceState::~CTraceState()
{
}


void CTraceState::Enter()
{
	Vec3 vEnemyPos = m_Owner->GetOwner()->Transform()->GetRelativePos();

	if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
	{
		CEnemyScript* pScript = dynamic_cast<CEnemyScript*>(m_Owner);
		pScript->PlayWalkFlip();
		m_DestPos = pScript->GetDestPos();
	}
	if (SCRIPT_TYPE::FATENEMYSCRIPT == m_Owner->GetScriptType())
	{
		CFatEnemyScript* pScript = dynamic_cast<CFatEnemyScript*>(m_Owner);
		pScript->PlayWalkFlip();
		m_DestPos = pScript->GetDestPos();
	}

	m_lPath = FindPath(vEnemyPos, m_DestPos);
	m_iter = m_lPath.begin();
}

void CTraceState::FinalTick()
{
	// ���� ã�� ����
	if (m_lPath.size() == 0)
	{
		if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
		{
			CEnemyScript* pScript = dynamic_cast<CEnemyScript*>(m_Owner);
			pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CFindState"));
		}
		else if (SCRIPT_TYPE::FATENEMYSCRIPT == m_Owner->GetScriptType())
		{
			CFatEnemyScript* pScript = dynamic_cast<CFatEnemyScript*>(m_Owner);
			pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CFindState"));
		}
		return;
	}

	// ���� �÷��̾��� ��ġ
	Vec3 vPlayerPos = CLevelMgr::GetInst()->FindObjectByName(L"Player")->Transform()->GetRelativePos();
	CScript* pScript = GetOwner();
	CGameObject* pObject = pScript->GetOwner();

	Vec3 vEnemyPos = GetOwner()->GetOwner()->Transform()->GetRelativePos();

	float Distance = vPlayerPos.Distance(vPlayerPos, vEnemyPos);


	// ���� ���߿� Player�� Ž������ ��
	if (Distance < m_DetectRange)
	{
		// Player�� ������
		if (!FindWall(vPlayerPos, vEnemyPos))
		{
			if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
			{
				// ���Ÿ� ������ Attack����
				CEnemyScript* pScript = (CEnemyScript*)m_Owner;
				if (pScript->IsEquippedGun())
				{
					pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CAttackState"));
				}
				// �ƴ϶�� ����
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
		
	}

	// ���� �������� ����
	if (m_iter == prev(m_lPath.end()) )
	{
		// �ش� ��ҿ��� Search
		if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
		{
			CEnemyScript* pScript = (CEnemyScript*)m_Owner;
			pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CSearchState"));
		}
		else if (SCRIPT_TYPE::FATENEMYSCRIPT == m_Owner->GetScriptType())
		{
			CFatEnemyScript* pScript = (CFatEnemyScript*)m_Owner;
			pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CSearchState"));
		}
		return;
	}

	Vec2 vDestPos = *m_iter;
	// �������� �̵�
	MoveToward(GetOwner()->GetOwner(), vDestPos, m_Speed);

	// �������� �����ϸ� ���� �������� ����
	if ((vDestPos.x - 5.f < vEnemyPos.x && vEnemyPos.x < vDestPos.x + 5.f)
		&& (vDestPos.y - 5.f < vEnemyPos.y && vEnemyPos.y < vDestPos.y + 5.f))
		++m_iter;

	
}

void CTraceState::Exit()
{
}

