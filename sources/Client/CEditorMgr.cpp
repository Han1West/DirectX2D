#include "pch.h"
#include "CEditorMgr.h"

#include <Engine/CGameObject.h>
#include <Engine/components.h>
#include <Engine/CRenderMgr.h>

#include "CGameObjectEx.h"
#include "CEditorCamScript.h"
#include "CPauseMenuScript.h"
#include "CPauseUIScript.h"

CEditorMgr::CEditorMgr()
{

}

CEditorMgr::~CEditorMgr()
{
	DeleteVec(m_vecEditorObj);
}

void CEditorMgr::Init()
{
	CGameObjectEx* pObject = new CGameObjectEx;
	pObject->SetName(L"EditorCamera");
	pObject->AddComponent(new CCamera);
	pObject->AddComponent(new CEditorCamScript);

	pObject->Transform()->SetRelativePos(0.f, 0.f, -200.f);
	pObject->Camera()->LayerCheckAll();
	pObject->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	pObject->Camera()->SetFar(100000.f);

	m_vecEditorObj.push_back(pObject);

	//pObject = new CGameObjectEx;
	//pObject->SetName(L"PauseMenu");
	//pObject->AddComponent(new CPauseMenuScript);

	//m_vecEditorObj.push_back(pObject);

	//pObject = new CGameObjectEx;
	//pObject->SetName(L"ContinueUI");
	//pObject->AddComponent(new CPauseUIScript);
	//pObject->AddComponent(new CMeshRender);

	//pObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	//pObject->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Material\\ContinueUIMtrl.mtrl"));

	//m_vecEditorObj.push_back(pObject);

	//pObject = new CGameObjectEx;
	//pObject->SetName(L"PauseRestartUI");
	//pObject->AddComponent(new CPauseUIScript);

	//pObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	//pObject->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Material\\PauseRestartUIMtrl.mtrl"));

	//m_vecEditorObj.push_back(pObject);

	//pObject = new CGameObjectEx;
	//pObject->SetName(L"QuitUI");
	//pObject->AddComponent(new CPauseUIScript);

	//pObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	//pObject->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Material\\QuitUIMtrl.mtrl"));

	//m_vecEditorObj.push_back(pObject);

	// Editor 용 카메라 등록
	CRenderMgr::GetInst()->RegisterEditorCamera(pObject->Camera());
}

void CEditorMgr::Progress()
{
	for (size_t i = 0; i < m_vecEditorObj.size(); ++i)
	{
		m_vecEditorObj[i]->Tick();
	}

	for (size_t i = 0; i < m_vecEditorObj.size(); ++i)
	{
		m_vecEditorObj[i]->FinalTick_Editor();
	}
}