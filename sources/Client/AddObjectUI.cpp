#include "pch.h"
#include "AddObjectUI.h"

#include "ListUI.h"
#include "CImGuiMgr.h"

#include <Engine/CAssetMgr.h>
#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CGameObject.h>

AddObjectUI::AddObjectUI()
	: EditorUI("AddObjectUI")
    , m_Object(nullptr)
    , m_Idx(0)
    , m_MoveWithChild(false)
{
}

AddObjectUI::~AddObjectUI()
{
}


void AddObjectUI::Render_Update()
{
    string ObjName;

    if (nullptr != m_Object)
        ObjName = string(m_Object->GetName().begin(), m_Object->GetName().end());


    if (ImGui::Button("Load Prefab"))
    {
        // ListUI 를 활성화 시키기
        ListUI* pListUI = (ListUI*)CImGuiMgr::GetInst()->FindUI("##ListUI");
        pListUI->SetName("Material");
        pListUI->SetActive(true);

        // ListUI 에 넣어줄 문자열 정보 가져오기
        pListUI->AddItem("None");

        vector<wstring> vecAssetNames;
        CAssetMgr::GetInst()->GetAssetNames(ASSET_TYPE::PREFAB, vecAssetNames);
        pListUI->AddItem(vecAssetNames);

        // 더블 클릭 시 호출시킬 함수 등록
        pListUI->AddDynamicDoubleCliked(this, (EUI_DELEGATE_2)&AddObjectUI::SelectPrefab);
    }

    ImGui::Text("Current Object");
    ImGui::SameLine(100);
    ImGui::SetNextItemWidth(150);
    ImGui::InputText("##ObjectName", (char*)ObjName.c_str(), ObjName.length(), ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);

    ImGui::Text("Layer Idx");
    ImGui::SameLine(100);
    ImGui::InputInt("##LayerIdx", &m_Idx);

    bool Move = m_MoveWithChild;

    ImGui::Text("MoveWithChild");
    ImGui::SameLine(100);
    ImGui::Checkbox("##MoveWithChild", &Move);
    m_MoveWithChild = Move;
    Move = m_MoveWithChild;

    if (ImGui::Button("Add"))
    {
        CreateObject(m_Object, m_Idx, m_MoveWithChild);
        m_Object = nullptr;
        //CLevelMgr::GetInst()->GetCurrentLevel()->AddObject(m_Idx, m_Object, m_MoveWithChild);
    }
}

void AddObjectUI::SelectPrefab(DWORD_PTR _ListUI, DWORD_PTR _SelectString)
{
    // 리스트에서 더블킬릭한 항목의 이름을 받아온다.
    ListUI* pListUI = (ListUI*)_ListUI;
    string* pStr = (string*)_SelectString;

    if (*pStr == "None")
    {
        return;
    }

    // 해당 항목 에셋을 찾아서, MeshRenderComponent 가 해당 메시를 참조하게 한다.
    Ptr<CPrefab> pPrefab = CAssetMgr::GetInst()->FindAsset<CPrefab>(wstring(pStr->begin(), pStr->end()));
    if (nullptr == pPrefab)
        return;

    m_Object = pPrefab->Instantiate();
}

