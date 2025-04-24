#include "pch.h"
#include "CMeleeAttackScript.h"

#include <Engine/CCollider2D.h>

CMeleeAttackScript::CMeleeAttackScript()
	: CScript(SCRIPT_TYPE::MELEEATTACKSCRIPT)
	, m_LifeTime(0.f)
	, m_CurWeapon(WEAPON_TYPE::NONE)
{
}

CMeleeAttackScript::~CMeleeAttackScript()
{
}


void CMeleeAttackScript::Begin()
{

	// 무기에 따라 공격 범위 조절
	switch (m_CurWeapon)
	{
	case WEAPON_TYPE::NONE:
		Collider2D()->SetScale(Vec2(500.f, 300.f));
		break;
	case WEAPON_TYPE::KNIFE:
		Collider2D()->SetScale(Vec2(500.f, 600.f));
		break;
	case WEAPON_TYPE::LONG_PIPE:
		break;
	case WEAPON_TYPE::BAT:
		break;
	case WEAPON_TYPE::KATANA:
	case WEAPON_TYPE::AXE:
		Collider2D()->SetScale(Vec2(800.f, 800.f));
		break;

	default:
		break;
	}

}

void CMeleeAttackScript::Tick()
{
	m_LifeTime += DT;


	if (0.05f < m_LifeTime)
	{
		m_LifeTime = 0.f;
		DestroyObject(GetOwner());
	}

}

void CMeleeAttackScript::BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CMeleeAttackScript::Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CMeleeAttackScript::EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CMeleeAttackScript::SaveComponent(FILE* _File)
{
}

void CMeleeAttackScript::LoadComponent(FILE* _File)
{
}
