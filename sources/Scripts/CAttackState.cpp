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

	// �÷��̾ �ٶ� ����.
	Vec3 vRot = m_Owner->GetOwner()->Transform()->GetRelativeRotation();
	vRot.z = Theta;
	m_Owner->GetOwner()->Transform()->SetRelativeRotation(vRot);

	// ���Ÿ� ������ ���
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
		// ���� ��� ����
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

	// ���� ���ݰ� Fat
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
			// ���� ������ �� ��� ReturnState�� ���ư���.
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

	// �Ѿ��� ������ ���� ������ ���� �ʴ´�.
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
		// ���� ��� ����
		if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
		{
			CEnemyScript* pScript = dynamic_cast<CEnemyScript*>(m_Owner);
			pScript->PlayAttackFlip();
		}
		m_RangeAttCreate = true;
		m_AttStart = true;
	}

	// ���� ���¿� ���� �÷��̾ ������� ������ ������ �Ѵ�.
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

		// ���� �÷��̾��� ��ġ
		Vec3 vPlayerPos = CLevelMgr::GetInst()->FindObjectByName(L"Player")->Transform()->GetRelativePos();
		Vec3 vEnemyPos = GetOwner()->GetOwner()->Transform()->GetRelativePos();

		float Distance = vPlayerPos.Distance(vPlayerPos, vEnemyPos);

		// �÷��̾ Ž�� ���� ���� ���Դ�.
		if (Distance < m_DetectRange)
		{
			// ���� ���� �÷��̾� ���̿� ���� �ִٸ� Ž���� ���Ѵ�.
			if (!FindWall(vPlayerPos, vEnemyPos))
			{
				m_RangeAttCreate = true;
			}
			// �÷��̾ ������ ���� ����
			else
			{
				m_RangeAttCreate = false;
				RockOn = false;
			}
		}

		// �÷��̾ ������� ���� ����
		else
		{
			m_RangeAttCreate = false;
			RockOn = false;
		}
	}
	// ���� �߿� �÷��̾ ������� �÷��̾ ã�Ƴ�����.
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

	// ������ ������ ȸ������ �������ش�.
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
			// FatEnemy�� ���
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

	// �Ѿ˰� ź���� ���� ����
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
					// ź�Ǵ� ���������� 90�� ����
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
					// ź�Ǵ� ���������� 90�� ����
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
					// ź�Ǵ� ���������� 90�� ����
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

