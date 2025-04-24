#include "pch.h"
#include "CEndingScript.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CAssetMgr.h>

#include "CLevelScript.h"

CEndingScript::CEndingScript()
	: CScript(SCRIPT_TYPE::ENDINGSCRIPT)
{
}

CEndingScript::~CEndingScript()
{
}

void CEndingScript::Begin()
{
	CGameObject* pPlayer = CLevelMgr::GetInst()->FindObjectByName(L"EndingPlayer");

	pPlayer->FlipbookPlayer()->Play(0, 4.f, true);

	m_BGM = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\bgm\\Ending.mp3", L"Sound\\bgm\\Ending.mp3");
	m_BGM->Play(0, 0.4f, false);
}

void CEndingScript::Tick()
{
	if (KEY_TAP(KEY::ENTER))
	{
		CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
		wstring ContentPath = CPathMgr::GetInst()->GetContentPath();
		m_BGM->Stop();

		ChangeLevelState(LEVEL_STATE::STOP);
		CLevel* Main = pCurLevel->g_LevelLoad(ContentPath + L"Level\\Main.lv");
		ChangeLevel(Main, LEVEL_STATE::PLAY);
	}
}

void CEndingScript::BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CEndingScript::Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CEndingScript::EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
}

void CEndingScript::SaveComponent(FILE* _File)
{
}

void CEndingScript::LoadComponent(FILE* _File)
{
}
