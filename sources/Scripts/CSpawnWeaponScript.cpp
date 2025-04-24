#include "pch.h"
#include "CSpawnWeaponScript.h"

CSpawnWeaponScript::CSpawnWeaponScript()
	: CScript(SCRIPT_TYPE::SPAWNWEAPONSCRIPT)
{
}

CSpawnWeaponScript::~CSpawnWeaponScript()
{
}

void CSpawnWeaponScript::Begin()
{
	random_device rd;
	mt19937 gen(rd()); // 난수 생성 엔진
	uniform_int_distribution<int> numb(1, 10); // 각도 범위: -45도 ~ 45도

	//float angles[] = { -30.f, 0.f, 30.f };

	int WeaponInt = numb(gen);

	Ptr<CPrefab> WeaponPrefab = nullptr;

	switch ((WEAPON_TYPE)WeaponInt)
	{
	case WEAPON_TYPE::NONE:
		break;
	case WEAPON_TYPE::KNIFE:
		WeaponPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\Knife.pref", L"Prefab\\Knife.pref");
		break;
	case WEAPON_TYPE::LONG_PIPE:
		WeaponPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\Pipe.pref", L"Prefab\\Pipe.pref");
		break;
	case WEAPON_TYPE::BAT:
		WeaponPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\Bat.pref", L"Prefab\\Bat.pref");
		break;
	case WEAPON_TYPE::KATANA:
		WeaponPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\Katana.pref", L"Prefab\\Katana.pref");
		break;
	case WEAPON_TYPE::AXE:
		WeaponPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\Axe.pref", L"Prefab\\Axe.pref");
		break;
	case WEAPON_TYPE::M16:
		WeaponPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\M16.pref", L"Prefab\\M16.pref");
		break;
	case WEAPON_TYPE::PISTOL:
	case WEAPON_TYPE::SMG:
		WeaponPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\SMG.pref", L"Prefab\\SMG.pref");
		break;
	case WEAPON_TYPE::DOUBLE_BARREL:
		WeaponPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\DB.pref", L"Prefab\\DB.pref");
		break;
	case WEAPON_TYPE::PUMP_SHOTGUN:
		WeaponPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\Shotgun.pref", L"Prefab\\Shotgun.pref");
		break;
	default:
		break;
	}

	Instantiate(WeaponPrefab, Transform()->GetRelativePos(), 4);
}

void CSpawnWeaponScript::Tick()
{
	DestroyObject(GetOwner());
}

void CSpawnWeaponScript::BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CSpawnWeaponScript::Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CSpawnWeaponScript::EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CSpawnWeaponScript::SaveComponent(FILE* _File)
{
}

void CSpawnWeaponScript::LoadComponent(FILE* _File)
{
}
