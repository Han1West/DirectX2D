#include "pch.h"
#include "PrefabEditor.h"

#include "CImGuiMgr.h"
#include "ListUI.h"
#include "Inspector.h"
#include "PF_View.h"

#include <Engine/CPathMgr.h>
#include <Engine/CAssetMgr.h>
#include <Engine/CGameObject.h>
#include <Engine/components.h>

#include <Scripts/CScriptMgr.h>

PrefabEditor::PrefabEditor()
	: EditorUI("PrefabEditor")
	, m_Prefab(nullptr)
	, m_ProtoObject(nullptr)
	, m_Buffer()
{
}

PrefabEditor::~PrefabEditor()
{
	
}

void PrefabEditor::Init()
{
	m_View = (PF_View*)CImGuiMgr::GetInst()->FindUI("PF_View");

	m_View->m_Owner = this;
}



void PrefabEditor::Render_Update()
{
	m_View->SetActive(IsActive());

	// 저장된 프리팹을 불러온다
	if (ImGui::Button("Load Prefab"))
	{
		wstring SelectedSpritePath = L"";
		OPENFILENAME ofn;
		wchar_t filePath[MAX_PATH] = L"";

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = filePath;
		ofn.nMaxFile = sizeof(filePath);
		ofn.lpstrFilter = L"Prefab\0*.pref\0ALL\0*.*";
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
		strFilePath += L"Prefab";
		ofn.lpstrInitialDir = strFilePath.c_str();

		if (GetOpenFileName(&ofn))
		{
			SelectedSpritePath = filePath; // 선택한 경로 저장
		}
		if (L"" == SelectedSpritePath)
			return;

		memset(m_Buffer, 0, sizeof(m_Buffer));

		wstring RelativePath = SelectedSpritePath.substr(SelectedSpritePath.find(L"Prefab"));

		m_Prefab = CAssetMgr::GetInst()->Load<CPrefab>(RelativePath, RelativePath);
		m_ProtoObject = m_Prefab->GetProtoObject();
		string ObjectName(m_ProtoObject->GetName().begin(), m_ProtoObject->GetName().end());

		strncpy_s(m_Buffer, ObjectName.c_str(), sizeof(m_Buffer) - 1);
	}

	// 새로운 오브젝트 생성
	if (ImGui::Button("Create Empty Object"))
	{
		m_ProtoObject = new CGameObject;
		m_Prefab = new CPrefab;
		strncpy_s(m_Buffer, "New Object", sizeof(m_Buffer) - 1);
	}

	// Object 이름 설정
	char inputBuffer[128] = "";
	string Temp = "";
	ImGui::Text("ObjectName");
	ImGui::InputText("##ObjectName", m_Buffer, IM_ARRAYSIZE(inputBuffer));
	if (ImGui::Button("Change Name"))
	{
		if (nullptr == m_ProtoObject)
			return;

		Temp = m_Buffer;
		wstring NewObjectName(Temp.begin(), Temp.end());
		m_ProtoObject->SetName(NewObjectName);
	}

	//if (nullptr != m_ProtoObject->MeshRender())
	//{

	//	GetView()->SetAtlasTex();
	//}

	if (ImGui::Button("Add Component"))
	{
		if (nullptr == m_ProtoObject)
			return;

		// ListUI 를 활성화 시키기
		ListUI* pListUI = (ListUI*)CImGuiMgr::GetInst()->FindUI("##ListUI");
		pListUI->SetName("Components");
		pListUI->SetActive(true);

		// ListUI 에 넣어줄 문자열 정보 가져오기
		pListUI->AddItem("None");

		vector<wstring> vecComponentsNames;
		vector<wstring> vecScirptsNames;
		GetComponentsNames(vecComponentsNames);
		CScriptMgr::GetScriptInfo(vecScirptsNames);
		vecComponentsNames.insert(vecComponentsNames.end(), vecScirptsNames.begin(), vecScirptsNames.end());

		pListUI->AddItem(vecComponentsNames);

		// 더블 클릭 시 호출시킬 함수 등록
		{
			pListUI->AddDynamicDoubleCliked(this, (EUI_DELEGATE_2)&PrefabEditor::SelectComponent);
		}
	}
	if (ImGui::Button("Delete Component"))
	{
		if (nullptr == m_ProtoObject)
			return;

		// ListUI 를 활성화 시키기
		ListUI* pListUI = (ListUI*)CImGuiMgr::GetInst()->FindUI("##ListUI");
		pListUI->SetName("Components");
		pListUI->SetActive(true);

		// ListUI 에 넣어줄 문자열 정보 가져오기
		pListUI->AddItem("None");

		vector<wstring> vecComponentsNames;
		vector<wstring> vecScirptsNames;
		GetComponentsNames(vecComponentsNames);
		CScriptMgr::GetScriptInfo(vecScirptsNames);
		vecComponentsNames.insert(vecComponentsNames.end(), vecScirptsNames.begin(), vecScirptsNames.end());

		pListUI->AddItem(vecComponentsNames);

		// 더블 클릭 시 호출시킬 함수 등록
		{
			pListUI->AddDynamicDoubleCliked(this, (EUI_DELEGATE_2)&PrefabEditor::DeleteComponent);
		}
	}

	if (ImGui::Button("Edit Component"))
	{
		if (nullptr == m_ProtoObject)
			return;

		Inspector* pInspector = (Inspector*)CImGuiMgr::GetInst()->FindUI("Inspector");
		pInspector->SetTargetObject(m_ProtoObject);
	}


	if (ImGui::Button("SAVE"))
	{
		if(m_Prefab->GetProtoObject() == nullptr)
			m_Prefab->SetProtoObject(m_ProtoObject);

		// 파일 경로 문자열
		wchar_t szFilePath[255] = {};

		OPENFILENAME Desc = {};

		Desc.lStructSize = sizeof(OPENFILENAME);
		Desc.hwndOwner = nullptr;
		Desc.lpstrFile = szFilePath;
		Desc.nMaxFile = 255;
		Desc.lpstrFilter = L"Prefab\0*.pref\0ALL\0*.*";
		Desc.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
		strFilePath += L"Prefab";
		Desc.lpstrInitialDir = strFilePath.c_str();

		if (GetSaveFileName(&Desc))
		{
			if (wcslen(szFilePath) != 0)
			{
				m_Prefab->Save(szFilePath);
			}
		}
	}
}


void PrefabEditor::IsRender()
{
	
}

void PrefabEditor::SelectComponent(DWORD_PTR _ListUI, DWORD_PTR _SelectString)
{
	// 리스트에서 더블클릭한 항목의 이름을 받아온다.
	ListUI* pListUI = (ListUI*)_ListUI;
	string* pStr = (string*)_SelectString;

	if (*pStr == "None")
	{
		return;
	}

	// 해당 컴퍼넌트를 ProtoObj에 추가한다.
	if (*pStr == "Transform")
	{
		m_ProtoObject->AddComponent(new CTransform);
		return;
	}
	if (*pStr == "Collider2D")
	{
		m_ProtoObject->AddComponent(new CCollider2D);
		return;
	}
	if (*pStr == "FlipbookPlayer")
	{
		m_ProtoObject->AddComponent(new CFlipbookPlayer);
		return;
	}
	if (*pStr == "Camera")
	{
		m_ProtoObject->AddComponent(new CCamera);
		return;
	}
	if (*pStr == "Light2D")
	{
		m_ProtoObject->AddComponent(new CLight2D);
		return;
	}
	//if (*pStr == "StateMachine")
	//{
	//	m_ProtoObject->AddComponent(new CSta);
	//}
	if (*pStr == "MeshRender")
	{
		m_ProtoObject->AddComponent(new CMeshRender);
		return;
	}
	if (*pStr == "Tilemap")
	{
		m_ProtoObject->AddComponent(new CTileMap);
		return;
	}
	if (*pStr == "ParticleSystem")
	{
		m_ProtoObject->AddComponent(new CParticleSystem);
		return;
	}
	
	// Scirpt를 추가하는 경우
	wstring pScriptStr = wstring(pStr->begin(), pStr->end());
	m_ProtoObject->AddComponent(CScriptMgr::GetScript(pScriptStr));
}

void PrefabEditor::DeleteComponent(DWORD_PTR _ListUI, DWORD_PTR _SelectString)
{
	// 리스트에서 더블클릭한 항목의 이름을 받아온다.
	ListUI* pListUI = (ListUI*)_ListUI;
	string* pStr = (string*)_SelectString;

	if (*pStr == "None")
	{
		return;
	}

	// 해당 컴퍼넌트를 ProtoObj에 추가한다.
	if (*pStr == "Transform")
	{
		m_ProtoObject->DeleteComponent(COMPONENT_TYPE::TRANSFORM);
		return;
	}
	if (*pStr == "Collider2D")
	{
		m_ProtoObject->DeleteComponent(COMPONENT_TYPE::COLLIDER2D); 
		return;
	}
	if (*pStr == "FlipbookPlayer")
	{
		m_ProtoObject->DeleteComponent(COMPONENT_TYPE::FLIPBOOKPLAYER);
		return;
	}
	if (*pStr == "Camera")
	{
		m_ProtoObject->DeleteComponent(COMPONENT_TYPE::CAMERA);
		return;
	}
	if (*pStr == "Light2D")
	{
		m_ProtoObject->DeleteComponent(COMPONENT_TYPE::LIGHT2D);
		return;
	}
	//if (*pStr == "StateMachine")
	//{
	//	m_ProtoObject->AddComponent(new CSta);
	//}
	if (*pStr == "MeshRender")
	{
		m_ProtoObject->DeleteComponent(COMPONENT_TYPE::MESHRENDER);
		return;
	}
	if (*pStr == "Tilemap")
	{
		m_ProtoObject->DeleteComponent(COMPONENT_TYPE::TILEMAP);
		return;
	}
	if (*pStr == "ParticleSystem")
	{
		m_ProtoObject->DeleteComponent(COMPONENT_TYPE::PARTICLE_SYSTEM);
		return;
	}

	// Scirpt를 추가하는 경우
	wstring pScriptStr = wstring(pStr->begin(), pStr->end());
	m_ProtoObject->DeleteScirpt(pScriptStr);
}
