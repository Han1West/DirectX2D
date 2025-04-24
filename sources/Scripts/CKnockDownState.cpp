#include "pch.h"
#include "CKnockDownState.h"

#include <Engine/CScript.h>
//#include <Engine/COO>

#include "CEnemyScript.h"

CKnockDownState::CKnockDownState()
	: CState(STATE_TYPE::KNOCKDOWNSTATE)
	, m_RecoverTime(0.f)
	, m_RecoveringTime(0.f)
	, m_KnockBackTime(0.f)
	, m_Dir()
	, m_IsMount(false)
{
}

CKnockDownState::~CKnockDownState()
{
}

void CKnockDownState::Enter()
{
	m_KnockBackTime = 0.f;

	float Theta = GetThetaWithPlayer();

	m_Dir = Vec2(cos(Theta), sin(Theta));

	Vec3 vRot = m_Owner->GetOwner()->Transform()->GetRelativeRotation();
	vRot.z = Theta;
	m_Owner->GetOwner()->Transform()->SetRelativeRotation(vRot);

	if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
	{
		CEnemyScript* pScript = dynamic_cast<CEnemyScript*>(m_Owner);
		pScript->PlaySpecialFlip(1);
		m_Owner->GetOwner()->Collider2D()->SetScale(Vec2(0.3f, 0.2f));
	}
}

void CKnockDownState::FinalTick()
{
	Vec3 vPos = GetOwner()->GetOwner()->Transform()->GetRelativePos();

	m_KnockBackTime += DT;
	if (m_KnockBackTime < 0.2f)
	{
		vPos.x += 300.f * m_Dir.x * DT;
		vPos.y += 300.f * m_Dir.y * DT;

		GetOwner()->GetOwner()->Transform()->SetRelativePos(vPos);
	}
	
	m_RecoverTime += DT;

	// 일정 시간이 지나면 기절에서 회복
	if (3.f < m_RecoverTime)
	{
		if (m_IsMount)
		{
			m_RecoverTime = 0.f;
		}


		if(0.f == m_RecoveringTime)
		{
			if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
			{
				CEnemyScript* pScript = dynamic_cast<CEnemyScript*>(m_Owner);
				pScript->PlaySpecialFlip(0);
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

void CKnockDownState::Exit()
{
	m_Owner->GetOwner()->Collider2D()->SetScale(Vec2(0.1f, 0.2f));
	//Vec3 vRot = m_Owner->GetOwner()->Transform()->GetRelativeRotation();
	//vRot.z -= 160.f;
	//m_Owner->GetOwner()->Transform()->SetRelativeRotation(vRot);
}

