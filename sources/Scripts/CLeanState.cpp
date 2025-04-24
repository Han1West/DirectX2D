#include "pch.h"
#include "CLeanState.h"
#include "CEnemyScript.h"


CLeanState::CLeanState()
	: CState(STATE_TYPE::LEANSTATE)
{
}

CLeanState::~CLeanState()
{
}

void CLeanState::Enter()
{
	Vec3 vRot = GetOwner()->GetOwner()->Transform()->GetRelativeRotation();

	if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
	{
		CEnemyScript* pScript = dynamic_cast<CEnemyScript*>(m_Owner);
		pScript->PlaySpecialFlip(2);
		m_Owner->GetOwner()->Collider2D()->SetScale(Vec2(0.3f, 0.2f));
	}
}

void CLeanState::FinalTick()
{
	m_RecoverTime += DT;

	// 일정 시간이 지나면 기절에서 회복
	if (3.f < m_RecoverTime)
	{
		if (0.f == m_RecoveringTime)
		{
			if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
			{
				CEnemyScript* pScript = dynamic_cast<CEnemyScript*>(m_Owner);
				pScript->PlaySpecialFlip(3);
			}
		}

		m_RecoveringTime += DT;
		if (1.f < m_RecoveringTime)
		{
			CEnemyScript* pScript = dynamic_cast<CEnemyScript*>(m_Owner);
			pScript->SetWeapon((int)WEAPON_TYPE::NONE);
			pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CReturnState"));
		}
	}
}

void CLeanState::Exit()
{
	m_Owner->GetOwner()->Collider2D()->SetScale(Vec2(0.1f, 0.2f));
}


