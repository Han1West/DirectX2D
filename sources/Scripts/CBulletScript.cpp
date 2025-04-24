#include "pch.h"
#include "CBulletScript.h"

#include <Engine/CPrefab.h>
#include <Engine/CAssetMgr.h>
#include <Engine/CLevelMgr.h>

#include "CEnemyScript.h"
#include "CFatEnemyScript.h"
#include "CPlayerScript.h"
#include "CState.h"

CBulletScript::CBulletScript()
	: CScript(SCRIPT_TYPE::BULLETSCRIPT)
	, m_Speed(800.f)
	, m_Dir(Vec2(0.f,0.f))
	, m_IsShotgun(false)
	, m_IsEnemy(false)
{
}

CBulletScript::~CBulletScript()
{
}

void CBulletScript::Begin()
{
}

void CBulletScript::Tick()
{
	CPlayerScript* pPlayerScript = (CPlayerScript*)CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScripts()[0];
	if (pPlayerScript->IsPaued())
		return;

	// ������ �������� �Ѿ� �̵�
	Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
	Vec3 vRot = GetOwner()->Transform()->GetRelativeRotation();
	bool RotChange = false;

	float Radian = atan2(m_Dir.y, m_Dir.x);

	vRot.z = Radian;


	if (!RotChange)
	{
		GetOwner()->Transform()->SetRelativeRotation(vRot);
		RotChange = true;
	}

	// �����̶�� 4���� �Ѿ��� �� ����� ������ �ٲ��ش�.
	if (m_IsShotgun)
	{
		Ptr<CPrefab> BulletPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\Bullet.pref", L"Prefab\\Bullet.pref");

		float angles[] = { -10.f, -5.f, 0.f, 5.f, 10.f };

		for (UINT i = 0; i < 4; ++i)
		{
			Vec2 Dir = RotateVector(m_Dir, angles[i]);

			vector<CScript*> vecBulletScript = BulletPrefab->GetProtoObject()->GetScripts();
			for (UINT i = 0; i < vecBulletScript.size(); ++i)
			{
				if (SCRIPT_TYPE::BULLETSCRIPT == vecBulletScript[i]->GetScriptType())
				{
					CBulletScript* pScript = dynamic_cast<CBulletScript*>(vecBulletScript[i]);
					pScript->SetDir(Dir);
					pScript->SetShotgun(false);
				}
			}

			if(m_IsEnemy)
				Instantiate(BulletPrefab, vPos, 6);
			else
				Instantiate(BulletPrefab, vPos, 5);
		}
		m_IsShotgun = false;
	}

	vPos.x += m_Speed * m_Dir.x * DT;
	vPos.y += m_Speed * m_Dir.y * DT;


	GetOwner()->Transform()->SetRelativePos(vPos);
}

void CBulletScript::BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{

	// �װų� �װ� �ִ� �����Դ� �Ѿ��� ������� �ʴ´�.
	if (_OtherCollider->GetName() == L"Enemy") 
	{
		CEnemyScript* pScript = (CEnemyScript*)_OtherObject->GetScripts()[0];
		if (pScript->IsDead())
			/*GetState()->GetStateType() == STATE_TYPE::DEADSTATE ||
			pScript->GetState()->GetStateType() == STATE_TYPE::DYINGSTATE
			|| pScript->GetState()->GetStateType() == STATE_TYPE::KNOCKDOWNSTATE)*/
		{
			return;
		}

	}
	if (_OtherCollider->GetName() == L"FatEnemy")
	{
		CFatEnemyScript* pScript = (CFatEnemyScript*)_OtherObject->GetScripts()[0];
		if (pScript->IsDead())
			/*GetState()->GetStateType() == STATE_TYPE::DEADSTATE ||
			pScript->GetState()->GetStateType() == STATE_TYPE::DYINGSTATE)*/
		{
			return;
		}
	}
	
	Vec3 vPos = Transform()->GetRelativePos();

	Ptr<CPrefab> BulletHitEffect = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\BulletHit.pref", L"Prefab\\BulletHit.pref");
	
	Instantiate(BulletHitEffect, vPos, 7);
	DestroyObject(GetOwner());
}                    

void CBulletScript::Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CBulletScript::EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CBulletScript::SaveComponent(FILE* _File)
{
}

void CBulletScript::LoadComponent(FILE* _File)
{
}


Vec2 CBulletScript::RotateVector(const Vec2& _vec, float Angle)
{
	float radians = Angle * (3.14f / 180.0f); // ������ �������� ��ȯ
	float cosTheta = cos(radians);
	float sinTheta = sin(radians);

	return Vec2( _vec.x * cosTheta - _vec.y * sinTheta,
				_vec.x * sinTheta + _vec.y * cosTheta );
}