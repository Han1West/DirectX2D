#include "pch.h"
#include "CChangeFloorScript.h"


#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CPathMgr.h>

#include "CLevelScript.h"
#include "CPlayerScript.h"
#include "CPhoneScript.h"

CChangeFloorScript::CChangeFloorScript()
	: CScript(SCRIPT_TYPE::CHANGEFLOORSCRIPT)
	, m_bNext(false)
	, m_CollDir(Vec3(0, 0, 0))
{
}

CChangeFloorScript::~CChangeFloorScript()
{
}

void CChangeFloorScript::Begin()
{
}

void CChangeFloorScript::Tick()
{

}

void CChangeFloorScript::BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	if (_OtherCollider->GetName() == L"Player")
	{
		CLevelScript* pScript = (CLevelScript*)CLevelMgr::GetInst()->FindObjectByName(L"LevelInfo")->GetScripts()[0];

		if (pScript->IsFloorClear() || pScript->IsAlreadyClear())
		{
			Vec3 vPlayerPos = _OtherObject->Transform()->GetRelativePos();
			Vec3 vPos = Transform()->GetRelativePos();
			Vec3 vScale = Transform()->GetRelativeScale();
			// �浹 ���� ��� (�ڽ� -> �ٸ� ������Ʈ ����)
			Vec3 vCollisionDirection = vPos - vPlayerPos;

			// ���� ���͸� ����ȭ
			vCollisionDirection.Normalize();
			m_CollDir = NormalizeDir(vCollisionDirection, vScale.x, vScale.y);
			ChangeFloor();
		}
		if (pScript->IsHome())
		{
			ChangeFloorAtHome();
		}
	}

}

void CChangeFloorScript::Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{


}

void CChangeFloorScript::EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CChangeFloorScript::SaveComponent(FILE* _File)
{
	fwrite(&m_bNext, sizeof(bool), 1, _File);
}

void CChangeFloorScript::LoadComponent(FILE* _File)
{
	fread(&m_bNext, sizeof(bool), 1, _File);
}

void CChangeFloorScript::ChangeFloor()                                 
{
	CLevelScript* pLevelScript = (CLevelScript*)CLevelMgr::GetInst()->FindObjectByName(L"LevelInfo")->GetScripts()[0];
	CPlayerScript* pPlayerScript = (CPlayerScript*)CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScripts()[0];

	// ���� ������ �����´�.
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	wstring ContentPath = CPathMgr::GetInst()->GetContentPath();

	// ���� ������ �̸��� ��θ� �����´�.
	wstring CurLevelPath = pLevelScript->GetRealtivePath();
	size_t LastSlash = CurLevelPath.find_last_of(L"\\/");
	wstring LoadedLevelName = CurLevelPath.substr(LastSlash + 1);
	
	wstring LoadedPrevLevelName = L"";
	wstring LoadedNextLevelName = L"";

	// ���� ������ �̸��� ��θ� �����´�
	wstring PrevLevelPath = pLevelScript->GetPrevLevelPath();
	if(PrevLevelPath != L"None")
		LoadedPrevLevelName = PrevLevelPath.substr(LastSlash + 1);
	
	// ���� ������ �̸��� ��θ� �����´�
	wstring NextLevelPath = pLevelScript->GetNextLevelPath();
	if (NextLevelPath != L"None")
		LoadedNextLevelName = NextLevelPath.substr(LastSlash + 1);

	// ���� �÷��̾��� ���¸� �����´�.
	WEAPON_TYPE CurPlayerWeapon = pPlayerScript->GetPlayerWeaponType();
	int CurPlayerWeaponRounds = pPlayerScript->GetCurRounds();
	MASK_TYPE CurPlayerMask = pPlayerScript->GetMask();
	bool CurPlayerEquipedGun = pPlayerScript->IsEquippedGun();
	int CurActionedKey = pPlayerScript->GetCurrentActionedKey();


	// �������� ����
	if (m_bNext)
	{
		// ���ο� ��ο� ������ ������ �����Ѵ�.
		ChangeLevelState(LEVEL_STATE::STOP);
		pCurLevel->g_LevelSave(ContentPath + L"ProgressLevel\\" + LoadedLevelName, pCurLevel);
		
		CLevel* pLoadedLevel = nullptr;

		// ������ Ŭ����� ���¶�� �����ߴ� ���� �ҷ��´�
		if (pLevelScript->IsLevelClear())
		{
			pLoadedLevel = pCurLevel->g_LevelLoad(ContentPath + L"ProgressLevel\\" + LoadedNextLevelName);
		}
		// ���� ������ �ҷ��´�.
		else
		{
			pLoadedLevel = pCurLevel->g_LevelLoad(ContentPath + pLevelScript->GetNextLevelPath());
		}
		
	
		
		vector<CGameObject*> pNextObjVector = pLoadedLevel->GetLayer(2)->GetParentObjects();
		vector<CGameObject*> pNextObjVector2 = pLoadedLevel->GetLayer(0)->GetParentObjects();

		// ���� ������ �÷��̾� ������ �Ѱ� �ش�.
		for (UINT i = 0; i < pNextObjVector.size(); ++i)
		{
			if (pNextObjVector[i]->GetName() == L"Player")
			{
				// �÷��̾� ��ġ ���� (�ٷ� �浹�� ���´�)
				Vec3 NewPos = AdjustPlayerPos(pNextObjVector[i]->Transform()->GetRelativePos());
				pNextObjVector[i]->Transform()->SetRelativePos(NewPos);

				CPlayerScript* NextPlayerScript = (CPlayerScript*)pNextObjVector[i]->GetScripts()[0];			
				NextPlayerScript->SetFade(true);
				NextPlayerScript->SetRounds(CurPlayerWeaponRounds);
				NextPlayerScript->SetMask(CurPlayerMask);
				NextPlayerScript->SetWeapon(CurPlayerWeapon);
				NextPlayerScript->SetEquipGun(CurPlayerEquipedGun);
				NextPlayerScript->SetActionKey(CurActionedKey);
			}
		}


		// ���� ������ ���� Ŭ���� ������ �Ѱ� �ش�. 
		for (UINT i = 0; i < pNextObjVector2.size(); ++i)
		{
			if (pNextObjVector2[i]->GetName() == L"LevelInfo")
			{
				CLevelScript* CurLevelScript = (CLevelScript*)CLevelMgr::GetInst()->FindObjectByName(L"LevelInfo")->GetScripts()[0];
				CLevelScript* NextLevelScript = (CLevelScript*)pNextObjVector2[i]->GetScripts()[0];
				NextLevelScript->SetStageRound(pLevelScript->GetStageRound());
				if (pLevelScript->IsLevelClear() || pLevelScript->IsAlreadyClear())
				{
					NextLevelScript->SetAlreadyClear(true);
					NextLevelScript->SetLevelEnd(true);
					NextLevelScript->SetBGM(CurLevelScript->GetBGM());
				}
					
			}
		}
		 
		ChangeLevel(pLoadedLevel, LEVEL_STATE::PLAY);
	}

	// �Ʒ������� ����
	else
	{
		ChangeLevelState(LEVEL_STATE::STOP);
		pCurLevel->g_LevelSave(ContentPath + L"ProgressLevel\\" + LoadedLevelName , pCurLevel);
		
		// ���� ������ �ҷ��´�.
		CLevel* pLoadedLevel = pCurLevel->g_LevelLoad(ContentPath + L"ProgressLevel\\" + LoadedPrevLevelName);


		vector<CGameObject*> pPrevObjVector = pLoadedLevel->GetLayer(2)->GetParentObjects();
		vector<CGameObject*> pPrevObjVector2 = pLoadedLevel->GetLayer(0)->GetParentObjects();

		// ���� ������ �÷��̾� ������ �Ѱ� �ش�.
		for (UINT i = 0; i < pPrevObjVector.size(); ++i)
		{
			if (pPrevObjVector[i]->GetName() == L"Player")
			{
				// �÷��̾� ��ġ ���� (�ٷ� �浹�� ���´�)
				Vec3 NewPos = AdjustPlayerPos(pPrevObjVector[i]->Transform()->GetRelativePos());
				pPrevObjVector[i]->Transform()->SetRelativePos(NewPos);

				CPlayerScript* PrevPlayerScript = (CPlayerScript*)pPrevObjVector[i]->GetScripts()[0];
				PrevPlayerScript->SetFade(true);
				PrevPlayerScript->SetRounds(CurPlayerWeaponRounds);
				PrevPlayerScript->SetMask(CurPlayerMask);
				PrevPlayerScript->SetWeapon(CurPlayerWeapon);
				PrevPlayerScript->SetEquipGun(CurPlayerEquipedGun);
				PrevPlayerScript->SetActionKey(CurActionedKey);
				PrevPlayerScript->SetState(PLAYER_STATE::PAUSERELEASD);
			}
		}

		// ���� ������ ���� Ŭ���� ������ �Ѱ� �ش�.
		for (UINT i = 0; i < pPrevObjVector2.size(); ++i)
		{
			if (pPrevObjVector2[i]->GetName() == L"LevelInfo")
			{
				CLevelScript* CurLevelScript = (CLevelScript*)CLevelMgr::GetInst()->FindObjectByName(L"LevelInfo")->GetScripts()[0];
				CLevelScript* PrevLevelScript = (CLevelScript*)pPrevObjVector2[i]->GetScripts()[0];
				PrevLevelScript->SetLevelEnd(CurLevelScript->IsLevelClear());
				PrevLevelScript->SetStageRound(CurLevelScript->GetStageRound());
				PrevLevelScript->SetStart(false);
				PrevLevelScript->SetBGM(CurLevelScript->GetBGM());
				PrevLevelScript->SetAlreadyClear(CurLevelScript->IsLevelClear());
			}
		}
		ChangeLevel(pLoadedLevel, LEVEL_STATE::PLAY);
	}
}

void CChangeFloorScript::ChangeFloorAtHome()
{
	CPlayerScript* pPlayerScript = (CPlayerScript*)CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScripts()[0];
	CLevelScript* pLevelScript = (CLevelScript*)CLevelMgr::GetInst()->FindObjectByName(L"LevelInfo")->GetScripts()[0];
	CPhoneScript* pPhoneScript = (CPhoneScript*)CLevelMgr::GetInst()->FindObjectByName(L"Phone")->GetScripts()[0];

	pPhoneScript->SetRing(false);

	// ���� ������ �����´�.
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	wstring ContentPath = CPathMgr::GetInst()->GetContentPath();

	// ���� ������ ��θ� �����´�
	wstring PrevLevelPath = pLevelScript->GetPrevLevelPath();

	int CurActionedKey = pPlayerScript->GetCurrentActionedKey();

	// �Ʒ������� ����
	ChangeLevelState(LEVEL_STATE::STOP);

	// ���� ������ �ҷ��´�.
	CLevel* pLoadedLevel = pCurLevel->g_LevelLoad(ContentPath + PrevLevelPath);
	vector<CGameObject*> pPrevObjVector = pLoadedLevel->GetLayer(0)->GetParentObjects();

	// ���� ������ ���� Ŭ���� ������ �Ѱ� �ش�.
	for (UINT i = 0; i < pPrevObjVector.size(); ++i)
	{
		if (pPrevObjVector[i]->GetName() == L"LevelInfo")
		{
			CLevelScript* PrevLevelScript = (CLevelScript*)pPrevObjVector[i]->GetScripts()[0];
			PrevLevelScript->SetStageRound(pLevelScript->GetStageRound());
		}
	}

	vector<CGameObject*> pPrevObjVector2 = pLoadedLevel->GetLayer(2)->GetParentObjects();
	// ���� ������ �÷��̾� ������ �Ѱ� �ش�.
	for (UINT i = 0; i < pPrevObjVector2.size(); ++i)
	{
		if (pPrevObjVector2[i]->GetName() == L"Player")
		{
			CPlayerScript* PrevPlayerScript = (CPlayerScript*)pPrevObjVector2[i]->GetScripts()[0];
			PrevPlayerScript->SetFade(true);
			PrevPlayerScript->SetActionKey(CurActionedKey);
			PrevPlayerScript->SetState(PLAYER_STATE::PAUSERELEASD);
		}
	}

	ChangeLevel(pLoadedLevel, LEVEL_STATE::PLAY);
}

Vec3 CChangeFloorScript::AdjustPlayerPos(Vec3 _Pos)
{
	if (m_CollDir == Vec3(1, 0, 0))
	{
		_Pos.x -= 25.f;
	}
	else if (m_CollDir == Vec3(-1, 0, 0))
	{
		_Pos.x += 35.f;
	}
	else if (m_CollDir == Vec3(0, 1, 0))
	{
		_Pos.y += 15.f;
	}
	else
	{
		_Pos.y -= 15.f;
	}

	return _Pos;
}

Vec3 CChangeFloorScript::NormalizeDir(const Vec3& _Dir, float _SizeX, float _SizeY)
{
	float xFactor = abs(_Dir.x) / _SizeX;   // X�� ����
	float yFactor = abs(_Dir.y) / _SizeY; // Y�� ����

	// x�� y�� �������� ���� ū ���� ���� Ȯ��
	if (xFactor > yFactor)
	{
		return _Dir.x > 0 ? Vec3(1, 0, 0) : Vec3(-1, 0, 0); // x�� �� ũ�� x�� �������� ����ȭ
	}
	else 
	{		
		return _Dir.y > 0 ? Vec3(0, 1, 0) : Vec3(0, -1, 0); // y�� �� ũ�� y�� �������� ����ȭ
	}
}
