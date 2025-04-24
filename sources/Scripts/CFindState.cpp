#include "pch.h"
#include "CFindState.h"

#include "CEnemyScript.h"
#include "CFatEnemyScript.h"

#include <Engine/CLevelMgr.h>

CFindState::CFindState()
	: CState(STATE_TYPE::FINDSTATE)
	, m_Speed(300.f)
	, m_DetectRange(600.f)
{
}

CFindState::~CFindState()
{
}


void CFindState::Enter()
{
	if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
	{
		CEnemyScript* pScript = dynamic_cast<CEnemyScript*>(m_Owner);
		pScript->PlayWalkFlip();
	}
	if (SCRIPT_TYPE::FATENEMYSCRIPT == m_Owner->GetScriptType())
	{
		CFatEnemyScript* pScript = dynamic_cast<CFatEnemyScript*>(m_Owner);
		pScript->PlayWalkFlip();
	}
}

void CFindState::FinalTick()
{
	CGameObject* pPlayer = CLevelMgr::GetInst()->FindObjectByName(L"Player");
	
	Vec3 vPlayerPos = pPlayer->Transform()->GetRelativePos();
	Vec3 vEnemyPos = GetOwner()->GetOwner()->Transform()->GetRelativePos();
	Vec3 vEnemyRot = GetOwner()->GetOwner()->Transform()->GetRelativeRotation();

	Vec2 Dir = Vec2(vPlayerPos.x - vEnemyPos.x, vPlayerPos.y - vEnemyPos.y);
	Dir.Normalize();

	vEnemyPos.x += m_Speed * Dir.x * DT;
	vEnemyPos.y += m_Speed * Dir.y * DT;
	vEnemyRot.z = atan2(Dir.y, Dir.x);

	GetOwner()->GetOwner()->Transform()->SetRelativePos(vEnemyPos);
	GetOwner()->GetOwner()->Transform()->SetRelativeRotation(vEnemyRot);

	// �÷��̾�� �����ߴ�.
	if (vPlayerPos.x - 30.f < vEnemyPos.x && vEnemyPos.x < vPlayerPos.x + 30.f
		&& vPlayerPos.y - 30.f < vEnemyPos.y && vEnemyPos.y < vPlayerPos.y + 30.f)
	{
		// ���� ����
		if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
		{
			CEnemyScript* pScript = dynamic_cast<CEnemyScript*>(m_Owner);
			pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CAttackState"));
		}
		else if (SCRIPT_TYPE::FATENEMYSCRIPT == m_Owner->GetScriptType())
		{
			CFatEnemyScript* pScript = dynamic_cast<CFatEnemyScript*>(m_Owner);
			pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CAttackState"));
		}
		return;
	}

	float Distance = vPlayerPos.Distance(vPlayerPos, vEnemyPos);

	// �÷��̾ Ž�� ���� ���� ����.
	if (Distance < m_DetectRange)
	{
		// ���� ���� �÷��̾� ���̿� ���� �ִٸ� Ž���� ���Ѵ�.
		if (FindWall(vPlayerPos, vEnemyPos))
		{
			// SearchState��
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
	}
	else
	{
		// SearchState��
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
	}

}

void CFindState::Exit()
{
}

