#include "pch.h"
#include "CPauseUIScript.h"

#include <Engine/CLevelMgr.h>


#include "CPlayerScript.h"
#include "CPauseMenuScript.h"

CPauseUIScript::CPauseUIScript()
	: CScript(SCRIPT_TYPE::PAUSEUISCRIPT)
{
}

CPauseUIScript::~CPauseUIScript()
{
}

void CPauseUIScript::Begin()
{
}

void CPauseUIScript::Tick()
{
	CPlayerScript* pScript = (CPlayerScript*)CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScripts()[0];
	if (pScript->IsPaued())
	{
		Active();
	}
	else
	{
		Transform()->SetRelativeScale(Vec3(0.f, 0.f, 0.f));
	}
}

void CPauseUIScript::BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CPauseUIScript::Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CPauseUIScript::EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CPauseUIScript::SaveComponent(FILE* _File)
{
}

void CPauseUIScript::LoadComponent(FILE* _File)
{
}

void CPauseUIScript::Active()
{
	if(GetOwner()->GetName()==L"PauseUI")
		GetOwner()->Transform()->SetRelativeScale(Vec3(368.f, 58.f, 0.f));
	else
		GetOwner()->Transform()->SetRelativeScale(Vec3(250.f, 50.f, 0.f));

	Vec3 UIScale = Transform()->GetRelativeScale();

	CPauseMenuScript* menuScript = (CPauseMenuScript*)CLevelMgr::GetInst()->FindObjectByName(L"PauseMenu")->GetScripts()[0];

	int CurIdx = menuScript->GetIdx();

	bool bContinueSelected = false;
	bool bRestartSelected = false;
	bool bQuitSelected = false;

	if (GetOwner()->GetName() == L"ContinueUI" && CurIdx == 0)
	{
		bContinueSelected = true;
		Vec3 vRot = Transform()->GetRelativeRotation();
		Vec3 vScale = Transform()->GetRelativeScale();

		float OscillationSpeed = 2.f;
		float OscillationAmplitued = 0.2f;

		m_AccTime += DT;

		vRot.z = OscillationAmplitued * sin(OscillationSpeed * m_AccTime);

		vScale.x = UIScale.x + 100.f;
		vScale.y = UIScale.y + 50.f;

		Transform()->SetRelativeRotation(vRot);
		Transform()->SetRelativeScale(vScale);
	}
	else if (GetOwner()->GetName() == L"ContinueUI" && CurIdx != 0)
	{
		Vec3 vRot = Transform()->GetRelativeRotation();
		Vec3 vScale = Transform()->GetRelativeScale();

		vRot.z = 0.f;

		Transform()->SetRelativeRotation(vRot);
	}

	if (GetOwner()->GetName() == L"PauseRestartUI" && CurIdx == 1)
	{
		bRestartSelected = true;
		Vec3 vRot = Transform()->GetRelativeRotation();
		Vec3 vScale = Transform()->GetRelativeScale();

		float OscillationSpeed = 2.f;
		float OscillationAmplitued = 0.2f;

		m_AccTime += DT;

		vRot.z = OscillationAmplitued * sin(OscillationSpeed * m_AccTime);

		vScale.x = UIScale.x + 100.f;
		vScale.y = UIScale.y + 50.f;

		Transform()->SetRelativeRotation(vRot);
		Transform()->SetRelativeScale(vScale);
	}
	else if (GetOwner()->GetName() == L"PauseRestartUI" && CurIdx != 1)
	{
		Vec3 vRot = Transform()->GetRelativeRotation();
		Vec3 vScale = Transform()->GetRelativeScale();

		vRot.z = 0.f;;

		Transform()->SetRelativeRotation(vRot);
	}

	if (GetOwner()->GetName() == L"QuitUI" && CurIdx == 2)
	{
		bRestartSelected = true;
		Vec3 vRot = Transform()->GetRelativeRotation();
		Vec3 vScale = Transform()->GetRelativeScale();

		float OscillationSpeed = 2.f;
		float OscillationAmplitued = 0.2f;

		m_AccTime += DT;

		vRot.z = OscillationAmplitued * sin(OscillationSpeed * m_AccTime);

		vScale.x = UIScale.x + 100.f;
		vScale.y = UIScale.y + 50.f;

		Transform()->SetRelativeRotation(vRot);
		Transform()->SetRelativeScale(vScale);
	}
	else if (GetOwner()->GetName() == L"QuitUI" && CurIdx != 2)
	{
		Vec3 vRot = Transform()->GetRelativeRotation();
		Vec3 vScale = Transform()->GetRelativeScale();

		vRot.z = 0.f;

		Transform()->SetRelativeRotation(vRot);
	}

}