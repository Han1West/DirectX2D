#include "pch.h"
#include "CDoorScript.h"

CDoorScript::CDoorScript()
	: CScript(SCRIPT_TYPE::DOORSCRIPT)
	, m_Speed(12.f)
	, m_IsOpening(false)
	, m_IsPlayerOpen(false)
{
}

CDoorScript::~CDoorScript()
{
}

void CDoorScript::Begin()
{
	float DoorRotZ = Transform()->GetRelativeRotation().z;
	m_OriginalPos = Transform()->GetRelativePos();

	if (0 == DoorRotZ)
	{
		m_IsHorizontal = true;
	}
	else
	{
		m_IsHorizontal = false;
	}
}

void CDoorScript::Tick()
{
	CPlayerScript* pPlayerScript = (CPlayerScript*)CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScripts()[0];
	if (pPlayerScript->IsPaued())
		return;

	if (m_IsOpening)
	{
		m_AccTime += DT;
		if(m_AccTime < 0.3f)
			OpenDoor(m_OtherObjPos);
		else
		{
			m_IsPlayerOpen = false;
			m_IsOpening = false;
			m_AccTime = 0.f;
		}
	}
}

void CDoorScript::OpenDoor(Vec3 _OtherObjPos)
{
	Vec3 DoorPos = Transform()->GetRelativePos();
	Vec3 DoorScale = Transform()->GetRelativeScale();
	Vec3 DoorRot = Transform()->GetRelativeRotation();
	float degrees = DoorRot.z * (180.f / XM_PI);
	float additionalOffset = 0.f;

	// 수평문 오른쪽 고정
	if (m_IsHorizontal)
	{
		// 위에서 아래로
		if (DoorPos.y < _OtherObjPos.y)
		{
			DoorRot.z += m_Speed * DT;

			degrees = DoorRot.z * (180.f / XM_PI);

			if (degrees > 120.f)
				degrees = 120.f;
				
			if (degrees > 90.f)
			{
				additionalOffset = sin((fabs(degrees)) - 90.f) * (DoorScale.x / 4.f);
			}
				
			DoorRot.z = degrees * (XM_PI / 180.f);
		}
		// 아래서 위로
		else
		{
			DoorRot.z -= m_Speed * DT;

			degrees = DoorRot.z * (180.f / XM_PI);			

			if (degrees < -120.f)
				degrees = -120.f;

			if (degrees < -90.f)
			{
				additionalOffset = sin((fabs(degrees)) - 90.f) * (DoorScale.x / 4.f);
			}

			float MoveDis = fabs(sin(DoorRot.z) * (DoorScale.x / 2.f));

			DoorRot.z = degrees * (XM_PI / 180.f);
		}
		// 현재 각도가 음수
		if (degrees < 0)
		{
			DoorPos.x = m_OriginalPos.x + fabs(sin(DoorRot.z) * (DoorScale.x / 2.f)) + fabs(additionalOffset);
			DoorPos.y = m_OriginalPos.y + fabs(sin(DoorRot.z) * (DoorScale.x / 2.f));
		}
		else
		{
			DoorPos.x = m_OriginalPos.x + fabs(sin(DoorRot.z) * (DoorScale.x / 2.f)) + fabs(additionalOffset);
			DoorPos.y = m_OriginalPos.y - fabs(sin(DoorRot.z) * (DoorScale.x / 2.f));
		}

		Transform()->SetRelativePos(DoorPos);
		Transform()->SetRelativeRotation(DoorRot);
	}
	
	// 수직문 위쪽 고정
	else
	{
		// 오른쪽에서 왼쪽
		if (DoorPos.x < _OtherObjPos.x)
		{
			DoorRot.z -= m_Speed * DT;

			degrees = DoorRot.z * (180.f / XM_PI);

			if (degrees < -30.f)
				degrees = -30.f;

			if (degrees < 0.f)
			{
				additionalOffset = cos((fabs(degrees)) - 90.f) * (DoorScale.x / 4.f);
			}

			DoorRot.z = degrees * (XM_PI / 180.f);
		}
		// 왼쪽에서 오른쪽
		else
		{
			DoorRot.z += m_Speed * DT;

			degrees = DoorRot.z * (180.f / XM_PI);

			if (degrees > 210.f)
				degrees = 210.f;

			if (degrees > 180.f )
			{
				additionalOffset = cos((fabs(degrees)) - 90.f) * (DoorScale.x / 4.f);
			}

			DoorRot.z = degrees * (XM_PI / 180.f);
		}
		if (degrees < 90)
		{
			DoorPos.x = m_OriginalPos.x - fabs(cos(DoorRot.z) * (DoorScale.x / 2.f));
			DoorPos.y = m_OriginalPos.y + fabs(cos(DoorRot.z) * (DoorScale.x / 2.f)) + fabs(additionalOffset);
		}
		else
		{
			DoorPos.x = m_OriginalPos.x + fabs(cos(DoorRot.z) * (DoorScale.x / 2.f));
			DoorPos.y = m_OriginalPos.y + fabs(cos(DoorRot.z) * (DoorScale.x / 2.f)) + fabs(additionalOffset);
		}
		

		Transform()->SetRelativePos(DoorPos);
		Transform()->SetRelativeRotation(DoorRot);
	}

}

void CDoorScript::BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	if (_OtherCollider->GetName() == L"Player")
	{
		if (!m_IsOpening)
		{
			// 소리 재생
			Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\etc\\DoorOpen.wav", L"Sound\\etc\\DoorOpen.wav");
			pSound->Play(1, 0.5f, false);
			m_IsOpening = true;
			m_IsPlayerOpen = true;
			m_OtherObjPos = _OtherObject->Transform()->GetRelativePos();
		}		
	}
	if (_OtherCollider->GetName() == L"Bullet"
		|| _OtherCollider->GetName() == L"Enemy" || _OtherCollider->GetName() == L"FatEnemy")
	{
		if (!m_IsOpening)
		{
			// 소리 재생
			Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\etc\\DoorOpen.wav", L"Sound\\etc\\DoorOpen.wav");
			pSound->Play(1, 0.5f, false);
			m_IsOpening = true;
			m_OtherObjPos = _OtherObject->Transform()->GetRelativePos();
		}
	}
}

void CDoorScript::Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CDoorScript::EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CDoorScript::SaveComponent(FILE* _File)
{
}

void CDoorScript::LoadComponent(FILE* _File)
{
}

