#include "pch.h"
#include "CBikerScript.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CAssetMgr.h>
#include <Engine/CSound.h>
#include <Engine/CFontMgr.h>

#include "CPlayerScript.h"
#include "CWeaponScript.h"
#include "CMeleeAttackScript.h"
#include "CBloodScript.h"
#include "CLevelScript.h"


CBikerScript::CBikerScript()
	: CScript(SCRIPT_TYPE::BIKERSCRIPT)
	, m_State(BIKER_STATE::COMPUTER)
	, m_Speed(400.f)
	, m_DetectRange(400.f)
	, m_KillRange(50.f)
	, m_AccTime(0.f)
	, m_bEquipWeapon(true)
	, m_bStart(false)
	, m_bPlayFlip(false)
	, m_AttackCount(0)
	, m_DownCount(0)
	, m_AttackDelay(0.f)
	, m_CanBash(false)
	, m_bDead(false)
	, m_XBlock(false)
	, m_YBlock(false)
	, m_bBGMChange(false)
	, m_BossBGM(nullptr)
	, m_bSound(false)
	, m_ActiveScript(false)
	, m_ScriptCount(0)
	, m_CrawlActiveScript(false)
	, m_ScriptPPActive(false)
	, m_StartSciptActive(false)
{
}

CBikerScript::~CBikerScript()
{
}


void CBikerScript::Begin()
{
	PlayComputeFlip();
}

void CBikerScript::Tick()
{
	CGameObject* pPlayer = CLevelMgr::GetInst()->FindObjectByName(L"Player");
	CPlayerScript* pPlayerScript = (CPlayerScript*)pPlayer->GetScripts()[0];
	if (pPlayerScript->IsPaued())
		return;

	m_PlayerPos = pPlayer->Transform()->GetRelativePos();
	m_PlayerRot = pPlayer->Transform()->GetRelativeRotation();

	m_BikerPos = Transform()->GetRelativePos();
	m_BikerRot = Transform()->GetRelativeRotation();

	m_DirToPlayer = Vec2(m_PlayerPos.x - m_BikerPos.x, m_PlayerPos.y - m_BikerPos.y);
	m_DirToPlayer.Normalize();

	
	float Distance = m_BikerPos.Distance(m_BikerPos, m_PlayerPos);

	// 플레이어가 탐지범위 내로 들어와 스크립트가 시작된다.
	if (Distance < m_DetectRange && !m_bStart)
	{
		m_State == BIKER_STATE::IDLE;
		// 플레이어를 바라보게 한다.
		m_BikerRot.z = atan2(m_DirToPlayer.y, m_DirToPlayer.x);
		Transform()->SetRelativeRotation(m_BikerRot);
		
		// 대화문이 활성화
		if (!m_StartSciptActive)
		{
			m_ActiveScript = true;
			m_StartSciptActive = true;
		}
		
		// 스크립트 시작
		if (m_ActiveScript)
		{
			CGameObject* pPhoneFace = (CGameObject*)CLevelMgr::GetInst()->FindObjectByName(L"BikerFace");
			pPhoneFace->Transform()->SetRelativeScale(Vec3(250.f, 250.f, 0.f));
			PrintScript(0);

			if (KEY_TAP(KEY::ENTER) || KEY_TAP(KEY::LBTN))
			{
				m_ScriptCount += 1;
			}
		}
		else
		{
			CGameObject* pPhoneFace = (CGameObject*)CLevelMgr::GetInst()->FindObjectByName(L"BikerFace");
			pPhoneFace->Transform()->SetRelativeScale(Vec3(0.f, 0.f, 0.f));
			m_bStart = true;
			ChangeState(BIKER_STATE::WALK);
		}
	}



	if (m_bStart)
	{
		if (!m_bBGMChange)
		{
			CLevelScript* pScript = (CLevelScript*)CLevelMgr::GetInst()->FindObjectByName(L"LevelInfo")->GetScripts()[0];
			Ptr<CSound> CurBGM = pScript->GetBGM();
			if(CurBGM != nullptr)
				CurBGM->Stop();
			m_BossBGM = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\bgm\\Boss.mp3", L"Sound\\bgm\\Boss.mp3");
			int Idx = m_BossBGM->Play(0, 0.4f, false);
			pScript->SetBGMIdx(Idx);
			pScript->SetBGM(m_BossBGM);
			m_bBGMChange = true;
		}


		switch (m_State)
		{
		case BIKER_STATE::WALK:
			WalkTo();
			break;
		case BIKER_STATE::KILL:
			Kill();
			break;
		case BIKER_STATE::ATTACK:
			Attack();
			break;
		case BIKER_STATE::THROW:
			Attack();
			break;
		case BIKER_STATE::PRY:
			Pry();
			break;
		case BIKER_STATE::DOWN:
			Down();
			break;
		case BIKER_STATE::GETUP:
			GetUp();
			break;
		case BIKER_STATE::CRAWL:
			Crawl();
			break;
		case BIKER_STATE::BASH:
			Bash();
			break;
		default:
			break;
		}
	}
}

void CBikerScript::BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	// 무기를 뽑는 도중 공격을 당하면 Down
	if (m_State == BIKER_STATE::PRY)
	{

		if (_OtherCollider->GetName() == L"MeleeAttack")
		{
			CMeleeAttackScript* pScript = (CMeleeAttackScript*)_OtherObject->GetScripts()[0];
			WEAPON_TYPE Type = pScript->GetCurWeapon();
			if (Type != WEAPON_TYPE::NONE)
			{
				Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\attack\\Hit.wav", L"Sound\\attack\\Hit.wav");
				pSound->Play(1, 0.5f, true);
				ChangeState(BIKER_STATE::DOWN);
				MakeBlood();
			}			
		}
	}

	if (m_State == BIKER_STATE::CRAWL)
	{
		if (m_CanBash)
		{
			if (_OtherCollider->GetName() == L"Player")
			{
				ChangeState(BIKER_STATE::BASH);
			}
		}
	}


	if (_OtherCollider->GetName() == L"Wall")
	{
		Vec4 MaxMinPos = GetMaxMinPosWall(_OtherObject);

		float WallMaxX = MaxMinPos.x;
		float WallMinX = MaxMinPos.z;

		float WallMaxY = MaxMinPos.y;
		float WallMinY = MaxMinPos.w;

		// 가로 벽
		if (m_BikerPos.y < WallMaxY && WallMinY < m_BikerPos.y)
		{
			m_XBlock = true;
		}
		else if (m_BikerPos.x < WallMaxX && WallMinX < m_BikerPos.x)
		{
			m_YBlock = true;
		}
	}
}

void CBikerScript::Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{

}

void CBikerScript::EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	if (_OtherCollider->GetName() == L"Wall")
	{
		Vec4 MaxMinPos = GetMaxMinPosWall(_OtherObject);

		float WallMaxX = MaxMinPos.x;
		float WallMinX = MaxMinPos.z;

		float WallMaxY = MaxMinPos.y;
		float WallMinY = MaxMinPos.w;

		// 가로 벽
		if (m_BikerPos.y < WallMaxY && WallMinY < m_BikerPos.y)
		{
			m_XBlock = false;
		}
		else if (m_BikerPos.x < WallMaxX && WallMinX < m_BikerPos.x)
		{
			m_YBlock = false;
		}
	}
}

void CBikerScript::SaveComponent(FILE* _File)
{
}

void CBikerScript::LoadComponent(FILE* _File)
{
}

void CBikerScript::WalkTo()
{
	m_PlayerPos = CLevelMgr::GetInst()->FindObjectByName(L"Player")->Transform()->GetRelativePos();

	// 애니메이션 재생
	if (!m_bPlayFlip)
	{
		PlayWalkFlip(m_bEquipWeapon);
		m_bPlayFlip = true;
	}

	static Vec3 PrevPlayerPos = m_PlayerPos;

	// 무기를 착용하고 있다면 플레이어와 대치
	if (m_bEquipWeapon)
	{
		m_AttackDelay += DT;
		float Distance = m_BikerPos.Distance(m_BikerPos, m_PlayerPos);

		// 원형으로 대치한다.
		static float orbitAngle = 0.f;
		float radius = 120.f;

		// 특정 거리 까지 가까워진다.
		if (radius < Distance)
		{
			m_BikerPos.x += m_Speed * m_DirToPlayer.x * DT;
			m_BikerPos.y += m_Speed * m_DirToPlayer.y * DT;

			Transform()->SetRelativePos(m_BikerPos);
		}
		// 특정 거리가 되면 원을 그리며 대치한다
		else
		{
			float rotationSpeed = 3.f;

			Vec3 PlayerDelta = m_PlayerPos - PrevPlayerPos;
			//if (!m_XBlock)
			//	m_BikerPos.x += PlayerDelta.x;
			//if (!m_YBlock)
			//	m_BikerPos.y += PlayerDelta.y;

			orbitAngle += rotationSpeed * DT;
			if (orbitAngle > 2 * XM_PI)
				orbitAngle -= 2 * XM_PI;

			if(!m_XBlock)
				m_BikerPos.x += -radius * sin(orbitAngle) * DT * rotationSpeed;
			if(!m_YBlock)
				m_BikerPos.y -= radius * cos(orbitAngle) * DT * rotationSpeed;

			Transform()->SetRelativePos(m_BikerPos);

			// 일정 거리가 가까워지면 플레이어를 죽인다.
			if (Distance < m_KillRange)
			{
				ChangeState(BIKER_STATE::KILL);
			}
		}
		
		PrevPlayerPos = m_PlayerPos;

		m_BikerRot.z = atan2(m_DirToPlayer.y, m_DirToPlayer.x);		
		Transform()->SetRelativeRotation(m_BikerRot);

		if (4.f < m_AttackDelay)
		{
			m_AttackDelay = 0.f;
			ChangeState(BIKER_STATE::ATTACK);
		}
	}

	// 무기가 없다면 무기를 주우러 간다.
	else
	{
		CGameObject* pWeapon = CLevelMgr::GetInst()->FindObjectByName(L"Clamp");
		CWeaponScript* pScript = (CWeaponScript*)pWeapon->GetScripts()[0];
		Vec3 vWeaponPos = pWeapon->Transform()->GetRelativePos();
		m_BikerPos = Transform()->GetRelativePos();
		m_BikerRot = Transform()->GetRelativeRotation();

		Vec2 DirToWeapon = Vec2(vWeaponPos.x - m_BikerPos.x, vWeaponPos.y - m_BikerPos.y);
		DirToWeapon.Normalize();

		m_BikerPos.x += m_Speed * DirToWeapon.x * DT;
		m_BikerPos.y += m_Speed * DirToWeapon.y * DT;
		m_BikerRot.z = atan2(DirToWeapon.y, DirToWeapon.x);

		Transform()->SetRelativePos(m_BikerPos);
		Transform()->SetRelativeRotation(m_BikerRot);

		// 무기에 도달
		if (vWeaponPos.x - 5.f < m_BikerPos.x && m_BikerPos.x < vWeaponPos.x + 5.f
			&& vWeaponPos.y - 5.f < m_BikerPos.y && m_BikerPos.y < vWeaponPos.y + 5.f)
		{
			if (pScript->IsGround())
				ChangeState(BIKER_STATE::KILL);
			else
				ChangeState(BIKER_STATE::PRY);

			pScript->SetEquip(true);
		}
	}
}

void CBikerScript::Kill()
{

	// 플레이어에 도달
	if (m_PlayerPos.x - 20.f < m_BikerPos.x && m_BikerPos.x < m_PlayerPos.x + 20.f
		&& m_PlayerPos.y - 20.f < m_BikerPos.y && m_BikerPos.y < m_PlayerPos.y + 20.f)
	{
		// 애니메이션 재생
		if (!m_bPlayFlip)
		{
			PlayAttackFlip(0);
			MakeMeleeAttack();
			m_bPlayFlip = true;
		}		
	}
	else
	{
		m_BikerPos.x += m_Speed * m_DirToPlayer.x * DT;
		m_BikerPos.y += m_Speed * m_DirToPlayer.y * DT;

		m_BikerRot.z = atan2(m_DirToPlayer.y, m_DirToPlayer.x);

		Transform()->SetRelativePos(m_BikerPos);
		Transform()->SetRelativeRotation(m_BikerRot);
	}
}

void CBikerScript::Attack()
{
	// 애니메이션 재생
	if (!m_bPlayFlip)
	{
		if (m_AttackCount == 2)
		{
			PlayAttackFlip(1);
			m_bPlayFlip = true;
		}			
	}
	

	m_BikerRot.z = atan2(m_DirToPlayer.y, m_DirToPlayer.x);

	Transform()->SetRelativeRotation(m_BikerRot);

	if (m_AttackCount < 2)
	{
		// 제자리에 멈춰선다.
		m_AccTime += DT;
		if (m_AccTime < 0.1f)
		{
			PlayIdleFlip();
			if (!m_bDrawSound)
			{
				Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\boss\\DrawKnife.wav", L"Sound\\boss\\DrawKnife.wav");
				pSound->Play(1, 0.5f, true);
				m_bDrawSound = true;
			}			
		}
			

		// 앞으로 돌진하면서 공격 진행
		if (0.1f < m_AccTime)
		{
			if (!m_bPlayFlip)
			{
				PlayAttackFlip(0);
				m_bPlayFlip = true;
			}

			if (m_AccTime < 0.6f)
			{
				m_BikerPos.x += m_Speed * m_DirToPlayer.x * DT;
				m_BikerPos.y += m_Speed * m_DirToPlayer.y * DT;
				Transform()->SetRelativePos(m_BikerPos);
				MakeMeleeAttack();
			}
	
			
			// 최종 위치에 공격을 만든다.
			if (1.2f < m_AccTime)
			{
				MakeMeleeAttack();
				m_AttackCount += 1;
				m_bSound = false;
				m_bDrawSound = false;
				ChangeState(BIKER_STATE::WALK);
			}			
		}
	}

	else if (m_AttackCount ==  2)
	{
		// 제자리에 멈춰선다.
		m_AccTime += DT;
		if (!m_bDrawSound)
		{
			Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\boss\\DrawKnife.wav", L"Sound\\boss\\DrawKnife.wav");
			pSound->Play(1, 0.5f, true);
			m_bDrawSound = true;
		}

		// 무기를 던진다.
		if (0.5f < m_AccTime)
		{
			if(m_bEquipWeapon)
				ThrowWeapon();

			if (1.f < m_AccTime)
			{
				m_AttackCount = 0;
				m_bSound = false;
				m_bDrawSound = false;
				ChangeState(BIKER_STATE::WALK);
			}			
		}
	}
}


void CBikerScript::Pry()
{
	// 애니메이션 재생
	if (!m_bPlayFlip)
	{
		PlayPryFlip();
		m_bPlayFlip = true;
	}

	m_AccTime += DT;

	// 시간이 지나면 무기를 다시 획득
	if (2.f < m_AccTime)
	{
		m_bEquipWeapon = true;
		ChangeState(BIKER_STATE::WALK);
	}

}

void CBikerScript::GetUp()
{
	// 애니메이션 재생
	if (!m_bPlayFlip)
	{
		PlayGetUpFlip();
		m_bPlayFlip = true;
	}

	m_AccTime += DT;

	if (1.f < m_AccTime)
	{
		ChangeState(BIKER_STATE::WALK);
	}
}

void CBikerScript::Down()
{
	// 애니메이션 재생
	if (!m_bPlayFlip)
	{
		PlayDownFlip(m_DownCount);
		m_bPlayFlip = true;
	}
	m_AccTime += DT;
	m_bEquipWeapon = true;

	if (m_DownCount == 0)
	{
		if (3.f < m_AccTime)
		{
			ChangeState(BIKER_STATE::GETUP);
			m_DownCount += 1;
		}
	}
	else
	{
		if (2.f < m_AccTime)
		{
			ChangeState(BIKER_STATE::CRAWL);
		}
	}
}

void CBikerScript::Crawl()
{
	// 애니메이션 재생
	if (!m_bPlayFlip)
	{
		PlayCrawlFlip(0);
		m_bPlayFlip = true;
	}
	
	m_AccTime += DT;
	
	if (0.5f < m_AccTime)
	{
		if(!m_CrawlActiveScript)
		{
			m_ActiveScript = true;
			m_CrawlActiveScript = true;
			m_ScriptCount = 0;
		}
		
		// 스크립트 시작
		if (m_ActiveScript)
		{
			CGameObject* pPhoneFace = (CGameObject*)CLevelMgr::GetInst()->FindObjectByName(L"BikerHurtFace");
			pPhoneFace->Transform()->SetRelativeScale(Vec3(250.f, 250.f, 0.f));
			PrintScript(1);

			if (KEY_TAP(KEY::ENTER) || KEY_TAP(KEY::LBTN))
			{
				m_ScriptCount += 1;
			}
			m_AccTime = 0.5f;
		}
		else
		{
			CGameObject* pPhoneFace = (CGameObject*)CLevelMgr::GetInst()->FindObjectByName(L"BikerHurtFace");
			pPhoneFace->Transform()->SetRelativeScale(Vec3(0.f, 0.f, 0.f));
		}
	}

	if (2.f < m_AccTime)
	{
		if(m_AccTime < 2.1f)
			m_bPlayFlip = false;
		m_CanBash = true;
		
		if (!m_bPlayFlip)
		{
			PlayCrawlFlip(1);
			m_bPlayFlip = true;
		}
		
		Vec2 CrawlDir = GetCrawlDir();

		m_BikerPos.x += (m_Speed / 100.f) * CrawlDir.x * DT;
		m_BikerPos.y += (m_Speed / 100.f) * CrawlDir.y * DT;

		m_BikerRot.z = atan2(CrawlDir.y, CrawlDir.x) + (90.f * (180.f / XM_PI));

		Transform()->SetRelativePos(m_BikerPos);
		Transform()->SetRelativeRotation(m_BikerRot);
	}
}

void CBikerScript::Bash()
{
	// 애니메이션 재생
	if (!m_bPlayFlip)
	{
		PlayBashFlip();
		m_bPlayFlip = true;

		m_BikerRot.z -= (90.f * (180.f / XM_PI));
		Transform()->SetRelativeRotation(m_BikerRot);
	}


	m_AccTime += DT;
	// 피 생성
	if (1.f < m_AccTime)
	{
		if (!m_bDead)
		{
			MakeBlood();
			MakePool();
			m_bDead = true;
			GetOwner()->SetName(L"DeadBody");
			m_BossBGM->Stop();
			CLevelScript* pScript = (CLevelScript*)CLevelMgr::GetInst()->FindObjectByName(L"LevelInfo")->GetScripts()[0];
			pScript->CheckFloorClear();
		}		
	}
	
}

void CBikerScript::ChangeState(BIKER_STATE _State)
{
	m_State = _State;
	m_bPlayFlip = false;
	m_AccTime = 0.f;
}

void CBikerScript::MakeMeleeAttack()
{
	Ptr<CPrefab> MeleePrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\MeleeAttack.pref", L"Prefab\\MelleAttack.pref");
	if (!m_bSound)
	{
		Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\boss\\Swing2.wav", L"Sound\\boss\\Swing2.wav");
		pSound->Play(1, 0.5f, true);
		m_bSound = true;
	}


	// 생성될 공격의 회전값을 조절해준다.

	float RadianTheta = m_BikerRot.z;
	
	CGameObject* pMelee = MeleePrefab->GetProtoObject();
	Vec3 vRot = pMelee->Transform()->GetRelativeRotation();

	Vec2 Dir = Vec2(cos(RadianTheta), sin(RadianTheta));
	Dir.Normalize();

	vRot.z = RadianTheta;
	pMelee->Transform()->SetRelativeRotation(vRot);

	Vec3 AttackSpawnPos = Vec3(m_BikerPos.x + Dir.x * 40.f, m_BikerPos.y + Dir.y * 50.f, 0.f);

	vector<CScript*> vecMeleeScript = MeleePrefab->GetProtoObject()->GetScripts();
	for (UINT i = 0; i < vecMeleeScript.size(); ++i)
	{
		if (SCRIPT_TYPE::MELEEATTACKSCRIPT == vecMeleeScript[i]->GetScriptType())
		{
			CMeleeAttackScript* pScript = dynamic_cast<CMeleeAttackScript*>(vecMeleeScript[i]);
			pScript->SetWeapon(WEAPON_TYPE::NONE);
		}
	}

	Instantiate(MeleePrefab, AttackSpawnPos, 6);
}

void CBikerScript::ThrowWeapon()
{
	Ptr<CPrefab> WeaponPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\Clamp.pref", L"Prefab\\Clamp.pref");

	if (!m_bSound)
	{
		Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\attack\\Throw.wav", L"Sound\\attack\\Throw.wav");
		pSound->Play(1, 0.5f, true);
		m_bSound = true;
	}

	Vec2 Dir = Vec2(cos(m_BikerRot.z), sin(m_BikerRot.z));
	Dir.Normalize();

	Vec3 SpawnPos = Vec3(Transform()->GetRelativePos().x + Dir.x * 20.f, Transform()->GetRelativePos().y + Dir.y * 20.f, 0.f);

	if (nullptr != WeaponPrefab)
	{
		vector<CScript*> vecBulletScript = WeaponPrefab->GetProtoObject()->GetScripts();
		for (UINT i = 0; i < vecBulletScript.size(); ++i)
		{
			if (SCRIPT_TYPE::WEAPONSCRIPT == vecBulletScript[i]->GetScriptType())
			{
				CWeaponScript* pScript = dynamic_cast<CWeaponScript*>(vecBulletScript[i]);
				pScript->SetDir(Dir);
				pScript->SetThrow(true);
			}
		}
		Instantiate(WeaponPrefab, SpawnPos, 6);
	}

	m_bEquipWeapon = false;
}

Vec2 CBikerScript::GetCrawlDir()
{
	vector<CGameObject*> vecWalls = FindWallAround(Vec2(m_BikerPos.x, m_BikerPos.y), 200.f);

	CGameObject* nearestWall = vecWalls[0];

	float MinDistance = m_BikerPos.Distance(m_BikerPos, nearestWall->Transform()->GetRelativePos());

	for (UINT i = 0; i < vecWalls.size(); ++i)
	{
		float Distance = m_BikerPos.Distance(m_BikerPos, vecWalls[i]->Transform()->GetRelativePos());

		if (Distance < MinDistance)
		{
			MinDistance = Distance;
			nearestWall = vecWalls[i];
		}
	}

	// 근처에 벽이 없다면 플레이어 반대방향으로
	if (nullptr == nearestWall)
	{
		Vec2 Dir = Vec2(cos(m_PlayerRot.z), sin(m_PlayerRot.z));
		Dir.Normalize();
		return Dir;
	}

	Vec3 WallPos = nearestWall->Transform()->GetRelativePos();
	Vec3 WallRot = nearestWall->Transform()->GetRelativeRotation();

	if (WallRot.z == 0.f)
	{
		if (m_BikerPos.x < WallPos.x)
			return Vec2(-1.f, 0.f);
		else
			return Vec2(1.f, 0.f);
	}
	else
	{
		if (m_BikerPos.y < WallPos.y)
			return Vec2(0.f, -1.f);
		else
			return Vec2(0.f, 1.f);
	}
}


void CBikerScript::PlayComputeFlip()
{
	GetOwner()->FlipbookPlayer()->Play(0, 10, true);
}

void CBikerScript::PlayWalkFlip(bool _Equip)
{
	if (_Equip)
	{
		GetOwner()->FlipbookPlayer()->Play(2, 10, true);
	}
	else
	{
		GetOwner()->FlipbookPlayer()->Play(3, 10, true);
	}
}

void CBikerScript::PlayAttackFlip(int _Idx)
{

	switch (_Idx)
	{
	case 0:
		// Attack
		GetOwner()->FlipbookPlayer()->Play(4, 10, false);
		break;
	case 1:
		// Throw
		GetOwner()->FlipbookPlayer()->Play(5, 7, false);
		break;
	}
}

void CBikerScript::PlayGetUpFlip()
{
	GetOwner()->FlipbookPlayer()->Play(9, 7, false);
}

void CBikerScript::PlayPryFlip()
{
	GetOwner()->FlipbookPlayer()->Play(6, 10, false);
}

void CBikerScript::PlayDownFlip(int _Idx)
{
	switch (_Idx)
	{
	case 0:
		// OneDown
		GetOwner()->FlipbookPlayer()->Play(7, 1, false);
		break;
	case 1:
		// Twodown
		GetOwner()->FlipbookPlayer()->Play(8, 1, false);
		break;
	}
}

void CBikerScript::PlayBashFlip()
{
	GetOwner()->FlipbookPlayer()->Play(12, 10, false);
}

void CBikerScript::PlayIdleFlip()
{
	GetOwner()->FlipbookPlayer()->Play(2, 1, false);
}

void CBikerScript::PlayCrawlFlip(int _Idx)
{
	switch (_Idx)
	{
	case 0:
		// Start
		GetOwner()->FlipbookPlayer()->Play(10, 5, false);
		break;
	case 1:
		// Crawl
		GetOwner()->FlipbookPlayer()->Play(11, 5, true);
		break;
	}
}

void CBikerScript::PrintScript(int _Idx)
{
	wstring script = L"";
	if (!m_ScriptPPActive)
	{
		Ptr<CPrefab> HighLightIngame = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\HighlightIngamePP.pref", L"Prefab\\HighlightIngamePP.pref");
		Instantiate(HighLightIngame, Transform()->GetRelativePos(), 0);
		m_ScriptPPActive = true;
	}

	if (_Idx == 0)
	{
		switch (m_ScriptCount)
		{
		case 0:
			script = L"Get out of here . . .";
			CFontMgr::GetInst()->SetStr(script);
			break;
		case 1:
			script = L"You're Dead Meat . . .";
			CFontMgr::GetInst()->SetStr(script);
			break;
		case 2:
			CFontMgr::GetInst()->SetStr(script);
			DestroyObject(CLevelMgr::GetInst()->FindObjectByName(L"HighLightIngamePP"));
			m_ActiveScript = false;
			m_ScriptPPActive = false;
			break;
		}
	}
	else if (_Idx == 1)
	{
		switch (m_ScriptCount)
		{
		case 0:
			script = L"This . . .\nThis can't be happening . . .";
			CFontMgr::GetInst()->SetStr(script);
			break;
		case 1:
			script = L"It can't end now.\nNot like this . . .";
			CFontMgr::GetInst()->SetStr(script);
			break;
		case 2:
			script = L". . . *COUGH*";
			CFontMgr::GetInst()->SetStr(script);
			break;
		case 3:
			CFontMgr::GetInst()->SetStr(script);
			DestroyObject(CLevelMgr::GetInst()->FindObjectByName(L"HighLightIngamePP"));
			m_ActiveScript = false;
			m_ScriptPPActive = false;
			break;
		}
	}
}



void CBikerScript::MakeBlood()
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
}

void CBikerScript::MakePool()
{
	Vec3 vPos = Transform()->GetRelativePos();

	Ptr<CPrefab> BloodPPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\BloodPool.pref", L"Prefab\\BloodPool.pref");
	BloodPPrefab->GetProtoObject()->FlipbookPlayer()->Play(0, 40, false);

	Instantiate(BloodPPrefab, vPos, 7);
}