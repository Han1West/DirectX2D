#include "pch.h"
#include "CShellScript.h"

CShellScript::CShellScript()
	: CScript(SCRIPT_TYPE::SHELLSCRIPT)
	, m_Speed(500.f)
	, m_Dir(Vec2(0.f,0.f))
	, m_MoveTime(0.f)
	, m_LifeTime(0.f)
{
}

CShellScript::~CShellScript()
{
}

void CShellScript::Begin()
{
}

void CShellScript::Tick()
{
	m_MoveTime += DT;
	m_LifeTime += DT;

	// ������ �������� ź�� �̵�, ȸ�� 
	Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
	Vec3 vRot = GetOwner()->Transform()->GetRelativeRotation();

	vPos.x += m_Speed * m_Dir.x * DT;
	vPos.y += m_Speed * m_Dir.y * DT;

	vRot.z += DT * XM_PI * 4.f;

	// ���� �ð��� ������ �����Ӱ� ȸ���� �����
	if (m_MoveTime < 0.5f)
	{
		GetOwner()->Transform()->SetRelativePos(vPos);
		GetOwner()->Transform()->SetRelativeRotation(vRot);
	}
	
	// ���� �ð��� ������ ź�ǻ���
	if (1.f < m_LifeTime)
	{
		DestroyObject(GetOwner());
	}
}

void CShellScript::BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CShellScript::Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CShellScript::EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CShellScript::SaveComponent(FILE* _File)
{
}

void CShellScript::LoadComponent(FILE* _File)
{
}


