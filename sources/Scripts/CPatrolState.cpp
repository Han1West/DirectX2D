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

	// 각도를 0 ~ 360 사이로 정규화한다.
	EnemyTheta = fmod(EnemyTheta, 360.f);

	if (EnemyTheta < 0)
		EnemyTheta += 360.f;

	// 자신의 근처에 있는 벽을 구해온다.
	vector<CGameObject*> vecAroundWalls = FindWallAround(Vec2(vEnemyPos.x, vEnemyPos.y), m_WallDetectRange);
	vector<CGameObject*> vecAroundDoor = FindDoorAround(Vec2(vEnemyPos.x, vEnemyPos.y), m_WallDetectRange);

	vecAroundWalls.insert(vecAroundWalls.end(), vecAroundDoor.begin(), vecAroundDoor.end());
	bool WallFront = false;

	// 자신의 앞에 벽이 있는지 확인
	for (int i = 0; i < vecAroundWalls.size(); ++i)
	{
		Vec4 MaxMinPos = GetMaxMinPosWall(vecAroundWalls[i]);

		float WallMaxX = MaxMinPos.x;
		float WallMinX = MaxMinPos.z;

		float WallMaxY = MaxMinPos.y;
		float WallMinY = MaxMinPos.w;

		if (EnemyTheta == 0.f) // 오른쪽을 바라봄
		{
			if ((WallMinY < vEnemyPos.y && vEnemyPos.y < WallMaxY)
				&& (vEnemyPos.x < WallMinX))
			{
				m_FrontWallDis = WallMinX - vEnemyPos.x;
				WallFront = true;
			}
		}
		else if (EnemyTheta == 90.f) // 윗쪽을 바라봄
		{
			if ((WallMinX < vEnemyPos.x && vEnemyPos.x < WallMaxX)
				&& (vEnemyPos.y < WallMaxY))
			{
				m_FrontWallDis = WallMinY - vEnemyPos.y;
				WallFront = true;
			}
		}
		else if (EnemyTheta == 270.f) // 아래를 바라봄
		{
			if ((WallMinX < vEnemyPos.x && vEnemyPos.x < WallMaxX)
				&& (WallMaxY < vEnemyPos.y))
			{
				m_FrontWallDis = vEnemyPos.y - WallMaxY;
				WallFront = true;
			}
		}
		else if (EnemyTheta == 180.f) // 왼쪽을 바라봄
		{
			if ((WallMinY < vEnemyPos.y && vEnemyPos.y < WallMaxY)
				&& (WallMaxX < vEnemyPos.x))
			{
				m_FrontWallDis = vEnemyPos.x - WallMaxX;
				WallFront = true;
			}
		}
	} 

	// 자신의 앞에 벽이 있다면 회전한 방향을 정한다
	if (WallFront)
	{
		bool Right = false;
		bool Left = false;
		// 자신의 오른쪽, 왼쪽에 벽이 있는지 확인
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
				// 옆의 벽의 조건
				if (WallMinX < vEnemyPos.x && vEnemyPos.x < WallMaxX)
				{
					// 오른쪽
					if (WallMaxY < vEnemyPos.y)
						Right = true;

					// 왼쪽
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
				// 옆의 벽의 조건
				if (WallMinX < vEnemyPos.x && vEnemyPos.x < WallMaxX)
				{
					// 오른쪽
					if (vEnemyPos.y < WallMinY)
						Right = true;

					// 왼쪽
					if (WallMaxY < vEnemyPos.y)
						Left = true;
				}
			}
		}

		
		// 오른쪽에 벽이 있다면 왼쪽으로 방향전환
		if (Right && m_FrontWallDis < 50.f)
		{
			EnemyTheta += 90.f;
		}
		// 왼쪽에 벽이 있다면 오른쪽으로 방향전환
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

	// 자신이 바라보는 방향으로 이동한다.
	Vec2 Dir = Vec2(cos(vEnemyRot.z), sin(vEnemyRot.z));

	vEnemyPos.x += m_Speed * Dir.x * DT;
	vEnemyPos.y += m_Speed * Dir.y * DT;

	GetOwner()->GetOwner()->Transform()->SetRelativePos(vEnemyPos);

	
	// 현재 플레이어의 위치
	Vec3 vPlayerPos = CLevelMgr::GetInst()->FindObjectByName(L"Player")->Transform()->GetRelativePos();
	

	float Distance = vPlayerPos.Distance(vPlayerPos, vEnemyPos);

	// 플레이어가 탐지 범위 내에 들어왔다.
	if (Distance < m_DetectRange)
	{
		// 만약 적과 플레이어 사이에 벽이 있다면 탐지를 못한다.
		if (!FindWall(vPlayerPos, vEnemyPos))
		{
			//// 플레이어를 추적한다
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
				// 플레이어를 추적한다
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

