#include "pch.h"
#include "CEnemyScript.h"

#include <Engine/CLevelMgr.h>

#include "CWeaponScript.h"
#include "CMeleeAttackScript.h"
#include "CPlayerScript.h"
#include "CLevelScript.h"
#include "CDoorScript.h"
#include "CBloodScript.h"
#include "CCameraScript.h"

#include "CState.h"
#include "CStateMgr.h"
#include "CLeanState.h"


CEnemyScript::CEnemyScript()
	: CScript(SCRIPT_TYPE::ENEMYSCRIPT)
	, m_CurState(nullptr)
	, m_CurWeaponInt(0)
	, m_CurWeaponType(WEAPON_TYPE::NONE)
	, m_EquipGun(false)
	, m_DeadByDoor(false)
	, m_Bleeding(true)
	, m_MakePool(true)
	, m_IsMounted(false)
	, m_bGiveup(false)
	, m_bDead(false)
	, m_bPlayerMakeBody(false)
	, m_SpawnPos()
	, m_DestPos()
	, m_StartState(STATE_TYPE::NONE)
{
}

CEnemyScript::~CEnemyScript()
{
	if (m_CurState)
	{
		delete m_CurState;
		m_CurState = nullptr;
	}
}

void CEnemyScript::Begin()
{
	if (m_CurState)
	{
		m_CurState->SetOwner(this);
		m_CurState->Enter();		
		m_StartState = m_CurState->GetStateType();
	}

	if (m_CurWeaponType == WEAPON_TYPE::M16 || m_CurWeaponType == WEAPON_TYPE::PUMP_SHOTGUN
		|| m_CurWeaponType == WEAPON_TYPE::DOUBLE_BARREL)
		m_EquipGun = true;
	if (m_EquipGun)
	{
		if (m_CurWeaponType == WEAPON_TYPE::M16)
			m_CurWeaponRounds = 30;
		if (m_CurWeaponType == WEAPON_TYPE::DOUBLE_BARREL)
			m_CurWeaponRounds = 2;
		if (m_CurWeaponType == WEAPON_TYPE::PUMP_SHOTGUN)
			m_CurWeaponRounds = 6;
	}

	m_SpawnPos = Transform()->GetRelativePos();
	if (m_bPlayerMakeBody)
	{
		SetState(CStateMgr::GetState((UINT)STATE_TYPE::PLAYERMAKEDEADSTATE));
	}
}

void CEnemyScript::Tick()
{
	CPlayerScript* pPlayerScript = static_cast<CPlayerScript*>(CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScripts()[0]);
	// Player 가 정지 상태라면 (메뉴) 동작을 하지 않음.
	if (pPlayerScript->IsPaued())
	{
		return;
	}
		

	if (m_CurState != nullptr)
	{
		// 현재 상태의 동작을 수행한다.
		m_CurState->FinalTick();

		CLevelScript* pScript = (CLevelScript*)CLevelMgr::GetInst()->FindObjectByName(L"LevelInfo")->GetScripts()[0];

		if (m_CurState->GetStateType() == STATE_TYPE::DEADSTATE || m_CurState->GetStateType() == STATE_TYPE::DYINGSTATE || m_CurState->GetStateType() == STATE_TYPE::PLAYERMAKEDEADSTATE)
		{
			if (!m_IsMounted)
			{
				if (m_Bleeding)
				{
					if (!pScript->IsAlreadyClear())
					{
						MakeBlood();
					}					
				}
				m_SpawnTime += DT;

				if (!m_bDead)
				{
					if (0.3f < m_SpawnTime)
						m_bDead = true;
				}

				if (1.f < m_SpawnTime)
				{
					if (m_MakePool)
					{
						GetOwner()->SetName(L"DeadBody");
						Vec3 vPos = Transform()->GetRelativePos();
						vPos.z = 50.f;
						Transform()->SetRelativePos(vPos);

						if(!pScript->IsAlreadyClear())
						{
							pScript->CheckFloorClear();
							MakePool();
						}
							
						
						
						m_MakePool = false;						
					}
				}
			}
		}
	}
	//else
	//{
	//	//if (m_Bleeding)
	//	//{
	//	//	MakeBlood();
	//	//	
	//	//}
	//	//m_SpawnTime += DT;

	//	//if (1.f < m_SpawnTime)
	//	//{
	//	//	if (m_MakePool)
	//	//	{
	//	//		GetOwner()->SetName(L"DeadBody");
	//	//		

	//	//		CLevelScript* pScript = (CLevelScript*)CLevelMgr::GetInst()->FindObjectByName(L"LevelInfo")->GetScripts()[0];
	//	//		pScript->CheckFloorClear();
	//	//		MakePool();
	//	//		m_MakePool = false;
	//	//		m_bDead = true;
	//	//	}
	//	//}
	//}
}

void CEnemyScript::BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	if (m_CurState == nullptr)
		return;

	if (m_CurState->GetStateType() == STATE_TYPE::KNOCKDOWNSTATE)
	{
		if (_OtherCollider->GetName() == L"Wall")
		{
			Vec3 vRot = Transform()->GetRelativeRotation();
			Vec3 WallPos = _OtherObject->Transform()->GetRelativePos();
			Vec3 WallScale = _OtherObject->Transform()->GetRelativeScale();
			Vec3 WallRot = _OtherObject->Transform()->GetRelativeRotation();

			float Theta = CalcWallVecPos(WallPos, WallScale, WallRot);
			vRot.z = Theta * (XM_PI / 180.f);


			Transform()->SetRelativeRotation(vRot);

			m_CurState = CStateMgr::ChangeState(m_CurState, L"CLeanState");
			CLeanState* pState = (CLeanState*)m_CurState;
		}
	}


	if (m_CurState->GetStateType() == STATE_TYPE::KNOCKDOWNSTATE
		|| m_CurState->GetStateType() == STATE_TYPE::DEADSTATE
		|| m_CurState->GetStateType() == STATE_TYPE::DYINGSTATE
		|| m_CurState->GetStateType() == STATE_TYPE::LEANSTATE
		|| m_CurState->GetStateType() == STATE_TYPE::PLAYERMAKEDEADSTATE)
	{
		return;
	}

	if (_OtherCollider->GetName() == L"Bullet")
	{
		m_CurState = CStateMgr::ChangeState(m_CurState, L"CDeadState");
		DropWeapon();
	}

	if (_OtherCollider->GetName() == L"MeleeAttack")
	{
		CMeleeAttackScript* pScript = (CMeleeAttackScript*)_OtherObject->GetScripts()[0];
		WEAPON_TYPE Type = pScript->GetCurWeapon();
		CPlayerScript* pPlayerScript = (CPlayerScript*)CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScripts()[0];
		CCameraScript* pCameraScript = (CCameraScript*)CLevelMgr::GetInst()->FindObjectByName(L"MainCamera")->GetScripts()[0];

		// 맨주먹
		if (Type == WEAPON_TYPE::NONE && pPlayerScript->GetMask() != MASK_TYPE::TONY_TIGER)
		{
			// 소리 재생
			Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\attack\\Punch.wav", L"Sound\\attack\\Throw.wav");
			pSound->Play(1, 0.5f, true);
			m_CurState = CStateMgr::ChangeState(m_CurState, L"CKnockDownState");
			DropWeapon();
		}
		// 무기 && 토니 마스크
		else
		{
			if (Type == WEAPON_TYPE::KATANA || Type == WEAPON_TYPE::KNIFE)
			{
				// 소리 재생
				Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\attack\\Cut1.wav", L"Sound\\attack\\Cut1.wav");
				pSound->Play(1, 0.5f, true);
			}
			else
			{
				// 소리 재생
				Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\attack\\Hit.wav", L"Sound\\attack\\Hit.wav");
				pSound->Play(1, 0.5f, true);
			}

			m_CurState = CStateMgr::ChangeState(m_CurState, L"CDeadState");
			DropWeapon();
		}
		pCameraScript->SetShake(true);

	}

	if (_OtherCollider->GetName() == L"Weapon")
	{
		CWeaponScript* pScript = (CWeaponScript*)_OtherObject->GetScripts()[0];
		// Player가 던진 무기에 맞았다.
		if (pScript->IsThorwing())
		{
			// 소리 재생
			Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\attack\\Punch.wav", L"Sound\\attack\\Punch.wav");
			pSound->Play(1, 0.5f, false);
			m_CurState = CStateMgr::ChangeState(m_CurState, L"CKnockDownState");
			DropWeapon();
		}
	}

	if (_OtherCollider->GetName() == L"Door")
	{
		CDoorScript* pScript = (CDoorScript*)_OtherObject->GetScripts()[0];
		CPlayerScript* pPlayerScript = (CPlayerScript*)CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScripts()[0];

		// 플레이어가 연 문에 맞았다.
		if (pScript->IsPlayerOpening())
		{
			if (pPlayerScript->GetMask() == MASK_TYPE::JUAN_HORSE)
			{
				// 소리 재생
				Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\etc\\DoorHit.wav", L"Sound\\attack\\DoorHit.wav");
				pSound->Play(1, 0.5f, true);
				m_DeadByDoor = true;
				m_CurState = CStateMgr::ChangeState(m_CurState, L"CDeadState");
				DropWeapon();
			}
			else
			{
				// 소리 재생
				Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\etc\\DoorHit.wav", L"Sound\\attack\\DoorHit.wav");
				pSound->Play(1, 0.5f, true);
				m_CurState = CStateMgr::ChangeState(m_CurState, L"CKnockDownState");
				DropWeapon();
			}						
		}
	}
}

void CEnemyScript::Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CEnemyScript::EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CEnemyScript::SaveComponent(FILE* _File)
{
	fwrite(&m_CurWeaponType, sizeof(int), 1, _File);
	CStateMgr::SaveState(m_CurState, _File);
}

void CEnemyScript::LoadComponent(FILE* _File)
{
	fread(&m_CurWeaponType, sizeof(int), 1, _File);
	CStateMgr::LoadState(m_CurState, _File);
}

void CEnemyScript::SetWeapon(int _Weapon)
{
	if (0 == _Weapon)
	{
		m_CurWeaponType = WEAPON_TYPE::NONE;
		m_EquipGun = false;
	}
	if (1 == _Weapon)
	{
		m_CurWeaponType = WEAPON_TYPE::KNIFE;
		m_EquipGun = false;
	}
	if (2 == _Weapon)
	{
		m_CurWeaponType = WEAPON_TYPE::LONG_PIPE;
		m_EquipGun = false;
	}
	if (3 == _Weapon)
	{
		m_CurWeaponType = WEAPON_TYPE::BAT;
		m_EquipGun = false;
	}
	if (4 == _Weapon)
	{
		m_CurWeaponType = WEAPON_TYPE::M16;
		m_EquipGun = true;
	}
	if (5 == _Weapon)
	{
		m_CurWeaponType = WEAPON_TYPE::DOUBLE_BARREL;
		m_EquipGun = true;
	}
	if (6 == _Weapon)
	{
		m_CurWeaponType = WEAPON_TYPE::PUMP_SHOTGUN;
		m_EquipGun = true;
	}
}

void CEnemyScript::EquipWeapon(CGameObject* _Weapon)
{
	wstring WeaponType = _Weapon->GetName();
	m_CurWeaponRounds = 0;

	if (WeaponType == L"Axe")
	{
		m_CurWeaponType = WEAPON_TYPE::AXE;
		m_EquipGun = false;
	}
	if (WeaponType == L"Bat")
	{
		m_CurWeaponType = WEAPON_TYPE::BAT;
		m_EquipGun = false;
	}
	if (WeaponType == L"DB")
	{
		m_CurWeaponType = WEAPON_TYPE::DOUBLE_BARREL;
		m_CurWeaponRounds = 2;
		m_EquipGun = true;
	}
	if (WeaponType == L"Katana")
	{
		m_CurWeaponType = WEAPON_TYPE::KATANA;
		m_EquipGun = false;
	}
	if (WeaponType == L"Knife")
	{
		m_CurWeaponType = WEAPON_TYPE::KNIFE;
		m_EquipGun = false;
	}
	if (WeaponType == L"Pipe")
	{
		m_CurWeaponType = WEAPON_TYPE::LONG_PIPE;
		m_EquipGun = false;
	}
	if (WeaponType == L"Shotgun")
	{
		m_CurWeaponType = WEAPON_TYPE::PUMP_SHOTGUN;
		m_CurWeaponRounds = 6;
		m_EquipGun = true;
	}
	if (WeaponType == L"SMG")
	{
		m_CurWeaponType = WEAPON_TYPE::SMG;
		m_CurWeaponRounds = 30;
		m_EquipGun = true;
	}
	if (WeaponType == L"M16")
	{
		m_CurWeaponType = WEAPON_TYPE::M16;
		m_CurWeaponRounds = 30;
		m_EquipGun = true;
	}

	vector<CScript*> vecScript = _Weapon->GetScripts();
	for (UINT i = 0; i < vecScript.size(); ++i)
	{
		if (SCRIPT_TYPE::WEAPONSCRIPT == vecScript[i]->GetScriptType())
		{
			CWeaponScript* pScript = dynamic_cast<CWeaponScript*>(vecScript[i]);
			if (!pScript->IsFirstUsed() || pScript->IsEnemyUsed())
				m_CurWeaponRounds = pScript->GetRound();
			pScript->SetEquip(true);
		}
	}
}


void CEnemyScript::DropWeapon()
{
	Ptr<CPrefab> WeaponPrefab = nullptr;

	switch (m_CurWeaponType)
	{
	case WEAPON_TYPE::KNIFE:
		WeaponPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\Knife.pref", L"Prefab\\Knife.pref");
		break;
	case WEAPON_TYPE::LONG_PIPE:
		WeaponPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\Pipe.pref", L"Prefab\\Pipe.pref");
		break;
	case WEAPON_TYPE::BAT:
		WeaponPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\Bat.pref", L"Prefab\\Bat.pref");
		break;
	case WEAPON_TYPE::M16:
		WeaponPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\M16.pref", L"Prefab\\M16.pref");
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

	Vec3 EnemyPos = Transform()->GetRelativePos();
	float SpawnTheta = Transform()->GetRelativeRotation().z;

	Vec2 Dir = Vec2(cos(SpawnTheta), sin(SpawnTheta));
	Dir.Normalize();

	Vec3 SpawnPos = Vec3(EnemyPos.x + Dir.x * 10.f, EnemyPos.y + Dir.y * 10.f, 0.f);

	if (nullptr != WeaponPrefab)
	{
		vector<CScript*> vecWeaponScript = WeaponPrefab->GetProtoObject()->GetScripts();
		for (UINT i = 0; i < vecWeaponScript.size(); ++i)
		{
			if (SCRIPT_TYPE::WEAPONSCRIPT == vecWeaponScript[i]->GetScriptType())
			{
				CWeaponScript* pScript = dynamic_cast<CWeaponScript*>(vecWeaponScript[i]);
				pScript->SetDir(Dir);
				pScript->SetDrop(true);
				pScript->SetThrow(false);
				pScript->SetRound(m_CurWeaponRounds);
				pScript->SetEnemyUse(true);
			}
		}
		Instantiate(WeaponPrefab, SpawnPos, 4);
	}

	m_CurWeaponType = WEAPON_TYPE::NONE;
}

void CEnemyScript::MakeBlood()
{
	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 EnemyPos = Transform()->GetRelativePos();
	float SpawnTheta = Transform()->GetRelativeRotation().z;

	Vec2 Dir = Vec2(cos(SpawnTheta), sin(SpawnTheta));
	Dir.Normalize();

	Vec3 SpawnPos = Vec3(EnemyPos.x + Dir.x * 30.f, EnemyPos.y + Dir.y * 30.f, 0.f);

	Ptr<CPrefab> BloodSquirtPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\SquirtBlood.pref", L"Prefab\\SquirtBlood.pref");
	CBloodScript* pScript = (CBloodScript*)BloodSquirtPrefab->GetProtoObject()->GetScripts()[0];
	pScript->SetSpread(true);
	pScript->SetBase(true);
	pScript->SetDir(Dir);

	Instantiate(BloodSquirtPrefab, SpawnPos, 7);
	m_Bleeding = false;
}

void CEnemyScript::MakePool()
{
	Vec3 vPos = Transform()->GetRelativePos();

	Ptr<CPrefab> BloodPPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\BloodPool.pref", L"Prefab\\BloodPool.pref");
	BloodPPrefab->GetProtoObject()->FlipbookPlayer()->Play(0, 40, false);

	Instantiate(BloodPPrefab, vPos, 7);
}

float CEnemyScript::CalcWallVecPos(Vec3 _WallPos, Vec3 _WallScale, Vec3 _WallRotation)
{
	Vec3 vPos = Transform()->GetRelativePos();
	Vec2 NewPosXY = Vec2(0.f, 0.f);

	float Theta = 0.f;
	float WallTheta = _WallRotation.z * (180.f / XM_PI);

	float MaxWallPosX = 0.f;
	float MinWallPosX = 0.f;

	float MaxWallPosY = 0.f;
	float MinWallPosY = 0.f;

	// 수평
	if (WallTheta == 90.f)
	{
		MaxWallPosX = _WallPos.x + _WallScale.y / 2.f;
		MinWallPosX = _WallPos.x - _WallScale.y / 2.f;

		MaxWallPosY = _WallPos.y + _WallScale.x / 2.f;
		MinWallPosY = _WallPos.y - _WallScale.x / 2.f;
	}
	// 수직
	else
	{
		MaxWallPosX = _WallPos.x + _WallScale.x / 2.f;
		MinWallPosX = _WallPos.x - _WallScale.x / 2.f;

		MaxWallPosY = _WallPos.y + _WallScale.y / 2.f;
		MinWallPosY = _WallPos.y - _WallScale.y / 2.f;
	}

	// 벽의 오른쪽
	if (vPos.x > MaxWallPosX)
	{
		Theta = 180.f;
		vPos.x = MaxWallPosX + 20.f;
	}

	// 벽의 왼쪽
	else if (vPos.x < MinWallPosX)
	{
		Theta = 0.f;
		vPos.x = MinWallPosX - 20.f;
	}

	// 벽의 위쪽
	if (vPos.y > MaxWallPosY)
	{
		Theta = -90.f;
		vPos.y = MaxWallPosY + 20.f;
	}

	// 벽의 아래쪽
	else if (vPos.y < MinWallPosY)
	{
		Theta = 90.f;
		vPos.y = MinWallPosY - 20.f;
	}

	// 위치를 벽에 고정해준다.
	Transform()->SetRelativePos(vPos);

	return Theta;
}

void CEnemyScript::PlayIdleFlip()
{
 	switch (m_CurWeaponType)
	{
	case WEAPON_TYPE::NONE:
		GetOwner()->FlipbookPlayer()->Play(6, 1, true);
		break;
	case WEAPON_TYPE::KNIFE:
		GetOwner()->FlipbookPlayer()->Play(9, 1, true);
		break;
	case WEAPON_TYPE::LONG_PIPE:
		GetOwner()->FlipbookPlayer()->Play(11, 1, true);
		break;
	case WEAPON_TYPE::BAT:
		GetOwner()->FlipbookPlayer()->Play(7, 1, true);
		break;
	case WEAPON_TYPE::KATANA:
		break;
	case WEAPON_TYPE::AXE:
		break;
	case WEAPON_TYPE::M16:
		GetOwner()->FlipbookPlayer()->Play(10, 1, true);
		break;
	case WEAPON_TYPE::PISTOL:
		break;
	case WEAPON_TYPE::SMG:
		break;
	case WEAPON_TYPE::DOUBLE_BARREL:
		GetOwner()->FlipbookPlayer()->Play(8, 1, true);
		break;
	case WEAPON_TYPE::PUMP_SHOTGUN:
		GetOwner()->FlipbookPlayer()->Play(12, 1, true);
		break;
	}
}

void CEnemyScript::PlayWalkFlip()
{
	switch (m_CurWeaponType)
	{
	case WEAPON_TYPE::NONE:
		GetOwner()->FlipbookPlayer()->Play(19, 10, true);
		break;
	case WEAPON_TYPE::KNIFE:
		GetOwner()->FlipbookPlayer()->Play(22, 10, true);
		break;
	case WEAPON_TYPE::LONG_PIPE:
		GetOwner()->FlipbookPlayer()->Play(24, 10, true);
		break;
	case WEAPON_TYPE::BAT:
		GetOwner()->FlipbookPlayer()->Play(20, 10, true);
		break;
	case WEAPON_TYPE::KATANA:
		break;
	case WEAPON_TYPE::AXE:
		break;
	case WEAPON_TYPE::M16:
		GetOwner()->FlipbookPlayer()->Play(23, 10, true);
		break;
	case WEAPON_TYPE::PISTOL:
		break;
	case WEAPON_TYPE::SMG:
		break;
	case WEAPON_TYPE::DOUBLE_BARREL:
		GetOwner()->FlipbookPlayer()->Play(21, 10, true);
		break;
	case WEAPON_TYPE::PUMP_SHOTGUN:
		GetOwner()->FlipbookPlayer()->Play(25, 10, true);
		break;
	}
}

void CEnemyScript::PlayAttackFlip()
{
	switch (m_CurWeaponType)
	{
	case WEAPON_TYPE::NONE:
		break;
	case WEAPON_TYPE::KNIFE:
		GetOwner()->FlipbookPlayer()->Play(2, 8, false);
		break;
	case WEAPON_TYPE::LONG_PIPE:
		GetOwner()->FlipbookPlayer()->Play(4, 10, false);
		break;
	case WEAPON_TYPE::BAT:
		GetOwner()->FlipbookPlayer()->Play(0, 10, false);
		break;
	case WEAPON_TYPE::KATANA:
		break;
	case WEAPON_TYPE::AXE:
		break;
	case WEAPON_TYPE::M16:
		GetOwner()->FlipbookPlayer()->Play(3, 10, false);
		break;
	case WEAPON_TYPE::PISTOL:
		break;
	case WEAPON_TYPE::SMG:
		break;
	case WEAPON_TYPE::DOUBLE_BARREL:
		GetOwner()->FlipbookPlayer()->Play(1, 10, false);
		break;
	case WEAPON_TYPE::PUMP_SHOTGUN:
		GetOwner()->FlipbookPlayer()->Play(5, 10, false);
		break;
	}
}

void CEnemyScript::PlaySearchFlip()
{
	switch (m_CurWeaponType)
	{
	case WEAPON_TYPE::NONE:
		break;
	case WEAPON_TYPE::KNIFE:
		GetOwner()->FlipbookPlayer()->Play(15, 5, true);
		break;
	case WEAPON_TYPE::LONG_PIPE:
		GetOwner()->FlipbookPlayer()->Play(17, 5, true);
		break;
	case WEAPON_TYPE::BAT:
		GetOwner()->FlipbookPlayer()->Play(13, 5, true);
		break;
	case WEAPON_TYPE::KATANA:
		break;
	case WEAPON_TYPE::AXE:
		break;
	case WEAPON_TYPE::M16:
		GetOwner()->FlipbookPlayer()->Play(16, 5, true);
		break;
	case WEAPON_TYPE::PISTOL:
		break;
	case WEAPON_TYPE::SMG:
		break;
	case WEAPON_TYPE::DOUBLE_BARREL:
		GetOwner()->FlipbookPlayer()->Play(14, 5, true);
		break;
	case WEAPON_TYPE::PUMP_SHOTGUN:
		GetOwner()->FlipbookPlayer()->Play(18, 5, true);
		break;
	}
}


void CEnemyScript::PlaySpecialFlip(int _Idx)
{
	switch (_Idx)
	{
	case 0:
		// GetUp
		GetOwner()->FlipbookPlayer()->Play(26, 10, false);
		break;
	case 1:
		// KnockDown
		GetOwner()->FlipbookPlayer()->Play(27, 12, false);
		break;
	case 2:
		// LeanDown
		GetOwner()->FlipbookPlayer()->Play(28, 10, false);
		break;
	case 3:
		// LeanUP
		GetOwner()->FlipbookPlayer()->Play(29, 10, false);
		break;
	case 4:
		//Giveup
		GetOwner()->FlipbookPlayer()->Play(45, 6, false);
		break;
	default:
		break;
	}

}

void CEnemyScript::PlayDyingFlip(int _Idx)
{
	switch(_Idx)
	{
	case 0:
		// 칼에 의한 확킬
		GetOwner()->FlipbookPlayer()->Play(33, 4, false);
		break;
	case 1:
		// 도끼에 의한 확킬
		GetOwner()->FlipbookPlayer()->Play(37, 4, false);
		break;
	case 2:
		// 파이프, 배트에 의한 확킬 1
		GetOwner()->FlipbookPlayer()->Play(38, 4, false);
		break;
	case 3:
		// 파이프, 배트에 의한 확킬 2
		GetOwner()->FlipbookPlayer()->Play(39, 4, false);
		break;
	case 4:
		// 파이프, 배트에 의한 확킬 3
		GetOwner()->FlipbookPlayer()->Play(40, 4, false);
		break;
	case 5:
		// 암살
		GetOwner()->FlipbookPlayer()->Play(43, 4, false);
		break;
	}
}

void CEnemyScript::PlayDeadFlip(int _Idx)
{
	switch (_Idx)
	{
	case 0:
		// 복부상해
		GetOwner()->FlipbookPlayer()->Play(30, 10, false);
		break;
	case 1:
		// 머리출혈
		GetOwner()->FlipbookPlayer()->Play(31, 10, false);
		break;
	case 2:
		// 머리 총상
		GetOwner()->FlipbookPlayer()->Play(32, 10, false);
		break;
	case 3:
		// 상반신 사라짐
		GetOwner()->FlipbookPlayer()->Play(34, 10, false);
		break;
	case 4:
		// 칼에 의한
		GetOwner()->FlipbookPlayer()->Play(41, 10, false);
		break;
	case 5:
		// 토니 주먹
		GetOwner()->FlipbookPlayer()->Play(42, 10, false);
		break;
	case 6:
		// 백 헤드샷
		GetOwner()->FlipbookPlayer()->Play(36, 10, false);
		break;
	case 7:
		// 암살
		GetOwner()->FlipbookPlayer()->Play(35, 10, false);
		break;
	case 8:
		// 발차기
		GetOwner()->FlipbookPlayer()->Play(44, 10, false);
		break;
	}
}

