#include "pch.h"
#include "CPlayerScript.h"

#include <Engine/CEngine.h>
#include <Engine/CTaskMgr.h>
#include <Engine/CLevelMgr.h>
#include <Engine/CKeyMgr.h>
#include <Engine/CFontMgr.h>
#include <Engine/CScript.h>
#include <Engine/CLevel.h>
#include <Engine/CConstBuffer.h>
#include <Engine/CDevice.h>

#include <Engine/CCollider2D.h>

#include "CMissileScript.h"
#include "CMeleeAttackScript.h"
#include "CWeaponScript.h"
#include "CBulletScript.h"
#include "CShellScript.h"
#include "CEnemyScript.h"
#include "CBikerScript.h"
#include "CSelectMaskScript.h"
#include "CCarScript.h"
#include "CPhoneScript.h"
#include "CLevelScript.h"

#include "CStateMgr.h"
#include "CKnockDownState.h"
#include "CDyingState.h"



CPlayerScript::CPlayerScript()
	: CScript((UINT)SCRIPT_TYPE::PLAYERSCRIPT)
	, m_PlayerSpeed(400)
	, m_CurWeapon(WEAPON_TYPE::NONE)
	, m_Mask(MASK_TYPE::END)
	, m_State(PLAYER_STATE::IDLE)
	, m_ActionedKey(0)
	, m_AttDelay(0.f)
	, m_BulletDelay(0.f)
	, m_AttCreateDelay(0.f)
	, m_KillAttackDelay(0.f)
	, m_GunSoundDurationTime(0.f)
	, m_FadeTime(0.f)
	, m_KillingCount(3)
	, m_IsMaskOn(false)
	, m_bMakeGunSound(false)
	, m_EquipGun(false)
	, m_FirstBullet(false)
	, m_bFade(false)
	, m_bAttackSound(false)
	, m_bMakeFade(false)
	, m_bPause(false)
	, m_vecKeyCheck()
{
	AddScriptParam(tScriptParam{ SCRIPT_PARAM::FLOAT, "Player Speed", &m_PlayerSpeed });
	AddScriptParam(tScriptParam{ SCRIPT_PARAM::TEXTURE, "Test Texture", &m_TargetTex });
	AddScriptParam(tScriptParam{ SCRIPT_PARAM::PREFAB, "Missile", &m_Prefab });
}

CPlayerScript::~CPlayerScript()
{
}

void CPlayerScript::Begin()
{
	for (int i = 0; i < 4; ++i)
	{
		m_vecKeyCheck.push_back(false);
	}

	m_Cursor = CLevelMgr::GetInst()->FindObjectByName(L"Cursor");

	if (m_Mask == MASK_TYPE::GRAHAM_RABBIT)
	{
		m_PlayerSpeed = 600.f;
	}
	if (m_Mask == MASK_TYPE::DENIS_WOLF)
	{
		m_CurWeapon = WEAPON_TYPE::KNIFE;
	}
	if (m_Mask == MASK_TYPE::RAMI_CAMEL)
	{
		m_AdditionRounds = 2;
	}

	if (m_Mask != MASK_TYPE::END)
	{
		Ptr<CPrefab> MaskPre = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\IngameMask.pref", L"Prefab\\IngameMask.pref");
		CGameObject* MaskObj = MaskPre->GetProtoObject();
		switch (m_Mask)
		{
		case MASK_TYPE::RICHARD_CHICKEN:
			MaskObj->FlipbookPlayer()->Play(0, 1, false);
			break;
		case MASK_TYPE::TONY_TIGER:
			MaskObj->FlipbookPlayer()->Play(1, 1, false);
			break;
		case MASK_TYPE::OBRI_PIG:
			MaskObj->FlipbookPlayer()->Play(2, 1, false);
			break;
		case MASK_TYPE::JUAN_HORSE:
			MaskObj->FlipbookPlayer()->Play(3, 1, false);
			break;
		case MASK_TYPE::GRAHAM_RABBIT:
			MaskObj->FlipbookPlayer()->Play(4, 1, false);
			break;
		case MASK_TYPE::DENIS_WOLF:
			MaskObj->FlipbookPlayer()->Play(5, 1, false);
			break;
		case MASK_TYPE::GEORGE_GIRAFFE:
			MaskObj->FlipbookPlayer()->Play(6, 1, false);
			break;
		case MASK_TYPE::RAMI_CAMEL:
			MaskObj->FlipbookPlayer()->Play(7, 1, false);
			break;
		default:
			break;
		}

		Instantiate(MaskPre, Vec3(0.f, 0.f, 0.f), 0);
	}

	switch (m_CurWeapon)
	{
	case WEAPON_TYPE::NONE:
		m_KillingCount = 3;
		break;
	case WEAPON_TYPE::KNIFE:
		m_KillingCount = 1;
		break;
	case WEAPON_TYPE::LONG_PIPE:
		m_KillingCount = 3;
		break;
	case WEAPON_TYPE::BAT:
		m_KillingCount = 3;
		break;
	case WEAPON_TYPE::KATANA:
		m_KillingCount = 1;
		break;
	case WEAPON_TYPE::AXE:
		m_KillingCount = 1;
		break;
	}

	m_vecTileGrid = MakeTileGrid();
}

void CPlayerScript::Tick()
{
	CGameObject* SelectMaskObj = CLevelMgr::GetInst()->FindObjectByName(L"SelectMask");


	if (m_bFade)
	{
		if (!m_bMakeFade)
		{
			//PostProcess ����
			Ptr<CPrefab> FadePost = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\FadePP.pref", L"Prefab\\FadePP.pref");
			Instantiate(FadePost, Transform()->GetRelativePos(), 0);
			m_bMakeFade = true;
		}
		// ���̵� �ð���	
		TimeData newTime = {};
		newTime.Time = m_FadeTime;
		m_FadeTime += DT;
		CConstBuffer* pCB = CDevice::GetInst()->GetCB(CB_TYPE::TIME);

		pCB->SetData(&newTime);
		pCB->Binding();

	}
	if (1.2f < m_FadeTime)
	{
		m_bFade = false;
		m_FadeTime = 0.f;
	}

	if (KEY_TAP(KEY::ESC))
	{
		m_bPause = !m_bPause;
		if(m_bPause)
			ActivePause();
		if (!m_bPause)
		{
			DestroyObject(CLevelMgr::GetInst()->FindObjectByName(L"HighlightPP"));
			CLevelScript* pLevelScript = (CLevelScript*)CLevelMgr::GetInst()->FindObjectByName(L"LevelInfo")->GetScripts()[0];			
			pLevelScript->GetBGM()->SetVolume(0.4f, pLevelScript->GetBGMIdx());
		}
	}

	if (m_bPause)
	{
		m_State = PLAYER_STATE::PAUSERELEASD;
		KeyCheck();
		return;
	}
		

	// ����ũ�� ���� ���� ���� ����
	if (SelectMaskObj != nullptr)
	{
		
		CSelectMaskScript* pScript = (CSelectMaskScript*)SelectMaskObj->GetScripts()[0];
		KeyCheck();
		if (pScript->IsActive())
		{
			m_State = PLAYER_STATE::PAUSERELEASD;
			return;
		}
			
	}

	if(!m_IsMaskOn)
		RegisterMask();
	

	// ����� ������� ������ ���� �Ұ�
	if (m_bDead)
	{
		AdjustMask();
		if (KEY_TAP(KEY::R))
		{
			RestartCurFloor();
		}
		KeyCheck();
	}
	else
	{
		// ��ȭ UI�� ������ ���� ����
		CGameObject* pPhone = CLevelMgr::GetInst()->FindObjectByName(L"Phone");
		CGameObject* pBiker = CLevelMgr::GetInst()->FindObjectByName(L"Biker");

		CPhoneScript* pPhoneScript = nullptr;
		CBikerScript* pBikerScript = nullptr;

		if(nullptr != pPhone)
			pPhoneScript = (CPhoneScript*)pPhone->GetScripts()[0];
		
		if (nullptr != pBiker)
			pBikerScript = (CBikerScript*)pBiker->GetScripts()[0];

		
		if ((pPhoneScript != nullptr && pPhoneScript->IsActiveScript()) 
			|| (pBikerScript != nullptr && pBikerScript->IsActiveScript()))
		{
			m_State = PLAYER_STATE::PAUSERELEASD;
			PlayIdleFlip();
			KeyCheck();
			return;
		}
			

		// �ڵ����� ź ���¶�� ���� ����
		CGameObject* PlayerCar = CLevelMgr::GetInst()->FindObjectByName(L"Car");
		if (PlayerCar != nullptr)
		{		
			CCarScript* pScript = (CCarScript*)PlayerCar->GetScripts()[0];
			if (pScript->GetRide())
			{
				m_State = PLAYER_STATE::PAUSERELEASD;
				return;
			}				
		}



		Vec3 vPos = Transform()->GetRelativePos();
		Vec3 vRot = Transform()->GetRelativeRotation();

		// ���콺 �ٶ󺸵��� ĳ���� ȸ��
		// ���콺 ��ǥ ���ϱ�
		Vec3 CursorPos = m_Cursor->Transform()->GetRelativePos();

		Vec3 CurMousePos = CKeyMgr::GetInst()->GetRelativeMousePos();

		// ���콺�� ĳ������ ������ ���Ѵ�.
		float Theta = atan2(CursorPos.y - vPos.y, CursorPos.x - vPos.x);

		vRot.z = Theta;



		// �ƹ��� Ű�Է��� ������ Idle ���·� ��ȯ�Ѵ�.
		if (m_ActionedKey == 0
			&& m_State != PLAYER_STATE::MOUNT && m_State != PLAYER_STATE::KILLING && m_State != PLAYER_STATE::AMBUSHING && m_State != PLAYER_STATE::KICKING && m_State != PLAYER_STATE::KILLBIKER)
			m_State = PLAYER_STATE::IDLE;

		// IDLE ���� �ִϸ��̼� ���
		if (m_State == PLAYER_STATE::IDLE)
		{
			PlayIdleFlip();
			m_State = PLAYER_STATE::WAIT;
		}

		if (m_State == PLAYER_STATE::AMBUSHING || m_State == PLAYER_STATE::KICKING || m_State == PLAYER_STATE::KILLBIKER)
		{
			m_AmbushingTime += DT;
			if (0.5f < m_AmbushingTime)
			{
				if (!m_bAttackSound)
				{
					Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\attack\\hit.wav", L"Sound\\attack\\hit.wav");
					pSound->Play(1, 0.5f, true);
					m_bAttackSound = true;
				}

			}
			if (1.f < m_AmbushingTime)
			{
				MakeBodyAfterKill();
				m_AmbushingTime = 0.f;
				m_bAttackSound = false;
				if (m_ActionedKey == 0)
					m_State = PLAYER_STATE::IDLE;
				else
					m_State = PLAYER_STATE::WALK;

				m_bKilling = false;
			}
		}

		// Mount ���¶�� Mount�� ������ �ȵǰ� �Ѵ�.
		if (m_State == PLAYER_STATE::MOUNT || m_State == PLAYER_STATE::KILLING)
		{
			PlayerMount(m_TargetEnemy);
			KeyCheck();
		}
		else if (m_State == PLAYER_STATE::AMBUSHING || m_State == PLAYER_STATE::KICKING || m_State == PLAYER_STATE::KILLBIKER)
		{
			KeyCheck();
		}

		else
		{
			// �÷��̾� ȸ��
			Transform()->SetRelativeRotation(vRot);

			// �÷��̾� �̵�
			PlayerMove(vPos);

			// �÷��̾� ���� ����
			PlayerAttack();
		}


		if (KEY_TAP(KEY::RBTN))
		{
			if (m_CurWeapon != WEAPON_TYPE::NONE)
			{
				PlayerThrow();
				m_CurWeapon = WEAPON_TYPE::NONE;
				PlayWalkFlip();
				m_KillingCount = 3;
				m_EquipGun = false;
			}
		}


		if (m_bMakeGunSound)
		{
			m_GunSoundDurationTime += DT;

			if (0.1f < m_GunSoundDurationTime)
			{
				m_bMakeGunSound = false;
				m_GunSoundDurationTime = 0.f;
			}
		}

		AdjustMask();
	}
}

void CPlayerScript::BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	if (_OtherCollider->GetName() == L"Wall" || _OtherCollider->GetName() == L"StgObj")
	{
		if (m_State == PLAYER_STATE::KICKING)
			return;

		Vec3 PlayerPos = Transform()->GetRelativePos();
		Vec3 WallPos = _OtherObject->Transform()->GetRelativePos();
		Vec3 WallScale = _OtherObject->Transform()->GetRelativeScale();
		Vec3 WallRot = _OtherObject->Transform()->GetRelativeRotation();

		vector<Vec3> vecWallnormalDir = CalcWallVector(PlayerPos, WallPos, WallScale, WallRot);
		for (UINT i = 0; i < vecWallnormalDir.size(); ++i)
		{
			m_CollisionDir.push_back(vecWallnormalDir[i]);
		}
	}


	if (_OtherCollider->GetName() == L"Biker")
	{
		CBikerScript* pBikerScript = (CBikerScript*)_OtherObject->GetScripts()[0];
		Vec3 PlayerPos = Transform()->GetRelativePos();
		Vec3 PlayerRot = Transform()->GetRelativeRotation();
		Vec3 BikerPos = pBikerScript->GetOwner()->Transform()->GetRelativePos();
		Vec3 BikerRot = pBikerScript->GetOwner()->Transform()->GetRelativeRotation();

		// ������ ������ �����Ѵ�
		if (pBikerScript->CanBash())
		{
			PlayerPos = BikerPos;
			PlayerRot.z = BikerRot.z - (90.f * (180.f / XM_PI));

			Transform()->SetRelativePos(PlayerPos);
			Transform()->SetRelativeRotation(PlayerRot);
			
			PlayAttackFlip();
			m_State = PLAYER_STATE::KILLBIKER;
		}
	}

	if (_OtherCollider->GetName() == L"MeleeAttack" || _OtherCollider->GetName() == L"Bullet" || _OtherObject->GetName() == L"Clamp")
	{
		if (!m_bDead)
		{
			Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\attack\\Hit.wav", L"Sound\\attack\\Hit.wav");
			pSound->Play(1, 0.5f, false);
			m_bDead = true;
			PlayDeadFlip();
		}
	}
}


void CPlayerScript::Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{

	// ����� �浹��
	if (_OtherCollider->GetName() == L"Weapon")
	{
		// ���⸦ �����Ѵ�.
		if (KEY_TAP(KEY::RBTN))
		{
			// ��� ����ũ�� ���� ���� �Ұ�
			if (m_Mask == MASK_TYPE::TONY_TIGER)
				return;

			if (m_CurWeapon == WEAPON_TYPE::NONE
				&& m_State != PLAYER_STATE::ATTACK_DELAY)
			{
				EquipWeapon(_OtherObject);
				// �Ҹ� ���
				Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\etc\\PickupWeapon.wav", L"Sound\\etc\\PickupWeapon.wav");
				pSound->Play(1, 0.5f, true);
			}				
		}
	}

	if (_OtherObject->GetName() == L"Enemy")
	{
		if (KEY_TAP(KEY::SPACE))
		{
			vector<CScript*> vecScript = _OtherObject->GetScripts();
			for (UINT i = 0; i < vecScript.size(); ++i)
			{
				if (SCRIPT_TYPE::ENEMYSCRIPT == vecScript[i]->GetScriptType())
				{
					CEnemyScript* pScript = dynamic_cast<CEnemyScript*>(vecScript[i]);
					

					if (!m_bKilling)
					{
						// ���� ��� �� Ȯų
						if (STATE_TYPE::LEANSTATE == pScript->GetStateType() && !m_EquipGun && m_State != PLAYER_STATE::KICKING)
						{
							Vec3 vPlayerPos = Transform()->GetRelativePos();
							Vec3 Pos = _OtherObject->Transform()->GetRelativePos();
							Vec3 Rot = _OtherObject->Transform()->GetRelativeRotation();

							Transform()->SetRelativeRotation(Rot);
							float Theta = Rot.z * (180.f / XM_PI);
							if (Theta == 90.f)
							{
								vPlayerPos.y = Pos.y - 14.f;
								vPlayerPos.x = Pos.x;
							}
							else if (Theta == -90.f)
							{
								vPlayerPos.y = Pos.y + 14.f;
								vPlayerPos.x = Pos.x;
							}
							else if (Theta == 0.f)
							{
								vPlayerPos.x = Pos.x - 14.f;
								vPlayerPos.y = Pos.y;
							}
							else
							{
								vPlayerPos.x = Pos.x + 14.f;
								vPlayerPos.y = Pos.y;
							}
							Transform()->SetRelativePos(vPlayerPos);
							m_State = PLAYER_STATE::KICKING;
							PlayKillFlip(1);
							DestroyObject(_OtherObject);
							m_bKilling = true;
							return;
						}

						// ����Ʈ ���� ����
						else if (STATE_TYPE::KNOCKDOWNSTATE == pScript->GetStateType() && !m_EquipGun && m_State != PLAYER_STATE::MOUNT)
						{
							m_State = PLAYER_STATE::MOUNT;
							m_TargetEnemy = _OtherObject;
							m_bKilling = true;
							return;
						}
						// �ϻ� ��� ����
						else if (STATE_TYPE::KNOCKDOWNSTATE != pScript->GetStateType() && STATE_TYPE::LEANSTATE != pScript->GetStateType())
						{
							if (m_State != PLAYER_STATE::AMBUSHING)
							{
								m_State = PLAYER_STATE::AMBUSHING;
								PlayKillFlip(7);
								DestroyObject(_OtherObject);
								m_bKilling = true;
								return;
							}
						}						
					}			
				}
			}
		}
	}


	// ��ȭ
	if (_OtherCollider->GetName() == L"Phone")
	{
		// ��ȭ�� �޴´�
		if (KEY_TAP(KEY::SPACE))
		{			
			// ���Ҹ� ����
			CPhoneScript* pScript = (CPhoneScript*)_OtherObject->GetScripts()[0];
			if (pScript->IsRing())
			{
				Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\etc\\Phone.wav", L"Sound\\etc\\Phone.wav");
				pSound->Play(1, 0.5f, false);
			}
			pScript->SetRing(false);
			
			// ��ȭ UI ���
			Ptr<CPrefab> HighLightIngame = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\HighlightIngamePP.pref", L"Prefab\\HighlightIngamePP.pref");
			Instantiate(HighLightIngame, Transform()->GetRelativePos(), 0);
			pScript->SetScript(true);
		}
	}
}

void CPlayerScript::EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	if (_OtherCollider->GetName() == L"Wall" || _OtherCollider->GetName() == L"StgObj")
	{
		Vec3 PlayerPos = Transform()->GetRelativePos();
		Vec3 WallPos = _OtherObject->Transform()->GetRelativePos();
		Vec3 WallScale = _OtherObject->Transform()->GetRelativeScale();
		Vec3 WallRot = _OtherObject->Transform()->GetRelativeRotation();
		
		vector<Vec3> vecWallnormalDir = CalcWallVector(PlayerPos, WallPos, WallScale, WallRot);
		for (UINT i = 0; i < vecWallnormalDir.size(); ++i)
		{
			m_CollisionDir.erase(remove(m_CollisionDir.begin(), m_CollisionDir.end(), vecWallnormalDir[i]), m_CollisionDir.end());
		}

	}
}

void CPlayerScript::EquipWeapon(CGameObject* _Weapon)
{
	wstring WeaponType = _Weapon->GetName();
	m_CurWeaponRounds = 0;

	if (WeaponType == L"Axe")
	{
		m_CurWeapon = WEAPON_TYPE::AXE;
		m_EquipGun = false;
		m_KillingCount = 1;
	}
	if (WeaponType == L"Bat")
	{
		m_CurWeapon = WEAPON_TYPE::BAT;
		m_EquipGun = false;
		m_KillingCount = 3;
	}
	if (WeaponType == L"DB")
	{
		m_CurWeapon = WEAPON_TYPE::DOUBLE_BARREL;
		m_CurWeaponRounds = 2 + m_AdditionRounds;
		m_EquipGun = true;
	}
	if (WeaponType == L"Katana")
	{
		m_CurWeapon = WEAPON_TYPE::KATANA;
		m_KillingCount = 1;
		m_EquipGun = false;
	}
	if (WeaponType == L"Knife")
	{
		m_CurWeapon = WEAPON_TYPE::KNIFE;
		m_KillingCount = 1;
		m_EquipGun = false;
	}
	if (WeaponType == L"Pipe")
	{
		m_CurWeapon = WEAPON_TYPE::LONG_PIPE;
		m_KillingCount = 3;
		m_EquipGun = false;
	}
	if (WeaponType == L"Shotgun")
	{
		m_CurWeapon = WEAPON_TYPE::PUMP_SHOTGUN;
		m_CurWeaponRounds = 6 + m_AdditionRounds;
		m_EquipGun = true;
	}
	if (WeaponType == L"SMG")
	{
		m_CurWeapon = WEAPON_TYPE::SMG;
		m_CurWeaponRounds = 30 + m_AdditionRounds;
		m_EquipGun = true;
	}

	if (WeaponType == L"M16")
	{
		m_CurWeapon = WEAPON_TYPE::M16;
		m_CurWeaponRounds = 30 + m_AdditionRounds;
		m_EquipGun = true;
	}

	PlayWalkFlip();

	vector<CScript*> vecScript = _Weapon->GetScripts();
	for (UINT i = 0; i < vecScript.size(); ++i)
	{
		if (SCRIPT_TYPE::WEAPONSCRIPT == vecScript[i]->GetScriptType())
		{
			CWeaponScript* pScript = dynamic_cast<CWeaponScript*>(vecScript[i]);

			if (!pScript->IsFirstUsed())
				m_CurWeaponRounds = pScript->GetRound();
			if (pScript->IsEnemyUsed())
				m_CurWeaponRounds = pScript->GetRound() + m_AdditionRounds;

			pScript->SetEquip(true);
		}
	}
	
}

void CPlayerScript::PlayerMove(Vec3 _Pos)
{
	// �̵� ���·� ����
	if (KEY_TAP(KEY::A))
	{
		if (m_State != PLAYER_STATE::WALK && m_State != PLAYER_STATE::ATTACK_DELAY)
			m_State = PLAYER_STATE::WALK;
		m_ActionedKey += 1;
	}

	if (KEY_TAP(KEY::D))
	{
		if (m_State != PLAYER_STATE::WALK && m_State != PLAYER_STATE::ATTACK_DELAY)
			m_State = PLAYER_STATE::WALK;
		m_ActionedKey += 1;
	}
	if (KEY_TAP(KEY::W))
	{
		if (m_State != PLAYER_STATE::WALK && m_State != PLAYER_STATE::ATTACK_DELAY)
			m_State = PLAYER_STATE::WALK;
		m_ActionedKey += 1;
	}
	if (KEY_TAP(KEY::S))
	{
		if (m_State != PLAYER_STATE::WALK && m_State != PLAYER_STATE::ATTACK_DELAY)
			m_State = PLAYER_STATE::WALK;
		m_ActionedKey += 1;
	}


	// �̵� �ִϸ��̼� ���
	if (m_State == PLAYER_STATE::WALK || m_State == PLAYER_STATE::PAUSERELEASD)
	{
		PlayWalkFlip();
		m_State = PLAYER_STATE::WAIT;
	}

	Vec3 inputVec = Vec3(0.f, 0.f, 0.f);

	// �̵��� ���
	if (KEY_PRESSED(KEY::A))
		inputVec.x -= 1.f;
	if (KEY_PRESSED(KEY::D))
		inputVec.x += 1.f;
	if (KEY_PRESSED(KEY::W))
		inputVec.y += 1.f;
	if (KEY_PRESSED(KEY::S))
		inputVec.y -= 1.f;

	if (inputVec.Length() > 0)
		inputVec.Normalize();

	for (const auto& normal : m_CollisionDir)
	{
		float dotRes = inputVec.Dot(normal);
		if (dotRes > 0)
		{
			inputVec -= normal * dotRes;
		}
	}

	// ���� �̵��� ����
	inputVec.Normalize();
	_Pos += inputVec * m_PlayerSpeed * DT;

	// �̵� Ű�Է��� ����ȴ�.
	if (KEY_RELEASED(KEY::A))
		m_ActionedKey -= 1;
	if (KEY_RELEASED(KEY::D))
		m_ActionedKey -= 1;
	if (KEY_RELEASED(KEY::W))
		m_ActionedKey -= 1;
	if (KEY_RELEASED(KEY::S))
		m_ActionedKey -= 1;


	Transform()->SetRelativePos(_Pos);
}

void CPlayerScript::PlayerAttack()
{
	// ���� ��� 
	if (KEY_TAP(KEY::LBTN))
	{
		if (m_State != PLAYER_STATE::ATTACK_DELAY)
		{
			if (m_EquipGun && m_CurWeaponRounds == 0)
				return;

			m_State = PLAYER_STATE::ATTACK;
			m_ActionedKey += 1;
			m_FirstBullet = true;
		}
	}

	// ���� �ִϸ��̼� ����
	if (m_State == PLAYER_STATE::ATTACK)
	{
		PlayAttackFlip();

		// ���� ���� ����
		//if(!m_EquipGun)
			

		m_State = PLAYER_STATE::ATTACK_DELAY;
	}

	// ���� Delay �ð� ���
	if (m_State == PLAYER_STATE::ATTACK_DELAY)
	{
		m_AttDelay += DT;

		// ���Ÿ� ���Ⱑ �ƴ϶��
		if (!m_EquipGun)
		{
			MakeMeleeAttack();
			if (m_CurWeapon == WEAPON_TYPE::KNIFE || m_CurWeapon == WEAPON_TYPE::NONE)
			{
				if (0.3f < m_AttDelay)
				{
					m_ActionedKey -= 1;
					m_AttDelay = 0.f;
					m_FirstBullet = false;
					m_AttCreateDelay = 0.f;
					m_bAttackSound = false;

					// ������ ������ ������ �������� ���ư���
					if (0 < m_ActionedKey)
						m_State = PLAYER_STATE::WALK;
					else
						m_State = PLAYER_STATE::IDLE;
				}
			}
			else
			{
				if (0.5f < m_AttDelay)
				{
					m_ActionedKey -= 1;
					m_AttDelay = 0.f;				
					m_FirstBullet = false;
					m_AttCreateDelay = 0.f;
					m_bAttackSound = false;

					// ������ ������ ������ �������� ���ư���
					if (0 < m_ActionedKey)
						m_State = PLAYER_STATE::WALK;
					else
						m_State = PLAYER_STATE::IDLE;
				}
			}

		}

		// ���Ÿ� ����
		else
		{
			// ����
			m_bMakeGunSound = true;
			if (m_CurWeapon == WEAPON_TYPE::PUMP_SHOTGUN)
			{
				MakeRangeAttack(true);

				if (0.4 < m_AttDelay)
				{
					m_ActionedKey -= 1;
					m_AttDelay = 0.f;
					if (0 < m_ActionedKey)
						m_State = PLAYER_STATE::WALK;
					else
						m_State = PLAYER_STATE::IDLE;
				}
			}
			// ���� �跲
			else if (m_CurWeapon == WEAPON_TYPE::DOUBLE_BARREL)
			{
				MakeRangeAttack(true);

				if (0.2 < m_AttDelay)
				{
					m_ActionedKey -= 1;
					m_AttDelay = 0.f;
					if (0 < m_ActionedKey)
						m_State = PLAYER_STATE::WALK;
					else
						m_State = PLAYER_STATE::IDLE;
				}
			}
			// ���� ��
			else
			{
				MakeRangeAttack(false);

				// ��ư�� ���� ������ ������.
				if (KEY_RELEASED(KEY::LBTN))
				{
					m_ActionedKey -= 1;
					if (0 < m_ActionedKey)
						m_State = PLAYER_STATE::WALK;
					else
						m_State = PLAYER_STATE::IDLE;

					m_FirstBullet = true;
				}
			}
		}
			
	}
		

}

void CPlayerScript::PlayerThrow()
{
	Ptr<CPrefab> WeaponPrefab = nullptr;

	switch (m_CurWeapon)
	{
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
		break;
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

	Vec3 PlayerPos = GetOwner()->Transform()->GetRelativePos();
	Vec3 CurosrPos = m_Cursor->Transform()->GetRelativePos();
	Vec3 MousePos = CKeyMgr::GetInst()->GetRelativeMousePos();

	Vec2 Dir = Vec2(CurosrPos.x - PlayerPos.x, CurosrPos.y - PlayerPos.y);
	Dir.Normalize();

	Vec3 SpawnPos = Vec3(Transform()->GetRelativePos().x + Dir.x * 50.f, Transform()->GetRelativePos().y + Dir.y * 50.f, 0.f);

	if (nullptr != WeaponPrefab)
	{
		// �Ҹ� ���
		Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\attack\\Throw.wav", L"Sound\\attack\\Throw.wav");
		pSound->Play(1, 0.5f, true);

		vector<CScript*> vecBulletScript = WeaponPrefab->GetProtoObject()->GetScripts();
		for (UINT i = 0; i < vecBulletScript.size(); ++i)
		{
			if (SCRIPT_TYPE::WEAPONSCRIPT == vecBulletScript[i]->GetScriptType())
			{
				CWeaponScript* pScript = dynamic_cast<CWeaponScript*>(vecBulletScript[i]);
				pScript->SetDir(Dir);
				pScript->SetThrow(true);
				pScript->SetRound(m_CurWeaponRounds);
				pScript->SetFirstUse(false);
			}
		}
		Instantiate(WeaponPrefab, SpawnPos, 4);
	}
}

void CPlayerScript::PlayerMount(CGameObject* _Enemy)
{
	// �Ѿ��� ���� ��ġ�� ��ġ ����
	if (m_TargetEnemy->IsDead() == false)
	{
		Vec3 EnemyPos = m_TargetEnemy->Transform()->GetRelativePos();
		Vec3 EnemyRot = m_TargetEnemy->Transform()->GetRelativeRotation();
		Transform()->SetRelativePos(EnemyPos);
		Transform()->SetRelativeRotation(EnemyRot);

		// ������ ���� ��ٿ� ���¿��� ȸ������ ���ϰ� �Ѵ�.
		CEnemyScript* pEnemyScript = (CEnemyScript*)(_Enemy->GetScripts()[0]);
		CKnockDownState* pState = (CKnockDownState*)(pEnemyScript->GetState());
		pState->SetMount(true);
		pEnemyScript->SetMount(true);
	}

	// ���� ������ ���� �ִϸ��̼ǰ� �Լ� ����
	switch (m_CurWeapon)
	{
	case WEAPON_TYPE::NONE:
		PlayerKilling(m_CurWeapon);
		if(m_State == PLAYER_STATE::MOUNT)
			PlayMountFlip(m_CurWeapon);
		DestroyObject(_Enemy);
		break;
	case WEAPON_TYPE::KNIFE:
		PlayerKilling(m_CurWeapon);
		if (m_State == PLAYER_STATE::MOUNT)
			PlayMountFlip(m_CurWeapon);
		DestroyObject(_Enemy);
		break;
	case WEAPON_TYPE::LONG_PIPE:
		PlayerKilling(m_CurWeapon);
		if (m_State == PLAYER_STATE::MOUNT)
			PlayMountFlip(m_CurWeapon);
		break;
	case WEAPON_TYPE::BAT:
		PlayerKilling(m_CurWeapon);
		if (m_State == PLAYER_STATE::MOUNT)
			PlayMountFlip(m_CurWeapon);
		break;
	case WEAPON_TYPE::KATANA:
		PlayerKilling(m_CurWeapon);
		if (m_State == PLAYER_STATE::MOUNT)
			PlayMountFlip(m_CurWeapon);
		break;
	case WEAPON_TYPE::AXE:
		PlayerKilling(m_CurWeapon);
		if (m_State == PLAYER_STATE::MOUNT)
			PlayMountFlip(m_CurWeapon);
		break;
	default:
		break;
	}
}

void CPlayerScript::PlayerKilling(WEAPON_TYPE _WeaponType)
{
	int FlipIdx = 0;
	float MaxTime = 0.f;

	// ���� ������ ���� �ٸ� Ȯų Ƚ���� Ȯų ��� �����̸� ������.
	switch (_WeaponType)
	{
	case WEAPON_TYPE::NONE:
		FlipIdx = 0;
		MaxTime = 0.3f;
		break;
	case WEAPON_TYPE::KNIFE:
		FlipIdx = 4;
		MaxTime = 0.5f;
		break;
	case WEAPON_TYPE::LONG_PIPE:
		FlipIdx = 5;
		MaxTime = 0.4f;
		break;
	case WEAPON_TYPE::BAT:
		FlipIdx = 3;
		MaxTime = 0.4f;
		break;
	case WEAPON_TYPE::KATANA:
		FlipIdx = 6;
		MaxTime = 0.8f;
		break;
	case WEAPON_TYPE::AXE:
		FlipIdx = 2;
		MaxTime = 0.8f;
		break;

	default:
		break;
	}

	// ��Ŭ���� �ϸ� �ѹ��� Ȯų ����� �����Ѵ�
	if (KEY_TAP(KEY::LBTN))
	{
		if (m_State == PLAYER_STATE::MOUNT)
		{
			m_State = PLAYER_STATE::KILLING;
			m_KillingCount -= 1;
			PlayKillFlip(FlipIdx);
			
			// ������ ���� ���¸� �׾�� ���·� �����Ѵ�.
			if (m_CurWeapon != WEAPON_TYPE::NONE && m_CurWeapon != WEAPON_TYPE::KNIFE)
			{
				CEnemyScript* pEnemyScript = dynamic_cast<CEnemyScript*>(m_TargetEnemy->GetScripts()[0]);
				CState* pNextState = CStateMgr::ChangeState(pEnemyScript->GetState(), L"CDyingState");
				pEnemyScript->SetState(pNextState);
				Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\attack\\hit.wav", L"Sound\\attack\\hit.wav");
				pSound->Play(1, 0.5f, true);
			}
			else if (m_CurWeapon == WEAPON_TYPE::NONE)
			{
				Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\attack\\Punch.wav", L"Sound\\attack\\Punch.wav");
				pSound->Play(1, 0.5f, true);
			}
			else if (m_CurWeapon == WEAPON_TYPE::KNIFE)
			{
				Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\attack\\Cut1.wav", L"Sound\\attack\\Cut1.wav");
				pSound->Play(1, 0.5f, true);
				if (m_KillingCount == 0)
				{
					Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\attack\\hit.wav", L"Sound\\attack\\hit.wav");
					pSound->Play(1, 0.5f, true);
				}
			}
		}
	}

	if (m_State == PLAYER_STATE::KILLING)
	{
		m_KillAttackDelay += DT;
	}		

	if (MaxTime < m_KillAttackDelay)
	{
		m_State = PLAYER_STATE::MOUNT;		
		m_KillAttackDelay = 0.f;

		if (m_KillingCount == 0)
		{
			if (m_CurWeapon == WEAPON_TYPE::NONE || m_CurWeapon == WEAPON_TYPE::KNIFE)
				MakeBodyAfterKill();

			if (m_CurWeapon != WEAPON_TYPE::NONE && m_CurWeapon != WEAPON_TYPE::KNIFE)
			{
				CEnemyScript* pEnemyScript = dynamic_cast<CEnemyScript*>(m_TargetEnemy->GetScripts()[0]);
				pEnemyScript->SetMount(false);
			}

			if (m_ActionedKey == 0)
				m_State = PLAYER_STATE::IDLE;
			else
				m_State = PLAYER_STATE::WALK;

			m_bKilling = false;

			switch (m_CurWeapon)
			{
			case WEAPON_TYPE::NONE:
				m_KillingCount = 3;
				break;
			case WEAPON_TYPE::KNIFE:
				m_KillingCount = 1;
				break;
			case WEAPON_TYPE::LONG_PIPE:
				m_KillingCount = 3;
				break;
			case WEAPON_TYPE::BAT:
				m_KillingCount = 3;
				break;
			case WEAPON_TYPE::KATANA:
				m_KillingCount = 1;
				break;
			case WEAPON_TYPE::AXE:
				m_KillingCount = 1;
				break;
			}
		}
	}
}

void CPlayerScript::MakeMeleeAttack()
{
	// �Ҹ� ���
	if (!m_bAttackSound)
	{
		Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\attack\\Swing1.wav", L"Sound\\attack\\Swing1.wav");
		pSound->Play(1, 0.5f, true);
		m_bAttackSound = true;
	}


	Ptr<CPrefab> MeleePrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\MeleeAttack.pref", L"Prefab\\MelleAttack.pref");

	Vec3 PlayerPos = GetOwner()->Transform()->GetRelativePos();
	Vec3 CurosrPos = m_Cursor->Transform()->GetRelativePos();
	Vec3 MousePos = CKeyMgr::GetInst()->GetRelativeMousePos();
	
	// ������ ������ ȸ������ �������ش�.
	CGameObject* pMelee = MeleePrefab->GetProtoObject();
	Vec3 vRot = pMelee->Transform()->GetRelativeRotation();

	Vec2 Dir = Vec2(CurosrPos.x - PlayerPos.x, CurosrPos.y - PlayerPos.y);
	Dir.Normalize();

	float Theta = atan2(Dir.y, Dir.x);
	vRot.z = Theta;
	pMelee->Transform()->SetRelativeRotation(vRot);

	Vec3 AttackSpawnPos = Vec3(Transform()->GetRelativePos().x + Dir.x * 40.f, Transform()->GetRelativePos().y + Dir.y * 50.f, 0.f);

	vector<CScript*> vecMeleeScript = MeleePrefab->GetProtoObject()->GetScripts();
	for (UINT i = 0; i < vecMeleeScript.size(); ++i)
	{
		if (SCRIPT_TYPE::MELEEATTACKSCRIPT == vecMeleeScript[i]->GetScriptType())
		{
			CMeleeAttackScript* pScript = dynamic_cast<CMeleeAttackScript*>(vecMeleeScript[i]);
			pScript->SetWeapon(m_CurWeapon);
		}
	}
	m_AttCreateDelay += DT;
	
	if (0.2f < m_AttCreateDelay)
	{
		Instantiate(MeleePrefab, AttackSpawnPos, 5);
		m_AttCreateDelay = 0.f;
	}
}

void CPlayerScript::MakeRangeAttack(bool _IsShotgun)
{
	Ptr<CPrefab> BulletPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\Bullet.pref", L"Prefab\\Bullet.pref");
	Ptr<CPrefab> ShellPrefab = nullptr;

	// �Ѿ˰� ź���� ���� ����
	Vec3 PlayerPos = GetOwner()->Transform()->GetRelativePos();
	Vec3 CurosrPos = m_Cursor->Transform()->GetRelativePos();
	Vec3 MousePos = CKeyMgr::GetInst()->GetRelativeMousePos();

	Vec2 Dir = Vec2(CurosrPos.x - PlayerPos.x, CurosrPos.y - PlayerPos.y);
	Dir.Normalize();

	Vec3 BulletSpawnPos = Vec3(Transform()->GetRelativePos().x + Dir.x * 50.f, Transform()->GetRelativePos().y + Dir.y * 50.f, 0.f);
	Vec3 ShellSpawnPos = Vec3(Transform()->GetRelativePos().x + Dir.y * 20.f, Transform()->GetRelativePos().y - Dir.x * 20.f, 0.f);

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
					pScript->SetEnemy(false);
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

			if (0 < m_CurWeaponRounds)
			{
				// �Ҹ� ���
				Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\attack\\Shotgun.wav", L"Sound\\attack\\Shotgun.wav");
				pSound->Play(1, 0.5f, true);
				Instantiate(BulletPrefab, BulletSpawnPos, 5);
				Instantiate(ShellPrefab, ShellSpawnPos, 5);
				m_CurWeaponRounds -= 1;
				m_FirstBullet = false;
			}
		}
	}

	else
	{
		ShellPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\M16Shell.pref", L"Prefab\\M16Shell.pref");
		if (m_FirstBullet)
		{
			vector<CScript*> vecBulletScript = BulletPrefab->GetProtoObject()->GetScripts();
			for (UINT i = 0; i < vecBulletScript.size(); ++i)
			{
				if (SCRIPT_TYPE::BULLETSCRIPT == vecBulletScript[i]->GetScriptType())
				{
					CBulletScript* pScript = dynamic_cast<CBulletScript*>(vecBulletScript[i]);
					pScript->SetDir(Dir);
					pScript->SetEnemy(false);
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

			if (0 < m_CurWeaponRounds)
			{
				// �Ҹ� ���
				if (m_CurWeapon == WEAPON_TYPE::M16)
				{
					Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\attack\\M16.wav", L"Sound\\attack\\M16.wav");
					pSound->Play(1, 0.5f, true);
				}
				if (m_CurWeapon == WEAPON_TYPE::SMG)
				{
					Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\attack\\M16.wav", L"Sound\\attack\\M16.wav");
					pSound->Play(1, 0.5f, true);
				}
				
				Instantiate(BulletPrefab, BulletSpawnPos, 5);
				Instantiate(ShellPrefab, ShellSpawnPos, 5);
				m_CurWeaponRounds -= 1;
				m_FirstBullet = false;
			}
		}

		// �Ѿ�, ź�� ����
		if (KEY_PRESSED(KEY::LBTN))
		{
			m_BulletDelay += DT;
			if (0.1 < m_BulletDelay)
			{
				m_bMakeGunSound = true;
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

				if (0 < m_CurWeaponRounds)
				{
					// �Ҹ� ���
					if (m_CurWeapon == WEAPON_TYPE::M16)
					{
						Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\attack\\M16.wav", L"Sound\\attack\\M16.wav");
						pSound->Play(1, 0.5f, true);
					}
					if (m_CurWeapon == WEAPON_TYPE::SMG)
					{
						Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\attack\\M16.wav", L"Sound\\attack\\M16.wav");
						pSound->Play(1, 0.5f, true);
					}

					Instantiate(BulletPrefab, BulletSpawnPos, 5);
					Instantiate(ShellPrefab, ShellSpawnPos, 5);
					m_CurWeaponRounds -= 1;
					m_FirstBullet = false;
					m_BulletDelay = 0.f;
				}
			}
		}
	}

}

void CPlayerScript::PlayIdleFlip()
{
	switch (m_CurWeapon)
	{
	case WEAPON_TYPE::NONE:
		GetOwner()->FlipbookPlayer()->Play(10, 1, false);
		break;
	case WEAPON_TYPE::KNIFE:
		GetOwner()->FlipbookPlayer()->Play(15, 1, false);
		break;
	case WEAPON_TYPE::LONG_PIPE:
		GetOwner()->FlipbookPlayer()->Play(17, 1, false);
		break;
	case WEAPON_TYPE::BAT:
		GetOwner()->FlipbookPlayer()->Play(12, 1, false);
		break;
	case WEAPON_TYPE::KATANA:
		GetOwner()->FlipbookPlayer()->Play(14, 1, false);
		break;
	case WEAPON_TYPE::AXE:
		GetOwner()->FlipbookPlayer()->Play(11, 1, false);
		break;
	case WEAPON_TYPE::M16:
		GetOwner()->FlipbookPlayer()->Play(16, 1, false);
		break;
	case WEAPON_TYPE::PISTOL:
		break;
	case WEAPON_TYPE::SMG:
		GetOwner()->FlipbookPlayer()->Play(19, 1, false);
		break;
	case WEAPON_TYPE::DOUBLE_BARREL:
		GetOwner()->FlipbookPlayer()->Play(13, 1, false);
		break;
	case WEAPON_TYPE::PUMP_SHOTGUN:
		GetOwner()->FlipbookPlayer()->Play(18, 1, false);
		break;
	}
}

void CPlayerScript::PlayWalkFlip()
{
	switch (m_CurWeapon)
	{
	case WEAPON_TYPE::NONE:
		GetOwner()->FlipbookPlayer()->Play(20, 10, true);
		break;
	case WEAPON_TYPE::KNIFE:
		GetOwner()->FlipbookPlayer()->Play(25, 10, true);
		break;
	case WEAPON_TYPE::LONG_PIPE:
		GetOwner()->FlipbookPlayer()->Play(27, 10, true);
		break;
	case WEAPON_TYPE::BAT:
		GetOwner()->FlipbookPlayer()->Play(22, 10, true);
		break;
	case WEAPON_TYPE::KATANA:
		GetOwner()->FlipbookPlayer()->Play(24, 10, true);
		break;
	case WEAPON_TYPE::AXE:
		GetOwner()->FlipbookPlayer()->Play(21, 10, true);
		break;
	case WEAPON_TYPE::M16:
		GetOwner()->FlipbookPlayer()->Play(26, 10, true);
		break;
	case WEAPON_TYPE::PISTOL:
		break;
	case WEAPON_TYPE::SMG:
		GetOwner()->FlipbookPlayer()->Play(28, 10, true);
		break;
	case WEAPON_TYPE::DOUBLE_BARREL:
		GetOwner()->FlipbookPlayer()->Play(23, 10, true);
		break;
	case WEAPON_TYPE::PUMP_SHOTGUN:
		GetOwner()->FlipbookPlayer()->Play(29, 10, true);
		break;
	}
}

void CPlayerScript::PlayAttackFlip()
{
	switch (m_CurWeapon)
	{
	case WEAPON_TYPE::NONE:
		GetOwner()->FlipbookPlayer()->Play(9, 14, false);
		break;
	case WEAPON_TYPE::KNIFE:
		GetOwner()->FlipbookPlayer()->Play(4, 18, false);
		break;
	case WEAPON_TYPE::LONG_PIPE:
		GetOwner()->FlipbookPlayer()->Play(6, 15, false);
		break;
	case WEAPON_TYPE::BAT:
		GetOwner()->FlipbookPlayer()->Play(1, 15, false);
		break;
	case WEAPON_TYPE::KATANA:
		GetOwner()->FlipbookPlayer()->Play(3, 14, false);
		break;
	case WEAPON_TYPE::AXE:
		GetOwner()->FlipbookPlayer()->Play(0, 14, false);
		break;
	case WEAPON_TYPE::M16:
		GetOwner()->FlipbookPlayer()->Play(5, 10, false);
		break;
	case WEAPON_TYPE::PISTOL:
		break;
	case WEAPON_TYPE::SMG:
		GetOwner()->FlipbookPlayer()->Play(7, 10, false);
		break;
	case WEAPON_TYPE::DOUBLE_BARREL:
		GetOwner()->FlipbookPlayer()->Play(2, 10, false);
		break;
	case WEAPON_TYPE::PUMP_SHOTGUN:
		GetOwner()->FlipbookPlayer()->Play(8, 10, false);
		break;
	}
}

void CPlayerScript::PlayMountFlip(WEAPON_TYPE _Weapon)
{
	switch (_Weapon)
	{
	case WEAPON_TYPE::NONE:
		GetOwner()->FlipbookPlayer()->Play(38, 1, false);
		break;
	case WEAPON_TYPE::KNIFE:
		GetOwner()->FlipbookPlayer()->Play(39, 1, false);
		break;
	case WEAPON_TYPE::LONG_PIPE:
		GetOwner()->FlipbookPlayer()->Play(40, 1, false);
		break;
	case WEAPON_TYPE::BAT:
		GetOwner()->FlipbookPlayer()->Play(41, 1, false);
		break;
	case WEAPON_TYPE::KATANA:
		GetOwner()->FlipbookPlayer()->Play(42, 1, false);
		break;
	case WEAPON_TYPE::AXE:
		GetOwner()->FlipbookPlayer()->Play(43, 1, false);
		break;

	default:
		break;
	}
}

void CPlayerScript::PlayKillFlip(int _Idx)
{

	switch (_Idx)
	{
	case 0:
		// Bash
		GetOwner()->FlipbookPlayer()->Play(30, 10, false);
		break;
	case 1:
		// KickFace
		GetOwner()->FlipbookPlayer()->Play(31, 10, false);
		break;
	case 2:
		// AxeKill
		GetOwner()->FlipbookPlayer()->Play(32, 10, false);
		break;
	case 3:
		// BatKill
		GetOwner()->FlipbookPlayer()->Play(33, 15, false);
		break;
	case 4:
		// KnifeKill
		GetOwner()->FlipbookPlayer()->Play(34, 7, false);
		break;
	case 5:
		// PipeKill
		GetOwner()->FlipbookPlayer()->Play(35, 15, false);
		break;
	case 6:
		// SwordKill
		GetOwner()->FlipbookPlayer()->Play(36, 10, false);
		break;
	case 7:
		// ThroatKill
		GetOwner()->FlipbookPlayer()->Play(37, 4, false);
		break;
	default:
		break;
	}
}

void CPlayerScript::PlayDeadFlip()
{
	GetOwner()->FlipbookPlayer()->Play(44, 1, false);
}

void CPlayerScript::MakeBodyAfterKill()
{
	Vec3 vPlayerPos = Transform()->GetRelativePos();
	Vec3 vPlayerRot = Transform()->GetRelativeRotation();
	Vec3 vSpawnPos = vPlayerPos;

	Ptr<CPrefab> EnemyPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\Enemy.pref", L"Prefab\\Enemy.pref");
	EnemyPrefab->GetProtoObject()->Transform()->SetRelativeRotation(vPlayerRot);
	CEnemyScript* pScript = (CEnemyScript*)EnemyPrefab->GetProtoObject()->GetScripts()[0];


	if (m_State == PLAYER_STATE::KILLBIKER)
		return;
	// �� ��� �� ����
	else if (m_State == PLAYER_STATE::KICKING)
	{
		float Theta = vPlayerRot.z * (180.f / XM_PI);
		if (Theta == 90.f)
		{
			vSpawnPos.y = vPlayerPos.y + 14.f;
		}
		else if (Theta == -90.f)
		{
			vSpawnPos.y = vPlayerPos.y - 14.f;
		}
		else if (Theta == 0.f)
		{
			vSpawnPos.x = vPlayerPos.x + 14.f;
		}
		else
		{
			vSpawnPos.x = vPlayerPos.x - 14.f;
		}
		pScript->SetPlayerMake(true);
		pScript->PlayDeadFlip(8);
	}
		
	// ��� ��� �� ����
	else if(m_State == PLAYER_STATE::AMBUSHING)
	{
		pScript->SetPlayerMake(true);
		pScript->PlayDyingFlip(5);
	}
	else
	{
		// �ָ� Ȯų �� ����
		if (m_CurWeapon == WEAPON_TYPE::NONE)
		{
			pScript->SetPlayerMake(true);
			pScript->PlayDeadFlip(1);
		}
			
		// Į Ȯų �� ����
		else if (m_CurWeapon == WEAPON_TYPE::KNIFE)
		{
			pScript->SetPlayerMake(true);
			pScript->PlayDeadFlip(4);
		}
	}
	
	Instantiate(EnemyPrefab, vSpawnPos, 3);
}

void CPlayerScript::KeyCheck()
{
	//if (KEY_TAP(KEY::A))
	//	m_ActionedKey += 1;
	//if (KEY_TAP(KEY::S))
	//	m_ActionedKey += 1;
	//if (KEY_TAP(KEY::D))
	//	m_ActionedKey += 1;
	//if (KEY_TAP(KEY::W))
	//	m_ActionedKey += 1;

	if (KEY_PRESSED(KEY::A))
		m_vecKeyCheck[0] = true;
	else
		m_vecKeyCheck[0] = false;
	if (KEY_PRESSED(KEY::D))
		m_vecKeyCheck[1] = true;
	else
		m_vecKeyCheck[1] = false;

	if (KEY_PRESSED(KEY::W))
		m_vecKeyCheck[2] = true;
	else
		m_vecKeyCheck[2] = false;

	if (KEY_PRESSED(KEY::S))
		m_vecKeyCheck[3] = true;
	else
		m_vecKeyCheck[3] = false;


	for (int i = 0; i < m_vecKeyCheck.size(); ++i)
	{
		m_ActionedKey = count(m_vecKeyCheck.begin(), m_vecKeyCheck.end(), true);
	}

	//if (KEY_RELEASED(KEY::A))
	//	m_ActionedKey -= 1;
	//if (KEY_RELEASED(KEY::S))
	//	m_ActionedKey -= 1;
	//if (KEY_RELEASED(KEY::D))
	//	m_ActionedKey -= 1;
	//if (KEY_RELEASED(KEY::W))
	//	m_ActionedKey -= 1;
}

void CPlayerScript::AdjustMask()
{
	CGameObject* pMask = CLevelMgr::GetInst()->FindObjectByName(L"IngameMask");
	if (nullptr == pMask)
		return;

	if (m_EquipGun)
	{
		if (m_Mask != MASK_TYPE::END)
		{
			Vec3 MaskPos = pMask->Transform()->GetRelativePos();
			MaskPos.x = -0.001f;
			pMask->Transform()->SetRelativePos(MaskPos);
		}
	}
	else
	{
		if (m_Mask != MASK_TYPE::END)
		{
			Vec3 MaskPos = pMask->Transform()->GetRelativePos();
			MaskPos.x = 0.f;
			pMask->Transform()->SetRelativePos(MaskPos);
		}
	}

	if (m_bDead)
	{
		if (m_Mask != MASK_TYPE::END)
		{
			Vec3 MaskScale = pMask->Transform()->GetRelativeScale();
			MaskScale.x = 0.f;
			MaskScale.y = 0.f;
			pMask->Transform()->SetRelativeScale(MaskScale);
		}
	}
}

void CPlayerScript::ActivePause()
{
	Vec3 vPlayerPos = Transform()->GetRelativePos();
	CLevelScript* pLevelScript = (CLevelScript*)CLevelMgr::GetInst()->FindObjectByName(L"LevelInfo")->GetScripts()[0];
	pLevelScript->GetBGM()->SetVolume(0.2f, pLevelScript->GetBGMIdx());

	Ptr<CPrefab> Highlight = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\HighlightPP.pref", L"Prefab\\HighlightPP.pref");


	Instantiate(Highlight, vPlayerPos, 0);

}

void CPlayerScript::RestartCurFloor()
{
	CGameObject* pLevelInfo = CLevelMgr::GetInst()->FindObjectByName(L"LevelInfo");

	if (!pLevelInfo) 
	{
		return;
	}
	CLevelScript* pLevelScript = dynamic_cast<CLevelScript*>(pLevelInfo->GetScripts()[0]);
	if (!pLevelScript)
	{
		return;
	}


	// ���� ������ �����´�.
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	CLevel* pLoadedLevel = nullptr;

	// ���� ���� ����
	ChangeLevelState(LEVEL_STATE::STOP);
	wstring ContentPath = CPathMgr::GetInst()->GetContentPath();

	//if (pLevelScript->GetRealtivePath() == L"Level\\Stage3_Floor2.lv")
	//{
	//	pLoadedLevel = pCurLevel->g_LevelLoad(ContentPath + L"Level\\Stage3_Floor2.lv");
	//}

	// �ӽ÷� ������ ������ �ҷ��´�

	pLoadedLevel = pCurLevel->g_LevelLoad(ContentPath + L"ProgressLevel\\Temp.lv");
	

	if (!pLoadedLevel) {
		// �α� ���
		return;
	}

	vector<CGameObject*> pTempObjVector = pLoadedLevel->GetLayer(2)->GetParentObjects();
	vector<CGameObject*> pTempObjVector2 = pLoadedLevel->GetLayer(0)->GetParentObjects();

	// ���� �� Player ������ �����´�.
	tTempInfo pTempInfo = pLevelScript->GetTempInfo();

	if (pTempObjVector.empty() || pTempObjVector2.empty()) {
		// �α� ��� �Ǵ� ���� ó��
		return;
	}

	// ���� ������ �÷��̾� ������ �Ѱ� �ش�.
	for (UINT i = 0; i < pTempObjVector.size(); ++i)
	{
		if (pTempObjVector[i]->GetName() == L"Player")
		{
			CPlayerScript* TempPlayerScript = (CPlayerScript*)pTempObjVector[i]->GetScripts()[0];
			TempPlayerScript->SetFade(true);
			TempPlayerScript->SetRounds(pTempInfo.CurWeaponRounds);
			TempPlayerScript->SetMask(pTempInfo.CurMask);
			TempPlayerScript->SetWeapon(pTempInfo.CurWeapon);
			TempPlayerScript->SetEquipGun(pTempInfo.EquippedGun);
			//TempPlayerScript->SetActionKey(0);
		}
	}

	// ���� ������ ���� Ŭ���� ������ �Ѱ� �ش�. 
	for (UINT i = 0; i < pTempObjVector2.size(); ++i)
	{
		if (pTempObjVector2[i]->GetName() == L"LevelInfo")
		{
			CLevelScript* TempLevelScript = (CLevelScript*)pTempObjVector2[i]->GetScripts()[0];
			TempLevelScript->SetStageRound(pLevelScript->GetStageRound());
			TempLevelScript->SetStart(false);
		}
	}

	ChangeLevel(pLoadedLevel, LEVEL_STATE::PLAY);
}



vector<Vec3> CPlayerScript::CalcWallVector(Vec3 _PlayerPos, Vec3 _WallPos, Vec3 _WallScale, Vec3 _WallRotation)
{
	Vec3 normal(0, 0, 0);
	vector<Vec3> vecNormal = {};

	float MaxWallPosX = 0.f;
	float MinWallPosX = 0.f;

	float MaxWallPosY = 0.f;
	float MinWallPosY = 0.f;

	// ����
	if (_WallRotation.z == 0.f)
	{
		MaxWallPosX = _WallPos.x + _WallScale.x / 2.f;
		MinWallPosX = _WallPos.x - _WallScale.x / 2.f;

		MaxWallPosY = _WallPos.y + _WallScale.y / 2.f;
		MinWallPosY = _WallPos.y - _WallScale.y / 2.f;
	}
	// ����
	else
	{
		MaxWallPosX = _WallPos.x + _WallScale.y / 2.f;
		MinWallPosX = _WallPos.x - _WallScale.y / 2.f;

		MaxWallPosY = _WallPos.y + _WallScale.x / 2.f;
		MinWallPosY = _WallPos.y - _WallScale.x / 2.f;

	}

	// ���� ������
	if (_PlayerPos.x > MaxWallPosX) 
	{
		normal = Vec3(-1, 0, 0); 
		vecNormal.push_back(normal);
	}

	// ���� ����
	else if (_PlayerPos.x < MinWallPosX) 
	{
		normal = Vec3(1, 0, 0); 
		vecNormal.push_back(normal);
	}

	// ���� �Ʒ���
	if (_PlayerPos.y > MaxWallPosY) 
	{
		normal = Vec3(0, -1, 0); 
		vecNormal.push_back(normal);
	}

	// ���� ����
	else if (_PlayerPos.y < MinWallPosY) 
	{
		normal = Vec3(0, 1, 0); 
		vecNormal.push_back(normal);
	}

	return vecNormal;
}

void CPlayerScript::RegisterMask()
{
	if (m_Mask != MASK_TYPE::END)
	{
		CGameObject* pMask = CLevelMgr::GetInst()->FindObjectByName(L"IngameMask");

		if (pMask)
		{
			AddChild(GetOwner(), pMask);
			
			m_IsMaskOn = true;
		}		
	}		
}


void CPlayerScript::SaveComponent(FILE* _File)
{
	fwrite(&m_PlayerSpeed, sizeof(float), 1, _File);
	//fwrite(&m_CurWeaponRounds, sizeof(int), 1, _File);
	//fwrite(&m_Mask, sizeof(int), 1, _File);
	//fwrite(&m_CurWeapon, sizeof(int), 1, _File);
	//fwrite(&m_EquipGun, sizeof(bool), 1, _File);
	//fwrite(&m_ActionedKey, sizeof(int), 1, _File);

	SaveAssetRef(m_TargetTex, _File);
}

void CPlayerScript::LoadComponent(FILE* _File)
{
	fread(&m_PlayerSpeed, sizeof(float), 1, _File);
	//fread(&m_CurWeaponRounds, sizeof(int), 1, _File);
	//fread(&m_Mask, sizeof(int), 1, _File);
	//fread(&m_CurWeapon, sizeof(int), 1, _File);
	//fread(&m_EquipGun, sizeof(bool), 1, _File);
	//fread(&m_ActionedKey, sizeof(int), 1, _File);
	//fread(&m_PaperBurnIntence, sizeof(float), 1, _File);
	LoadAssetRef(m_TargetTex, _File);
}

