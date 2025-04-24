#include "pch.h"
#include "CSelectMaskScript.h"

#include <Engine/CKeyMgr.h>
#include <Engine/CLevelMgr.h>

#include "CPlayerScript.h"

CSelectMaskScript::CSelectMaskScript()
	: CScript(SCRIPT_TYPE::SELECTMASKSCRIPT)
	, m_Active(true)
	, m_Changing(false)
	, m_AccTime(0.f)
	, m_ChangingTime(0.f)
	, m_Idx()
	, m_IsUp(false)
	, m_SelectedMask(MASK_TYPE::RICHARD_CHICKEN)
{
}

CSelectMaskScript::~CSelectMaskScript()
{
}

void CSelectMaskScript::Tick()
{
	CPlayerScript* pPlayerScript = (CPlayerScript*)CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScripts()[0];
	if (pPlayerScript->IsPaued())
		return;

	if (m_Active)
	{
		if (KEY_TAP(KEY::W) && !m_Changing)
		{
			m_Idx -= 1;
			m_Changing = true;
			if (m_Idx < 0)
			{
				m_Idx = 0;
				m_Changing = false;
			}
			m_IsUp = true;
		}
		if (KEY_TAP(KEY::S) && !m_Changing)
		{
			m_Idx += 1;
			m_Changing = true;
			if (m_Idx == (int)MASK_TYPE::END)
			{
				m_Idx = (int)MASK_TYPE::END - 1;
				m_Changing = false;
			}
			m_IsUp = false;
		}

		if (m_Changing)
		{
			m_ChangingTime += DT;

			if (0.5f < m_ChangingTime)
			{
				m_Changing = false;
				m_ChangingTime = 0.f;
			}
				
		}


		if (KEY_TAP(KEY::SPACE))// || KEY_TAP(KEY::LBTN))
		{
			m_SelectedMask = (MASK_TYPE)m_Idx;
			
			CGameObject* pPlayer = CLevelMgr::GetInst()->FindObjectByName(L"Player");
			CPlayerScript* pPlayerScript = (CPlayerScript*)pPlayer->GetScripts()[0];

			pPlayerScript->SetMask(m_SelectedMask);
			pPlayerScript->Begin();
			m_Active = false;
		}	
	}

	if (!m_Active)
	{
		m_AccTime += DT;
		if (1.f < m_AccTime)
			DestroyObject(GetOwner());
	}
}

void CSelectMaskScript::SaveComponent(FILE* _File)
{
}

void CSelectMaskScript::LoadComponent(FILE* _File)
{
}


