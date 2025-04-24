#include "pch.h"
#include "CCursorscript.h"

#include <Engine/CKeyMgr.h>
#include <Engine/CLevelMgr.h>
#include <Engine/CGameObject.h>
#include <Engine/CLevel.h>
#include <Engine/CConstBuffer.h>
#include <Engine/CDevice.h>

#include "CPlayerScript.h"

CCursorscript::CCursorscript()
	: CScript(SCRIPT_TYPE::CURSORSCRIPT)
	, m_bFixAim(false)
	, m_Target(nullptr)
{
}

CCursorscript::~CCursorscript()
{
}



void CCursorscript::Begin()
{
}

void CCursorscript::Tick()
{
	CPlayerScript* pPlayerScript = (CPlayerScript*)CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScripts()[0];
	if (pPlayerScript->IsPaued())

		return;
	Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
	Vec3 vMousePos = CKeyMgr::GetInst()->GetRelativeMousePos();

	if (!m_bFixAim)
	{
		vPos = vMousePos;
	}

	else
	{
		if (m_Target == nullptr)
		{
			m_bFixAim = false;
			return;
		}
		if (m_Target->GetName() == L"DeadBody")
		{
			m_bFixAim = false;
			return;
		}
		if (m_Target->IsDead())
		{
			m_bFixAim = false;
			return;
		}
		vPos = m_Target->Transform()->GetRelativePos();
	}


	vPos.z = 0.f;

	if (KEY_TAP(KEY::WHELLBTN))
	{
		// ���� �������¶�� ������ Ǭ��
		if (m_bFixAim)
			m_bFixAim = false;

		// ���� Ŀ�� ��ó�� ������ �����Ѵ�.
		else
		{
			CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
			vector<CGameObject*> vecEnemy = pCurLevel->GetLayer(3)->GetParentObjects();
			float MinDis = 500.f;

			for (UINT i = 0; i < vecEnemy.size(); ++i)
			{
				Vec3 vEnemyPos = vecEnemy[i]->Transform()->GetRelativePos();
				
				float CurDistance = vEnemyPos.Distance(vEnemyPos, vPos);

				if (vEnemyPos.x - 100.f < vPos.x && vPos.x < vEnemyPos.x + 100.f && vEnemyPos.y - 100.f < vPos.y && vPos.y < vEnemyPos.y + 100.f)
				{
					if (CurDistance < MinDis)
					{
						MinDis = CurDistance;
						m_bFixAim = true;
						m_Target = vecEnemy[i];
					}
				}					
			}
		}	
	}

	BindingFixAim();

	GetOwner()->Transform()->SetRelativePos(vPos);
}

void CCursorscript::SaveComponent(FILE* _File)
{
}

void CCursorscript::LoadComponent(FILE* _File)
{
}

void CCursorscript::BindingFixAim()
{
	CursorData Cursor = {};
 	Cursor.bAimFix = m_bFixAim;

	CConstBuffer* pCB = CDevice::GetInst()->GetCB(CB_TYPE::CURSOR);

	pCB->SetData(&Cursor);
	pCB->Binding();
}


