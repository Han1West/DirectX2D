#include "pch.h"
#include "CPatrolState.h"

#include "CEnemyScript.h"
#include "CFatEnemyScript.h"
#include "CPlayerScript.h"

#include <Engine/CLevelMgr.h>

CPatrolState::CPatrolState()
	: CState(STATE_TYPE::PATROLSTATE)
	, m_Speed(100.f)
	, m_DetectRange(600.f)
	, m_WallDetectRange(70.f)
	, m_FrontWallDis(0.f)
{
}

CPatrolState::~CPatrolState()
{
}


void CPatrolState::Enter()
{
	if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
	{
		CEnemyScript* pScript = dynamic_cast<CEnemyScript*>(m_Owner);
		pScript->PlayWalkFlip();
	}
	if (SCRIPT_TYPE::FATENEMYSCRIPT == m_Owner->GetScriptType())
	{
		CFatEnemyScript* pScript = dynamic_cast<CFatEnemyScript*>(m_Owner);
		pScript->PlayWalkFlip();
	}
}

void CPatrolState::FinalTick()
{
	Vec3 vEnemyPos = GetOwner()->GetOwner()->Transform()->GetRelativePos();
	Vec3 vEnemyRot = GetOwner()->GetOwner()->Transform()->GetRelativeRotation();
	
	float EnemyTheta = vEnemyRot.z * (180.f / XM_PI);

	// ������ 0 ~ 360 ���̷� ����ȭ�Ѵ�.
	EnemyTheta = fmod(EnemyTheta, 360.f);

	if (EnemyTheta < 0)
		EnemyTheta += 360.f;

	// �ڽ��� ��ó�� �ִ� ���� ���ؿ´�.
	vector<CGameObject*> vecAroundWalls = FindWallAround(Vec2(vEnemyPos.x, vEnemyPos.y), m_WallDetectRange);
	vector<CGameObject*> vecAroundDoor = FindDoorAround(Vec2(vEnemyPos.x, vEnemyPos.y), m_WallDetectRange);

	vecAroundWalls.insert(vecAroundWalls.end(), vecAroundDoor.begin(), vecAroundDoor.end());
	bool WallFront = false;

	// �ڽ��� �տ� ���� �ִ��� Ȯ��
	for (int i = 0; i < vecAroundWalls.size(); ++i)
	{
		Vec4 MaxMinPos = GetMaxMinPosWall(vecAroundWalls[i]);

		float WallMaxX = MaxMinPos.x;
		float WallMinX = MaxMinPos.z;

		float WallMaxY = MaxMinPos.y;
		float WallMinY = MaxMinPos.w;

		if (EnemyTheta == 0.f) // �������� �ٶ�
		{
			if ((WallMinY < vEnemyPos.y && vEnemyPos.y < WallMaxY)
				&& (vEnemyPos.x < WallMinX))
			{
				m_FrontWallDis = WallMinX - vEnemyPos.x;
				WallFront = true;
			}
		}
		else if (EnemyTheta == 90.f) // ������ �ٶ�
		{
			if ((WallMinX < vEnemyPos.x && vEnemyPos.x < WallMaxX)
				&& (vEnemyPos.y < WallMaxY))
			{
				m_FrontWallDis = WallMinY - vEnemyPos.y;
				WallFront = true;
			}
		}
		else if (EnemyTheta == 270.f) // �Ʒ��� �ٶ�
		{
			if ((WallMinX < vEnemyPos.x && vEnemyPos.x < WallMaxX)
				&& (WallMaxY < vEnemyPos.y))
			{
				m_FrontWallDis = vEnemyPos.y - WallMaxY;
				WallFront = true;
			}
		}
		else if (EnemyTheta == 180.f) // ������ �ٶ�
		{
			if ((WallMinY < vEnemyPos.y && vEnemyPos.y < WallMaxY)
				&& (WallMaxX < vEnemyPos.x))
			{
				m_FrontWallDis = vEnemyPos.x - WallMaxX;
				WallFront = true;
			}
		}
	} 

	// �ڽ��� �տ� ���� �ִٸ� ȸ���� ������ ���Ѵ�
	if (WallFront)
	{
		bool Right = false;
		bool Left = false;
		// �ڽ��� ������, ���ʿ� ���� �ִ��� Ȯ��
		for (int i = 0; i < vecAroundWalls.size(); ++i)
		{
			//Vec3 WallPos = vecAroundWalls[i]->Transform()->GetRelativePos();
			//Vec3 WallScale = vecAroundWalls[i]->Transform()->GetRelativeScale();

			Vec4 MaxMinPos = GetMaxMinPosWall(vecAroundWalls[i]);

			float WallMaxX = MaxMinPos.x;
			float WallMinX = MaxMinPos.z;
			
			float WallMaxY = MaxMinPos.y;			
			float WallMinY = MaxMinPos.w;

			if (EnemyTheta == 0.f)
			{
				// ���� ���� ����
				if (WallMinX < vEnemyPos.x && vEnemyPos.x < WallMaxX)
				{
					// ������
					if (WallMaxY < vEnemyPos.y)
						Right = true;

					// ����
					if (vEnemyPos.y < WallMinY)
						Left = true;
				}				
			}
			else if (EnemyTheta == 90.f)
			{
				if (WallMinY < vEnemyPos.y && vEnemyPos.y < WallMaxY)
				{				
					if (vEnemyPos.x < WallMinX)
						Right = true;

					if (WallMaxX < vEnemyPos.x)
						Left = true;
				}
			}
			else if (EnemyTheta == 270.f)
			{
				if (WallMinY < vEnemyPos.y && vEnemyPos.y < WallMaxY)
				{
					if (WallMaxX < vEnemyPos.x)
						Right = true;

					if (vEnemyPos.x < WallMinX)
						Left = true;
				}
			}
			else if (EnemyTheta == 180.f)
			{
				// ���� ���� ����
				if (WallMinX < vEnemyPos.x && vEnemyPos.x < WallMaxX)
				{
					// ������
					if (vEnemyPos.y < WallMinY)
						Right = true;

					// ����
					if (WallMaxY < vEnemyPos.y)
						Left = true;
				}
			}
		}

		
		// �����ʿ� ���� �ִٸ� �������� ������ȯ
		if (Right && m_FrontWallDis < 50.f)
		{
			EnemyTheta += 90.f;
		}
		// ���ʿ� ���� �ִٸ� ���������� ������ȯ
		if (Left && m_FrontWallDis < 50.f)
		{
			EnemyTheta -= 90.f;
		}

		if (!Right && !Left && m_FrontWallDis < 50.f)
		{
			EnemyTheta += 90.f;
		}

		vEnemyRot.z = EnemyTheta* (XM_PI / 180.f);

		GetOwner()->GetOwner()->Transform()->SetRelativeRotation(vEnemyRot);
	}

	// �ڽ��� �ٶ󺸴� �������� �̵��Ѵ�.
	Vec2 Dir = Vec2(cos(vEnemyRot.z), sin(vEnemyRot.z));

	vEnemyPos.x += m_Speed * Dir.x * DT;
	vEnemyPos.y += m_Speed * Dir.y * DT;

	GetOwner()->GetOwner()->Transform()->SetRelativePos(vEnemyPos);

	
	// ���� �÷��̾��� ��ġ
	Vec3 vPlayerPos = CLevelMgr::GetInst()->FindObjectByName(L"Player")->Transform()->GetRelativePos();
	

	float Distance = vPlayerPos.Distance(vPlayerPos, vEnemyPos);

	// �÷��̾ Ž�� ���� ���� ���Դ�.
	if (Distance < m_DetectRange)
	{
		// ���� ���� �÷��̾� ���̿� ���� �ִٸ� Ž���� ���Ѵ�.
		if (!FindWall(vPlayerPos, vEnemyPos))
		{
			//// �÷��̾ �����Ѵ�
			if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
			{
				CEnemyScript* pScript = (CEnemyScript*)m_Owner;
				pScript->SetDestination(vPlayerPos);
				if (pScript->IsEquippedGun())
					pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CAttackState"));
				else
					pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CFindState"));
			}
			else if (SCRIPT_TYPE::FATENEMYSCRIPT == m_Owner->GetScriptType())
			{
				CFatEnemyScript* pScript = (CFatEnemyScript*)m_Owner;
				pScript->SetDestination(vPlayerPos);
				pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CFindState"));
			}
		}
		else
		{
			CGameObject* pPlayer = CLevelMgr::GetInst()->FindObjectByName(L"Player");
			CPlayerScript* pPlayerScript = (CPlayerScript*)pPlayer->GetScripts()[0];
			if (pPlayerScript->IsMakeGunSound())
			{
				// �÷��̾ �����Ѵ�
				if (SCRIPT_TYPE::ENEMYSCRIPT == m_Owner->GetScriptType())
				{
					CEnemyScript* pScript = (CEnemyScript*)m_Owner;
					pScript->SetDestination(vPlayerPos);
					pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CTraceState"));

				}
				else if (SCRIPT_TYPE::FATENEMYSCRIPT == m_Owner->GetScriptType())
				{
					CFatEnemyScript* pScript = (CFatEnemyScript*)m_Owner;
					pScript->SetDestination(vPlayerPos);
					pScript->SetState(CStateMgr::ChangeState(pScript->GetState(), L"CTraceState"));
				}
				return;
			}
		}
	}
}

void CPatrolState::Exit()
{
}

