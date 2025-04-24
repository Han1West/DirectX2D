#include "pch.h"
#include "CCameraScript.h"

#include <windows.h>
#include <Engine/CEngine.h>
#include <Engine/CKeyMgr.h>
#include <Engine/CTimeMgr.h>
#include <Engine/CTransform.h>
#include <Engine/CCamera.h>
#include <Engine/CLevelMgr.h>
#include <Engine/CConstBuffer.h>
#include <Engine/CDevice.h>

#include "CPlayerScript.h"

CCameraScript::CCameraScript()
	: CScript((UINT)SCRIPT_TYPE::CAMERASCRIPT)
	, m_CameraSpeed(800.f)
	, m_ShakeTime(0.f)
	, m_MaximamPos()
	, m_DirX(0.f)
	, m_DirY(0.f)
	, m_bSearch(false)
	, m_bShake(true)

{
}

CCameraScript::~CCameraScript()
{
}

void CCameraScript::Tick()
{
	if (PROJ_TYPE::ORTHOGRAPHIC == Camera()->GetProjType())
		CameraOrthgraphicMove();
	else
		CameraPerspectiveMove();
}


void CCameraScript::CameraOrthgraphicMove()
{
	CPlayerScript* pPlayerScript = (CPlayerScript*)CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScripts()[0];
	if (pPlayerScript->IsPaued())
		return;

	//Vec3 vWorldPos = Transform()->GetRelativePos();
	Vec3 vSearchPos = Transform()->GetRelativePos();
	Vec2 vMouseRealPos = CKeyMgr::GetInst()->GetMousePos();

	CGameObject* pPlayer = CLevelMgr::GetInst()->FindObjectByName(L"Player");

	// 마우스 커서가 화면 끝에 도달하는지 확인
	Vec2 ScreenScale = CEngine::GetInst()->GetResoulution();
	Vec3 vPlayerPos = pPlayer->Transform()->GetRelativePos();
	Vec3 vMousePos = CKeyMgr::GetInst()->GetRelativeMousePos();

	// 카메라가 이동할 방향을 구한다.
	Vec2 vDir = Vec2(vMousePos.x - vPlayerPos.x, vMousePos.y - vPlayerPos.y);
	vDir.Normalize();
	if (m_bShake)
	{
		m_ShakeTime += DT;
		if(m_ShakeTime <0.5f)
			UpdateShake();
		else
		{
			m_bShake = false;
			m_ShakeTime = 0.f;
			OffShake();
		}
	}
	

	if (m_bSearch)
	{
		CPlayerScript* PlayerScript = (CPlayerScript*)(pPlayer->GetScripts())[0];

		Transform()->SetRelativePos(Vec3((vPlayerPos.x + vMousePos.x) / 2.f , (vPlayerPos.y + vMousePos.y) / 2.f, vSearchPos.z));


		RECT rect;
		GetClientRect(CEngine::GetInst()->GetMainWnd(), &rect);
		MapWindowPoints(CEngine::GetInst()->GetMainWnd(), nullptr, (POINT*)&rect, 2);
		
		if (PlayerScript->GetMask() == MASK_TYPE::GEORGE_GIRAFFE)
		{
			rect.left = rect.left - 400.f;
			rect.right = rect.right + 400.f;
			rect.top = rect.top + 400.f;
			rect.bottom = rect.bottom - 400.f;
		}

		ClipCursor(&rect);
	}
	else
	{
		vSearchPos.x = vPlayerPos.x + vDir.x * 30.f;
		vSearchPos.y = vPlayerPos.y + vDir.y * 30.f;

		Transform()->SetRelativePos(vSearchPos);
	}


	// LSHIFT 조작 시 카메라를 움직인다
	if (KEY_TAP(KEY::LSHIFT))
	{
		// 마우스 커서 방향으로 카메라를 옮김
		m_bSearch = true;
	}

	if (KEY_RELEASED(KEY::LSHIFT))
	{
		m_bSearch = false;
		ClipCursor(NULL);
	}
}

void CCameraScript::CameraPerspectiveMove()
{
	float Speed = m_CameraSpeed;
	if (KEY_PRESSED(KEY::LSHIFT))
		Speed *= 5.f;

	Vec3 vFront = Transform()->GetLocalDir(DIR_TYPE::FRONT);
	Vec3 vRight= Transform()->GetLocalDir(DIR_TYPE::RIGHT);	

	Vec3 vPos = Transform()->GetRelativePos();

	if (KEY_PRESSED(KEY::W))
		vPos += vFront * DT * Speed;
	if(KEY_PRESSED(KEY::S))
		vPos -= vFront * DT * Speed;
	if (KEY_PRESSED(KEY::A))
		vPos -= vRight * DT * Speed;
	if (KEY_PRESSED(KEY::D))
		vPos += vRight * DT * Speed;

	Transform()->SetRelativePos(vPos);

	// 마우스 방향에 따른 오브젝트 회전
	if (KEY_PRESSED(KEY::RBTN))
	{
		Vec3 vRot = Transform()->GetRelativeRotation();

		Vec2 vDir = CKeyMgr::GetInst()->GetMouseDir();
		vRot.y += vDir.x * DT * 15.f;
		vRot.x += vDir.y * DT * 10.f;

		Transform()->SetRelativeRotation(vRot);
	}
}

void CCameraScript::UpdateShake()
{
	m_ShakeTime += DT;

	ShakeData Shake = {};

	// Perlin Noise 또는 간단한 진동 계산
	float offsetX = sin(m_ShakeTime * 30.0f) * 0.003f; // 예제: 사인파 기반
	float offsetY = cos(m_ShakeTime * 30.0f) * 0.003f;

	Shake.ShakeOffset = Vec2(offsetX, offsetY);
	Shake.ShakeStrength = 1.f; // 강도는 필요에 따라 조정

	CConstBuffer* pCB = CDevice::GetInst()->GetCB(CB_TYPE::SHAKE);

	pCB->SetData(&Shake);
	pCB->Binding();
}

void CCameraScript::OffShake()
{
	ShakeData Shake = {};

	// Perlin Noise 또는 간단;한 진동 계산
	float offsetX = 0.f;
	float offsetY = 0.f;

	Shake.ShakeOffset = Vec2(offsetX, offsetY);
	Shake.ShakeStrength = 0.f; // 강도는 필요에 따라 조정

	CConstBuffer* pCB = CDevice::GetInst()->GetCB(CB_TYPE::SHAKE);

	pCB->SetData(&Shake);
	pCB->Binding();
}


void CCameraScript::SaveComponent(FILE* _File)
{
	fwrite(&m_CameraSpeed, sizeof(float), 1, _File);
}

void CCameraScript::LoadComponent(FILE* _File)
{
	fread(&m_CameraSpeed, sizeof(float), 1, _File);
}