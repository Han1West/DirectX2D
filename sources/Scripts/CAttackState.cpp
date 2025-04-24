#include "pch.h"
#include "CAttackState.h"

#include "CEnemyScript.h"
#include "CFatEnemyScript.h"
#include "CMeleeAttackScript.h"
#include "CBulletScript.h"
#include "CShellScript.h"

#include <Engine/CLevelMgr.h>

CAttackState::CAttackState()
	: CState(STATE_TYPE::ATTACKSTATE)
	, m_AttDelay(0.f)
	, m_AttCreateDelay(0.f)
	, m_AttStartDelay(0.f)
	, m_AttStart(false)
	, m_DetectRange(600.f)
	, m_MeleeAttCreate(false)
	, m_RangeAttCreate(false)
	, m_FirstBullet(true)
{
}

CAttackState::~CAttackState()
{
}

void CAttackState::Enter()
{
	m_AttDelay = 0.f;


}

void CAttackState::FinalTick()
{
	m_AttStartDelay += DT;
	m_AttDelay += DT;

	bool IsRangeAttack = false;
	if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
	{
		CEnemyScript* pScript = dynamic_cast<CEnemyScript*>(m_Owner);
		IsRangeAttack = pScript->IsEquippedGun();
		if (pScript->IsGivenUP())
			return;
	}
	
	float Theta = GetThetaWithPlayer();
	Theta += 180.f * (XM_PI / 180.f);

	// 플레이어를 바라 본다.
	Vec3 vRot = m_Owner->GetOwner()->Transform()->GetRelativeRotation();
	vRot.z = Theta;
	m_Owner->GetOwner()->Transform()->SetRelativeRotation(vRot);

	// 원거리 공격의 경우
	if (IsRangeAttack)
	{
		RangeAttack();
	}
	else
	{
		MeleeAttack();
	}
}

void CAttackState::Exit()
{

}

void CAttackState::MeleeAttack()
{
	if (0.2f < m_AttStartDelay && !m_AttStart)
	{
		// 공격 모션 실행
		if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
		{
			CEnemyScript* pScript = dynamic_cast<CEnemyScript*>(m_Owner);
			pScript->PlayAttackFlip();
		}
		if (SCRIPT_TYPE::FATENEMYSCRIPT == m_Owner->GetScriptType())
		{
			CFatEnemyScript* pScript = dynamic_cast<CFatEnemyScript*>(m_Owner);
			pScript->PlayAttackFlip();
		}
		m_AttStart = true;
	}

	// 근접 공격과 Fat
	if (m_AttStart && !m_MeleeAttCreate)
	{
		if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
		{
			CEnemyScript* pScript = dynamic_cast<CEnemyScript*>(m_Owner);
			MakeMeleeAttack();
			m_MeleeAttCreate = true;

		}
		if (SCRIPT_TYPE::FATENEMYSCRIPT == m_Owner->GetScriptType())
		{
			CFatEnemyScript* pScript = dynamic_cast<CFatEnemyScript*>(m_Owner);
			MakeMeleeAttack();
			m_MeleeAttCreate = true;
		}
	}

	if (m_MeleeAttCreate)
	{
		m_AttCreateDelay += DT;

		if (1.2f < m_AttCreateDelay)
		{
			// 근접 공격을 한 경우 ReturnState로 돌아간다.
			if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
			{
				CEnemyScript* pScript = dynamic_cast<CEnemyScript*>(m_Owner);
				if (!pScript->IsEquippedGun())
					pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CReturnState"));
			}

			else if (SCRIPT_TYPE::FATENEMYSCRIPT == m_Owner->GetScriptType())
			{
				CFatEnemyScript* pScript = dynamic_cast<CFatEnemyScript*>(m_Owner);
				pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CReturnState"));
			}
			m_AttCreateDelay = 0.f;
		}
	}
}

void CAttackState::RangeAttack()
{
	CEnemyScript* pScript = (CEnemyScript*)m_Owner;

	// 총알이 없으면 공격 생성을 하지 않는다.
	if (pScript->GetCurWeaponRounds() <= 0)
	{
		CEnemyScript* pScript = dynamic_cast<CEnemyScript*>(m_Owner);
		if (pScript->IsGivenUP())
			return;
		pScript->DropWeapon();
		pScript->PlaySpecialFlip(4);
		pScript->SetGiveup(true);
		return;
	}

	bool RockOn = true;

	if (0.5f < m_AttStartDelay && !m_AttStart)
	{
		// 공격 모션 실행
		if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
		{
			CEnemyScript* pScript = dynamic_cast<CEnemyScript*>(m_Owner);
			pScript->PlayAttackFlip();
		}
		m_RangeAttCreate = true;
		m_AttStart = true;
	}

	// 공격 상태에 들어가면 플레이어가 사라지기 전까지 공격을 한다.
	if (m_RangeAttCreate)
	{
		if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
		{
			CEnemyScript* pScript = dynamic_cast<CEnemyScript*>(m_Owner);
			bool IsShotgun = true;
			if (pScript->GetWeaponType() == WEAPON_TYPE::M16)
				IsShotgun = false;
			MakeRangerAttack(IsShotgun, pScript->GetCurWeaponRounds());
		}

		// 현재 플레이어의 위치
		Vec3 vPlayerPos = CLevelMgr::GetInst()->FindObjectByName(L"Player")->Transform()->GetRelativePos();
		Vec3 vEnemyPos = GetOwner()->GetOwner()->Transform()->GetRelativePos();

		float Distance = vPlayerPos.Distance(vPlayerPos, vEnemyPos);

		// 플레이어가 탐지 범위 내에 들어왔다.
		if (Distance < m_DetectRange)
		{
			// 만약 적과 플레이어 사이에 벽이 있다면 탐지를 못한다.
			if (!FindWall(vPlayerPos, vEnemyPos))
			{
				m_RangeAttCreate = true;
			}
			// 플레이어가 숨으면 공격 중지
			else
			{
				m_RangeAttCreate = false;
				RockOn = false;
			}
		}

		// 플레이어가 사라지면 공격 중지
		else
		{
			m_RangeAttCreate = false;
			RockOn = false;
		}
	}
	// 공격 중에 플레이어가 사라지면 플레이어를 찾아나선다.
	if(!RockOn)
	{
		Vec3 vPlayerPos = CLevelMgr::GetInst()->FindObjectByName(L"Player")->Transform()->GetRelativePos();

		if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
		{
			CEnemyScript* pScript = (CEnemyScript*)m_Owner;
			pScript->SetDestination(vPlayerPos);
			pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CTraceState"));
		}
	}

}

void CAttackState::MakeMeleeAttack()
{
	Ptr<CPrefab> MeleePrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\MeleeAttack.pref", L"Prefab\\MelleAttack.pref");
	CEnemyScript* OwnerScript = dynamic_cast<CEnemyScript*>(m_Owner);

	Vec3 MonsterPos = m_Owner->GetOwner()->Transform()->GetRelativePos();
	float Theta = m_Owner->GetOwner()->Transform()->GetRelativeRotation().z;

	// 생성될 공격의 회전값을 조절해준다.
	CGameObject* pMelee = MeleePrefab->GetProtoObject();
	Vec3 vRot = pMelee->Transform()->GetRelativeRotation();

	Vec2 Dir = Vec2(cos(Theta), sin(Theta));
	Dir.Normalize();

	vRot.z = Theta;
	pMelee->Transform()->SetRelativeRotation(vRot);

	Vec3 AttackSpawnPos = Vec3(MonsterPos.x + Dir.x * 40.f, MonsterPos.y + Dir.y * 50.f, 0.f);

	vector<CScript*> vecMeleeScript = MeleePrefab->GetProtoObject()->GetScripts();
	for (UINT i = 0; i < vecMeleeScript.size(); ++i)
	{
		if (SCRIPT_TYPE::MELEEATTACKSCRIPT == vecMeleeScript[i]->GetScriptType())
		{
			CMeleeAttackScript* pScript = dynamic_cast<CMeleeAttackScript*>(vecMeleeScript[i]);
			// FatEnemy의 경우
			if (OwnerScript == nullptr)
				pScript->SetWeapon(WEAPON_TYPE::NONE);
			else
				pScript->SetWeapon(OwnerScript->GetWeaponType());
		}
	}


	OwnerScript->Instantiate(MeleePrefab, AttackSpawnPos, 6);	
}

void CAttackState::MakeRangerAttack(bool _IsShotgun, int _WeaponRounds)
{
	Ptr<CPrefab> BulletPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\Bullet.pref", L"Prefab\\Bullet.pref");
	Ptr<CPrefab> ShellPrefab = nullptr;

	// 총알과 탄피의 방향 지정
	Vec3 MonsterPos = m_Owner->GetOwner()->Transform()->GetRelativePos();
	float Theta = m_Owner->GetOwner()->Transform()->GetRelativeRotation().z;

	Vec2 Dir = Vec2(cos(Theta), sin(Theta));
	Dir.Normalize();

	Vec3 BulletSpawnPos = Vec3(MonsterPos.x + Dir.x * 50.f, MonsterPos.y + Dir.y * 50.f, 0.f);
	Vec3 ShellSpawnPos = Vec3(MonsterPos.x + Dir.y * 20.f, MonsterPos.y - Dir.x * 20.f, 0.f);


	if (_IsShotgun)
	{		
		ShellPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\ShotgunShell.pref", L"Prefab\\ShotgunShell.pref");
		
		if (m_FirstBullet)
		{
			vector<CScript*> vecBulletScript = BulletPrefab->GetProtoObject()->GetScripts();
			for (UINT i = 0; i < vecBulletScript.size(); ++i)
			{
				if (SCRIPT_TYPE::BULLETSCRIPT == vecBulletScript[i]->GetScriptType())
				{
					CBulletScript* pScript = dynamic_cast<CBulletScript*>(vecBulletScript[i]);
					pScript->SetDir(Dir);
					pScript->SetEnemy(true);
					pScript->SetShotgun(true);
				}
			}

			vector<CScript*> vecShellScript = ShellPrefab->GetProtoObject()->GetScripts();
			for (UINT i = 0; i < vecShellScript.size(); ++i)
			{
				if (SCRIPT_TYPE::SHELLSCRIPT == vecShellScript[i]->GetScriptType())
				{
					CShellScript* pScript = dynamic_cast<CShellScript*>(vecShellScript[i]);
					// 탄피는 오른쪽으로 90도 꺽임
					pScript->SetDir(Vec2(Dir.y, -Dir.x));
				}
			}
			Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\attack\\Shotgun.wav", L"Sound\\attack\\Shotgun.wav");
			pSound->Play(1, 0.5f, true);
			m_Owner->Instantiate(BulletPrefab, BulletSpawnPos, 6);
			m_Owner->Instantiate(ShellPrefab, ShellSpawnPos, 6);
			_WeaponRounds -= 1;


			CEnemyScript* pOwnerScript = (CEnemyScript*)m_Owner;
			pOwnerScript->SetWeaponRounds(_WeaponRounds);

			m_FirstBullet = false;
		}
		
		m_AttCreateDelay += DT;

		if (0.3f < m_AttCreateDelay)
		{
			vector<CScript*> vecBulletScript = BulletPrefab->GetProtoObject()->GetScripts();
			for (UINT i = 0; i < vecBulletScript.size(); ++i)
			{
				if (SCRIPT_TYPE::BULLETSCRIPT == vecBulletScript[i]->GetScriptType())
				{
					CBulletScript* pScript = dynamic_cast<CBulletScript*>(vecBulletScript[i]);
					pScript->SetDir(Dir);
					pScript->SetEnemy(true);
					pScript->SetShotgun(true);					
				}
			}

			vector<CScript*> vecShellScript = ShellPrefab->GetProtoObject()->GetScripts();
			for (UINT i = 0; i < vecShellScript.size(); ++i)
			{
				if (SCRIPT_TYPE::SHELLSCRIPT == vecShellScript[i]->GetScriptType())
				{
					CShellScript* pScript = dynamic_cast<CShellScript*>(vecShellScript[i]);
					// 탄피는 오른쪽으로 90도 꺽임
					pScript->SetDir(Vec2(Dir.y, -Dir.x));
				}
			}

			if (0 < _WeaponRounds)
			{
				Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\attack\\Shotgun.wav", L"Sound\\attack\\Shotgun.wav");
				pSound->Play(1, 0.5f, true);
				m_Owner->Instantiate(BulletPrefab, BulletSpawnPos, 6);
				m_Owner->Instantiate(ShellPrefab, ShellSpawnPos, 6);
				_WeaponRounds -= 1;

				CEnemyScript* pOwnerScript = (CEnemyScript*)m_Owner;
				pOwnerScript->SetWeaponRounds(_WeaponRounds);

				m_AttCreateDelay = 0.f;
			}
		}	
	}

	else
	{
		ShellPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\M16Shell.pref", L"Prefab\\M16Shell.pref");
		m_AttCreateDelay += DT;

		if (0.1f < m_AttCreateDelay)
		{
			vector<CScript*> vecBulletScript = BulletPrefab->GetProtoObject()->GetScripts();
			for (UINT i = 0; i < vecBulletScript.size(); ++i)
			{
				if (SCRIPT_TYPE::BULLETSCRIPT == vecBulletScript[i]->GetScriptType())
				{
					CBulletScript* pScript = dynamic_cast<CBulletScript*>(vecBulletScript[i]);
					pScript->SetDir(Dir);
				}
			}

			vector<CScript*> vecShellScript = ShellPrefab->GetProtoObject()->GetScripts();
			for (UINT i = 0; i < vecShellScript.size(); ++i)
			{
				if (SCRIPT_TYPE::SHELLSCRIPT == vecShellScript[i]->GetScriptType())
				{
					CShellScript* pScript = dynamic_cast<CShellScript*>(vecShellScript[i]);
					// 탄피는 오른쪽으로 90도 꺽임
					pScript->SetDir(Vec2(Dir.y, -Dir.x));
				}
			}

			if (0 < _WeaponRounds)
			{
				Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\attack\\M16.wav", L"Sound\\attack\\M16.wav");
				pSound->Play(1, 0.5f, true);
				m_Owner->Instantiate(BulletPrefab, BulletSpawnPos, 6);
				m_Owner->Instantiate(ShellPrefab, ShellSpawnPos, 6);
				_WeaponRounds -= 1;

				CEnemyScript* pOwnerScript = (CEnemyScript*)m_Owner;
				pOwnerScript->SetWeaponRounds(_WeaponRounds);

				m_AttCreateDelay = 0.f;
			}
		}
	}
}

