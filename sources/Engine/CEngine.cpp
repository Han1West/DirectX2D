#include "pch.h"
#include "CEngine.h"

#include "CDevice.h"

#include "CPathMgr.h"
#include "CTimeMgr.h"
#include "CKeyMgr.h"
#include "CAssetMgr.h"
#include "CLevelMgr.h"
#include "CRenderMgr.h"
#include "CCollisionMgr.h"
#include "CTaskMgr.h"
#include "CFontMgr.h"
#include "CLevel.h"

CEngine::CEngine()
	: m_hMainWnd(nullptr)
{
}

CEngine::~CEngine()
{
}

int CEngine::Init(HWND _hWnd, UINT _Width, UINT _Height
		, GAMEOBJECT_SAVE _SaveFunc, GAMEOBJECT_LOAD _LoadFunc
		, LEVEL_SAVE _LevelSaveFunc, LEVEL_LOAD _LevelLoadFunc)
{
	m_hMainWnd = _hWnd;

	m_Resolution.x = (float)_Width;
	m_Resolution.y = (float)_Height;

	// GameObject Save Load 함수 포인터
	CPrefab::g_ObjectSave = _SaveFunc;
	CPrefab::g_ObjectLoad = _LoadFunc;
	CLevel::g_LevelSave = _LevelSaveFunc;
	CLevel::g_LevelLoad = _LevelLoadFunc;

	RECT rt = {0, 0, (int)m_Resolution.x , (int)m_Resolution.y };
	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, !!GetMenu(m_hMainWnd));
	SetWindowPos(m_hMainWnd, nullptr, 10, 10, rt.right - rt.left, rt.bottom - rt.top, 0);

	if (FAILED(CDevice::GetInst()->Init(m_hMainWnd, m_Resolution)))
	{
		return E_FAIL;
	}
	// FMOD 초기화		
	FMOD::System_Create(&m_FMODSystem);
	assert(m_FMODSystem);

	// 32개 채널 생성
	m_FMODSystem->init(32, FMOD_DEFAULT, nullptr);

	// Manager 초기화
	CPathMgr::GetInst()->Init();
	CKeyMgr::GetInst()->Init();
	CTimeMgr::GetInst()->Init();
	CAssetMgr::GetInst()->Init();
	CRenderMgr::GetInst()->Init();
	CLevelMgr::GetInst()->Init();
	CFontMgr::GetInst()->Init();

	return S_OK;
}

void CEngine::Progress()
{
	// FMOD Tick
	m_FMODSystem->update();

	// Engine Tick
	CKeyMgr::GetInst()->Tick();
	CTimeMgr::GetInst()->Tick();		
	CLevelMgr::GetInst()->Progress();
	CCollisionMgr::GetInst()->Tick();

	// Engine Render
	CRenderMgr::GetInst()->Render();
	CTimeMgr::GetInst()->Render();
	CFontMgr::GetInst()->Render();

	// Task
	CTaskMgr::GetInst()->Tick();
}
