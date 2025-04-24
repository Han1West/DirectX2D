#include "pch.h"
#include "CPhoneScript.h"

#include "CPlayerScript.h"
#include "CLevelScript.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CFontMgr.h>

CPhoneScript::CPhoneScript()
	: CScript(SCRIPT_TYPE::PHONESCRIPT)
	, m_bRing(false)
	, m_bSound(false)
	, m_ActiveScript(false)
	, m_RingSound(nullptr)
	, m_ScriptCount(0)
{
}

CPhoneScript::~CPhoneScript()
{
}

void CPhoneScript::Begin()
{
	m_RingSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\etc\\PhoneCall.wav", L"Sound\\etc\\PhoneCall.wav");
}

void CPhoneScript::Tick()
{
	CPlayerScript* pPlayerScript = (CPlayerScript*)CLevelMgr::GetInst()->FindObjectByName(L"Player")->GetScripts()[0];

	if (pPlayerScript->IsPaued())
	{
		if(nullptr != m_RingSound)
			m_RingSound->SetVolume(0, m_SoundIdx);
		return;
	}
	else
	{
		if (nullptr != m_RingSound)
			m_RingSound->SetVolume(0.3f, m_SoundIdx);

	}
	

	if (!m_bSound)
	{
		m_AccTime += DT;
	}
	

	if (2.f < m_AccTime)
	{
		m_bRing = true;
		m_AccTime = 0.f;
	}
	// º§¼Ò¸®
	if (m_bRing)
	{
		if (!m_bSound)
		{		
			m_SoundIdx = m_RingSound->Play(0, 0.3f, false);
			m_bSound = true;
		}		
	}
	if (!m_bRing)
	{
		m_RingSound->Stop();
	}

	if (m_ActiveScript)
	{
		CGameObject* pPhoneFace = (CGameObject*)CLevelMgr::GetInst()->FindObjectByName(L"PhoneFace");
		pPhoneFace->Transform()->SetRelativeScale(Vec3(200.f, 250.f, 0.f));
		PrintScript();

		if (KEY_TAP(KEY::ENTER) || KEY_TAP(KEY::LBTN))
		{
			m_ScriptCount += 1;
		}
	}
	else
	{
		CGameObject* pPhoneFace = (CGameObject*)CLevelMgr::GetInst()->FindObjectByName(L"PhoneFace");
		pPhoneFace->Transform()->SetRelativeScale(Vec3(0.f, 0.f, 0.f));
	}
}

void CPhoneScript::BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CPhoneScript::Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CPhoneScript::EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CPhoneScript::SaveComponent(FILE* _File)
{
}

void CPhoneScript::LoadComponent(FILE* _File)
{
}

void CPhoneScript::PrintScript()
{
	wstring script = L"";
	CLevelScript* pLevelScript = (CLevelScript*)CLevelMgr::GetInst()->FindObjectByName(L"LevelInfo")->GetScripts()[0];
	int CurRound = pLevelScript->GetStageRound();

	if (CurRound == 0)
	{
		switch (m_ScriptCount)
		{
		case 0:
			script = L"Hello, it's 'Linda'... I need a \nbabysitter right away.";
			CFontMgr::GetInst()->SetStr(script);
			break;
		case 1:
			script = L"Got a few kids that need to be \ndisciplined here.";
			CFontMgr::GetInst()->SetStr(script);
			break;
		case 2:
			script = L"I'm at East 7th Steet.";
			CFontMgr::GetInst()->SetStr(script);
			break;
		case 3:
			CFontMgr::GetInst()->SetStr(script);
			DestroyObject(CLevelMgr::GetInst()->FindObjectByName(L"HighLightIngamePP"));
			m_ActiveScript = false;
			break;
		}
	}
	else if (CurRound == 1)
	{
		switch (m_ScriptCount)
		{
		case 0:
			script = L"This is 'Thomas' from the \nmethadone clinic.";
			CFontMgr::GetInst()->SetStr(script);
			break;
		case 1:
			script = L"We 've scheduled a short meeting\n for you tonight.";
			CFontMgr::GetInst()->SetStr(script);
			break;
		case 2:
			script = L"We 're at NW 184th Street, apt 105.";
			CFontMgr::GetInst()->SetStr(script);
			break;
		case 3:
			CFontMgr::GetInst()->SetStr(script);
			DestroyObject(CLevelMgr::GetInst()->FindObjectByName(L"HighLightIngamePP"));
			m_ActiveScript = false;
			break;
		}
	}
	else if (CurRound == 2)
	{
		switch (m_ScriptCount)
		{
		case 0:
			script = L"We have a prank caller at the\n telephone company.";
			CFontMgr::GetInst()->SetStr(script);
			break;
		case 1:
			script = L"Why don't you go there and\n see if you can..";
			CFontMgr::GetInst()->SetStr(script);
			break;
		case 2:
			script = L"'Talk some sense' with him.\n it's 342nd NW St.";
			CFontMgr::GetInst()->SetStr(script);
			break;
		case 3:
			CFontMgr::GetInst()->SetStr(script);
			DestroyObject(CLevelMgr::GetInst()->FindObjectByName(L"HighLightIngamePP"));
			m_ActiveScript = false;
			break;
		}
	}
}
