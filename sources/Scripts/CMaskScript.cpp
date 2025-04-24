#include "pch.h"
#include "CMaskScript.h"

#include "CSelectMaskScript.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>

CMaskScript::CMaskScript()
	: CScript(SCRIPT_TYPE::MASKSCRIPT)
	, m_ChangingSpeed(600.f)
{
}

CMaskScript::~CMaskScript()
{
}


void CMaskScript::Begin()
{
	m_Scale = Transform()->GetRelativeScale();

	m_OwnType = (MASK_TYPE)m_OwnTypeint;
	Vec3 vPlayerPos = CLevelMgr::GetInst()->FindObjectByName(L"Player")->Transform()->GetRelativePos();
	switch (m_OwnType)
	{
	case MASK_TYPE::RICHARD_CHICKEN:
		Transform()->SetRelativePos(Vec3(-400.f, vPlayerPos.y + ((int)m_OwnType * -300.f), -100.f));
		break;
	case MASK_TYPE::TONY_TIGER:
		Transform()->SetRelativePos(Vec3(-400.f, vPlayerPos.y + ((int)m_OwnType * -300.f), -100.f));
		break;
	case MASK_TYPE::OBRI_PIG:
		Transform()->SetRelativePos(Vec3(-400.f, vPlayerPos.y + ((int)m_OwnType * -300.f), -100.f));
		break;
	case MASK_TYPE::JUAN_HORSE:
		Transform()->SetRelativePos(Vec3(-400.f, vPlayerPos.y + ((int)m_OwnType * -300.f), -100.f));
		break;
	case MASK_TYPE::GRAHAM_RABBIT:
		Transform()->SetRelativePos(Vec3(-400.f, vPlayerPos.y + ((int)m_OwnType * -300.f), -100.f));
		break;
	case MASK_TYPE::DENIS_WOLF:
		Transform()->SetRelativePos(Vec3(-400.f, vPlayerPos.y + ((int)m_OwnType * -300.f), -100.f));
		break;
	case MASK_TYPE::GEORGE_GIRAFFE:
		Transform()->SetRelativePos(Vec3(-400.f, vPlayerPos.y + ((int)m_OwnType * -300.f), -100.f));
		break;
	case MASK_TYPE::RAMI_CAMEL:
		Transform()->SetRelativePos(Vec3(-400.f, vPlayerPos.y + ((int)m_OwnType * -300.f), -100.f));
		break;
	default:
		break;
	}
}

void CMaskScript::Tick()
{
	CGameObject* SelectMaskObj = CLevelMgr::GetInst()->FindObjectByName(L"SelectMask");
	CSelectMaskScript* pScript = (CSelectMaskScript*)SelectMaskObj->GetScripts()[0];

	int CurIdx = pScript->GetCurIndex();

	// 현재 이 가면을 선택중임
	if (CurIdx == (int)m_OwnType)
	{
		m_Focus = true;
	}
	else
	{
		m_Focus = false;
	}
	
	if (pScript->IsChanging())
	{
		Vec3 CurPos = Transform()->GetRelativePos();

		if (pScript->IsUp())
		{
			if (500.f < m_MoveDistance)
			{
				m_MoveDistance = 0.f;
				return;
			}
			CurPos.y -= m_ChangingSpeed * DT;
			m_MoveDistance += m_ChangingSpeed * DT;

			Transform()->SetRelativePos(CurPos);
		}
		else
		{
			if (500.f < m_MoveDistance)
			{
				m_MoveDistance = 0.f;
				return;
			}
			CurPos.y += m_ChangingSpeed * DT;
			m_MoveDistance += m_ChangingSpeed * DT;

			Transform()->SetRelativePos(CurPos);
		}
	}
	

	if (!pScript->IsActive())
		DestroyObject(GetOwner());

	if (m_Focus)
	{
		Vec3 vScale = Transform()->GetRelativeScale();
		Vec3 vRot = Transform()->GetRelativeRotation();

		float OscillationSpeed = 2.f;
		float OscillationAmplitued = 0.2f;

		m_AccTime += DT;

		vScale.x = m_Scale.x + 100.f;
		vScale.y = m_Scale.y + 100.f;

		vRot.z = OscillationAmplitued * sin(OscillationSpeed * m_AccTime);

		Transform()->SetRelativeRotation(vRot);
		Transform()->SetRelativeScale(vScale);
	}
	else
	{
		Vec3 vRot = Transform()->GetRelativeRotation();

		vRot.z = 0.f;

		Transform()->SetRelativeRotation(vRot);
		Transform()->SetRelativeScale(m_Scale);
	}
}

void CMaskScript::SaveComponent(FILE* _File)
{
	fwrite(&m_OwnTypeint, sizeof(int), 1, _File);
}

void CMaskScript::LoadComponent(FILE* _File)
{
	fread(&m_OwnTypeint, sizeof(int), 1, _File);
}

