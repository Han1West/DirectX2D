#include "pch.h"
#include "CWeaponScript.h"

#include "CEnemyScript.h"

#include "CState.h"

#include <Engine/CConstBuffer.h>
#include <Engine/CLevelMgr.h>
#include <Engine/CDevice.h>

#include "CPlayerScript.h"



CWeaponScript::CWeaponScript()
	: CScript(SCRIPT_TYPE::WEAPONSCRIPT)
	, m_IsEquip(false)
	, m_Thrown(false)
	, m_IsDrop(false)
	, m_Round(0)
	, m_Speed(800.f)
	, m_AccTime(0.f)
	, m_Dir(Vec2(0.f,0.f))
	, m_FirstUse(true)
	, m_EnemyUse(false)
	, m_IsBounceOff(false)
	, m_UpdateDirTime(0.f)
	, m_BounceAccTime(0.f)
	, m_IsGround(false)
{
}

CWeaponScript::~CWeaponScript()
{
}


void CWeaponScript::Begin()
{
	m_IsGround = true;
}

void CWeaponScript::Tick()
{
	CPlayerScript* pPlayerScript = (CPlayerScript*)CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScripts()[0];
	if (pPlayerScript->IsPaued())
		return;

	if (m_IsEquip)
	{
		DestroyObject(GetOwner());
	}

	if (GetOwner()->GetName() == L"Clamp")
	{
		if (m_Thrown)
		{
			m_IsGround = false;
			Throw();
		}
	}
	else
	{
		if (m_Thrown)
		{
			m_IsGround = false;
			m_AccTime += DT;

			if (m_AccTime < 0.5f)
				Throw();
			else
			{
				m_AccTime = 0.f;
				m_Thrown = false;
				m_IsGround = true;
			}

		}
	}


	if (m_IsDrop)
	{
		m_IsGround = false;
		m_AccTime += DT;

		if (m_AccTime < 0.2f)
			Dropped();
		else
		{
			m_AccTime = 0.f;
			m_IsDrop = false;
			m_IsGround = true;
		}			
	}

	if (m_IsBounceOff)
	{
		m_IsGround = false;
		m_BounceAccTime += DT;

		if (m_BounceAccTime < 0.15f)
			BounceOff();
		else
		{
			m_BounceAccTime = 0.f;
			m_IsBounceOff = false;
			m_IsGround = true;
		}
	}


	BindingGround(m_IsGround);
}

void CWeaponScript::Throw()
{
	Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
	Vec3 vRot = GetOwner()->Transform()->GetRelativeRotation();

	vPos.x += m_Speed * m_Dir.x * DT;
	vPos.y += m_Speed * m_Dir.y * DT;

	vRot.z += DT * XM_PI * 8.f;

	GetOwner()->Transform()->SetRelativePos(vPos);
	GetOwner()->Transform()->SetRelativeRotation(vRot);
}

void CWeaponScript::Dropped()
{
	Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
	Vec3 vRot = GetOwner()->Transform()->GetRelativeRotation();

	vPos.x += (m_Speed / 2.f) * m_Dir.x * DT;
	vPos.y += (m_Speed / 2.f) * m_Dir.y * DT;

	vRot.z += DT * XM_PI * 8.f;

	GetOwner()->Transform()->SetRelativePos(vPos);
	GetOwner()->Transform()->SetRelativeRotation(vRot);
}


void CWeaponScript::BounceOff()
{
	m_UpdateDirTime += DT;

	Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
	Vec3 vRot = GetOwner()->Transform()->GetRelativeRotation();

	vPos.x += m_Speed * m_RfDir.x * DT;
	vPos.y += m_Speed * m_RfDir.y * DT;

	vRot.z += DT * XM_PI * 8.f;

	GetOwner()->Transform()->SetRelativePos(vPos);
	GetOwner()->Transform()->SetRelativeRotation(vRot);
	
	// 다음 충돌을 대비해 방향값 갱신
	if (0.00002f < m_UpdateDirTime)
	{
		m_Dir = m_RfDir;
		m_UpdateDirTime = 0.f;
	}
}

Vec2 CWeaponScript::GetReflectionDir(Vec2 _ObjDir, float _WallTheta)
{
	float ThetaRadian = _WallTheta * (180.f / XM_PI);
	Vec2 WallDir = Vec2(cos(ThetaRadian), sin(ThetaRadian));
	WallDir.Normalize();
	float NewTheta = m_Dir.Dot(WallDir);
	Vec2 NewDir = m_Dir - WallDir * (2 * NewTheta);

	return m_Dir - WallDir * (2 * NewTheta);
}

void CWeaponScript::BindingGround(bool _IsGround)
{
	WeaponData Weapon = {};
	Weapon.bGround = _IsGround;

	CConstBuffer* pCB = CDevice::GetInst()->GetCB(CB_TYPE::WEAPON);

	pCB->SetData(&Weapon);
	pCB->Binding();
}

void CWeaponScript::BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	if (GetOwner()->GetName() == L"Clamp")
	{
		if (_OtherCollider->GetName() == L"Wall")
		{
			Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\boss\\HitWall.wav", L"Sound\\attack\\HitWall.wav");
			pSound->Play(1, 0.5f, true);
			m_Thrown = false;
			//m_RfDir = GetReflectionDir(m_Dir, _OtherObject->Transform()->GetRelativeRotation().z);
		}

		if (_OtherCollider->GetName() == L"StgObj")
		{
			Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\boss\\HitWall.wav", L"Sound\\attack\\HitWall.wav");
			pSound->Play(1, 0.5f, true);
			m_Thrown = false;
			m_IsBounceOff = true;
			m_RfDir = Vec2(0.f, 1.f);//GetReflectionDir(m_Dir, _OtherObject->Transform()->GetRelativeRotation().z);
		}
	}
	else
	{

		// 던져지고 있는 도중에는 충돌체크를 한다
		if (m_Thrown || m_IsDrop || m_IsBounceOff)
		{
			// 적 또는 문과 충돌 했다.
			if (_OtherCollider->GetName() == L"Enemy")
			{
				CEnemyScript* pScript = (CEnemyScript*)_OtherObject->GetScripts()[0];
				if (pScript->GetStateType() == STATE_TYPE::DEADSTATE ||
					pScript->GetStateType() == STATE_TYPE::DYINGSTATE)
				{
					return;
				}
				else
				{
					m_Thrown = false;
				}
			}

			if (_OtherCollider->GetName() == L"Door")
			{
				m_Thrown = false;
			}

			// 벽과 충돌했다.
			if (_OtherCollider->GetName() == L"Wall")
			{
				m_Thrown = false;
				m_IsBounceOff = true;
				// 튕겨나갈 방향 구하기
				m_RfDir = GetReflectionDir(m_Dir, _OtherObject->Transform()->GetRelativeRotation().z);
			}
		}
	}
}

void CWeaponScript::Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CWeaponScript::EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CWeaponScript::SaveComponent(FILE* _File)
{
}

void CWeaponScript::LoadComponent(FILE* _File)
{
}

