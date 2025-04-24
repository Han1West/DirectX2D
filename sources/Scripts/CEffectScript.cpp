#include "pch.h"
#include "CEffectScript.h"

CEffectScript::CEffectScript()
	: CScript(SCRIPT_TYPE::EFFECTSCRIPT)
	, m_AccTime(0.f)
{
}

CEffectScript::~CEffectScript()
{
}


void CEffectScript::Begin()
{
}

void CEffectScript::Tick()
{
	m_AccTime += DT;

	if (1.f < m_AccTime)
		DestroyObject(GetOwner());
}

void CEffectScript::SaveComponent(FILE* _File)
{
}

void CEffectScript::LoadComponent(FILE* _File)
{
}

