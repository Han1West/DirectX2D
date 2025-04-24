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

	// ���콺 Ŀ���� ȭ�� ���� �����ϴ��� Ȯ��
	Vec2 ScreenScale = CEngine::GetInst()->GetResoulution();
	Vec3 vPlayerPos = pPlayer->Transform()->GetRelativePos();
	Vec3 vMousePos = CKeyMgr::GetInst()->GetRelativeMousePos();

	// ī�޶� �̵��� ������ ���Ѵ�.
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


	// LSHIFT ���� �� ī�޶� �����δ�
	if (KEY_TAP(KEY::LSHIFT))
	{
		// ���콺 Ŀ�� �������� ī�޶� �ű�
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

	// ���콺 ���⿡ ���� ������Ʈ ȸ��
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

	// Perlin Noise �Ǵ� ������ ���� ���
	float offsetX = sin(m_ShakeTime * 30.0f) * 0.003f; // ����: ������ ���
	float offsetY = cos(m_ShakeTime * 30.0f) * 0.003f;

	Shake.ShakeOffset = Vec2(offsetX, offsetY);
	Shake.ShakeStrength = 1.f; // ������ �ʿ信 ���� ����

	CConstBuffer* pCB = CDevice::GetInst()->GetCB(CB_TYPE::SHAKE);

	pCB->SetData(&Shake);
	pCB->Binding();
}

void CCameraScript::OffShake()
{
	ShakeData Shake = {};

	// Perlin Noise �Ǵ� ����;�� ���� ���
	float offsetX = 0.f;
	float offsetY = 0.f;

	Shake.ShakeOffset = Vec2(offsetX, offsetY);
	Shake.ShakeStrength = 0.f; // ������ �ʿ信 ���� ����

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