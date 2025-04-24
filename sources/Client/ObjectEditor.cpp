#include "pch.h"
#include "ObjectEditor.h"

#include "CImGuiMgr.h"
#include "ListUI.h"
#include "Inspector.h"
#include "PrefabEditor.h"

#include <Engine/CPathMgr.h>
#include <Engine/CAssetMgr.h>
#include <Engine/CGameObject.h>
#include <Engine/components.h>

#include <Scripts/CScriptMgr.h>

ObjectEditor::ObjectEditor()
	:EditorUI("ObjectEditor")
{
}

ObjectEditor::~ObjectEditor()
{
}

void ObjectEditor::Render_Update()
{
	// ����� �������� �ҷ��´�
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
			SelectedSpritePath = filePath; // ������ ��� ����
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

	if (ImGui::Button("Create Empty Object"))
	{
		m_ProtoObject = new CGameObject;
		m_Prefab = new CPrefab;
		strncpy_s(m_Buffer, "New Object", sizeof(m_Buffer) - 1);
	}

	// Object �̸� ����
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


	if (ImGui::Button("Add Component"))
	{
		if (nullptr == m_ProtoObject)
			return;

		// ListUI �� Ȱ��ȭ ��Ű��
		ListUI* pListUI = (ListUI*)CImGuiMgr::GetInst()->FindUI("##ListUI");
		pListUI->SetName("Components");
		pListUI->SetActive(true);

		// ListUI �� �־��� ���ڿ� ���� ��������
		pListUI->AddItem("None");

		vector<wstring> vecComponentsNames;
		vector<wstring> vecScirptsNames;
		GetComponentsNames(vecComponentsNames);
		CScriptMgr::GetScriptInfo(vecScirptsNames);
		vecComponentsNames.insert(vecComponentsNames.end(), vecScirptsNames.begin(), vecScirptsNames.end());

		pListUI->AddItem(vecComponentsNames);

		// ���� Ŭ�� �� ȣ���ų �Լ� ���
		{
			pListUI->AddDynamicDoubleCliked(this, (EUI_DELEGATE_2)&PrefabEditor::SelectComponent);
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
		m_Prefab->SetProtoObject(m_ProtoObject);

		// ���� ��� ���ڿ�
		wchar_t szFilePath[255] = {};

		OPENFILENAME Desc = {};

		Desc.lStructSize = sizeof(OPENFILENAME);
		Desc.hwndOwner = nullptr;
		Desc.lpstrFile = szFilePath;
		Desc.nMaxFile = 255;
		Desc.lpstrFilter = L"Object\0*.obj\0ALL\0*.*";
		Desc.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
		strFilePath += L"Object";
		Desc.lpstrInitialDir = strFilePath.c_str();

		if (GetSaveFileName(&Desc))
		{
			if (wcslen(szFilePath) != 0)
			{
				
			}
		}
	}

}

