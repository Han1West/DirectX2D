#include "pch.h"
#include "CBloodScript.h"

#include <Engine/CLevelMgr.h>
#include "CPlayerScript.h"

CBloodScript::CBloodScript()
	: CScript(SCRIPT_TYPE::BLOODSCRIPT)
	, m_IsBase(false)
	, m_IsSpread(false)
	, m_Speed(100.f)
	, m_AccTime(0.f)
{
}

CBloodScript::~CBloodScript()
{
}

void CBloodScript::Begin()
{
}

void CBloodScript::Tick()
{
	CPlayerScript* pPlayerScript = (CPlayerScript*)CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScripts()[0];
	if (pPlayerScript->IsPaued())
		return;

	if (m_IsSpread)
	{
		m_AccTime += DT;
		if (m_AccTime < 1.f)
		{
			Spread();
			MakeSplat();
		}
			
		else
		{
			m_AccTime = 0.f;
			m_IsSpread = false;
			DestroyObject(GetOwner());
		}
	}
}


void CBloodScript::Spread()
{
	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vRot = Transform()->GetRelativeRotation();
	bool RotChange = false;

	float Radian = atan2(m_Dir.y, m_Dir.x);

	vRot.z = Radian;


	if (!RotChange)
	{
		GetOwner()->Transform()->SetRelativeRotation(vRot);
		RotChange = true;
	}

	if (m_IsBase)
	{
		Ptr<CPrefab> BloodPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\SquirtBlood.pref", L"Prefab\\SquirtBlood.pref");

		random_device rd;
		mt19937 gen(rd()); // 난수 생성 엔진
		uniform_real_distribution<float> dis(-100.0f, 100.0f); // 각도 범위: -45도 ~ 45도

		//float angles[] = { -30.f, 0.f, 30.f };

		for (UINT i = 0; i < 3; ++i)
		{
			float randomAngle = dis(gen);
			Vec2 Dir = RotateVector(m_Dir, randomAngle);

			vector<CScript*> vecBloodScript = BloodPrefab->GetProtoObject()->GetScripts();
			for (UINT i = 0; i < vecBloodScript.size(); ++i)
			{
				if (SCRIPT_TYPE::BLOODSCRIPT == vecBloodScript[i]->GetScriptType())
				{
					CBloodScript* pScript = dynamic_cast<CBloodScript*>(vecBloodScript[i]);
					pScript->SetDir(Dir);
					pScript->SetBase(false);
				}
			}

			Instantiate(BloodPrefab, vPos, 7);
		}
		m_IsBase = false;
	}

	vPos.x += m_Speed * m_Dir.x * DT;
	vPos.y += m_Speed * m_Dir.y * DT;


	GetOwner()->Transform()->SetRelativePos(vPos);
}

void CBloodScript::MakeSplat()
{
	Vec3 vPos = Transform()->GetRelativePos();
	m_SpawnTime += DT;

	Ptr<CPrefab> BloodPrefab = nullptr;

	random_device rd;
	mt19937 gen(rd()); // 난수 생성 엔진
	uniform_int_distribution<int> Type(0, 4); 

	int BloodType = Type(gen);

	switch (BloodType)
	{
	case 0:
		BloodPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\SplatBlood_0.pref", L"Prefab\\SplatBlood_0.pref");
		break;
	case 1:
		BloodPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\SplatBlood_1.pref", L"Prefab\\SplatBlood_1.pref");
		break;
	case 2:
		BloodPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\SplatBlood_2.pref", L"Prefab\\SplatBlood_2.pref");
		break;
	case 3:
		BloodPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\SplatBlood_3.pref", L"Prefab\\SplatBlood_3.pref");
		break;
	case 4:
		BloodPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\SplatBlood_4.pref", L"Prefab\\SplatBlood_4.pref");
		break;
	}
	
	if (0.4f < m_SpawnTime)
	{
		Instantiate(BloodPrefab, vPos, 7);
		m_SpawnTime = 0.f;
	}	
}

void CBloodScript::BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CBloodScript::Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CBloodScript::EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CBloodScript::SaveComponent(FILE* _File)
{
}

void CBloodScript::LoadComponent(FILE* _File)
{
}

Vec2 CBloodScript::RotateVector(const Vec2& _vec, float Angle)
{
	float radians = Angle * (3.14f / 180.0f); // 각도를 라디안으로 변환
	float cosTheta = cos(radians);
	float sinTheta = sin(radians);

	return Vec2(_vec.x * cosTheta - _vec.y * sinTheta,
		_vec.x * sinTheta + _vec.y * cosTheta);
}

