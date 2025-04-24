#include "pch.h"
#include "FlipbookPlayerUI.h"

#include <Engine/CFlipbookPlayer.h>
#include <Engine/CFlipbook.h>
#include <Engine/CAssetMgr.h>

#include "CImGuiMgr.h"
#include "TreeUI.h"
#include "ListUI.h"

FlipbookPlayerUI::FlipbookPlayerUI()
	:ComponentUI("FlipbookPlayer", COMPONENT_TYPE::FLIPBOOKPLAYER)
	, m_FlipbookID(0)
	, m_FlipbookChanged(true)
{
}

FlipbookPlayerUI::~FlipbookPlayerUI()
{
}

void FlipbookPlayerUI::Render_Update()
{
	ComponentTitle("FlipbookPlayer");


	CFlipbookPlayer* pFlipbookPlayer = GetTargetObject()->FlipbookPlayer();


	vector<Ptr<CFlipbook>> vecFlipbook = pFlipbookPlayer->GetvecFlipbook();


	Ptr<CFlipbook> PlayingFlipbook = pFlipbookPlayer->GetCurFlipbook();
	string FlipbookName;
	wstring wFlipbookKey;
	if(PlayingFlipbook != nullptr)
		wFlipbookKey = PlayingFlipbook->GetRelativePath();

	if(PlayingFlipbook != nullptr)
		FlipbookName = string(wFlipbookKey.begin(), wFlipbookKey.end());


	ImGui::Text("CurFlipbook");
	ImGui::SameLine(100);
	ImGui::SetNextItemWidth(400);
	ImGui::InputText("##CurFlipbook", (char*)FlipbookName.c_str(), FlipbookName.length(), ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);

	if (ImGui::BeginDragDropTarget())
	{
		if (ImGui::AcceptDragDropPayload("ContentUI"))
		{
			const ImGuiPayload* pPayload = ImGui::GetDragDropPayload();
			TreeNode* pNode = *((TreeNode**)pPayload->Data);
			Ptr<CFlipbook> pAsset = (CFlipbook*)pNode->GetData();
			if (pAsset->GetAssetType() == ASSET_TYPE::FLIPBOOK)
			{
				pFlipbookPlayer->SetCurFlipbook(pAsset);
				ImGui::SetWindowFocus(nullptr);
			}
		}

		ImGui::EndDragDropTarget();
	}
	ImGui::Text("");

	ImGui::SetNextItemWidth(50);
	ImGui::InputInt("##PlayIdx", &m_Idx, 0);
	ImGui::SameLine(60);

	if (ImGui::Button("Play"))
	{
		if (vecFlipbook[m_Idx] == nullptr)
			return;

		pFlipbookPlayer->SetCurFlipbook(vecFlipbook[m_Idx]);
		pFlipbookPlayer->Play(m_Idx, 10, false);
	}
	
	int FlipbookCount = 0;

	if (m_FlipbookChanged)
	{
		UpdateFlipbookName(vecFlipbook);
		m_FlipbookChanged = false;
	}
	
	ImGui::Text("Flipbook List");
	for (size_t i = 0; i < vecFlipbook.size(); ++i)
	{
		//Ptr<CFlipbook> CurFlipbook = vecFlipbook[i];

		//string FlipbookName = string(CurFlipbook->GetKey().begin(), CurFlipbook->GetKey().end());
		
        char szID[255] = {};
		string ID = std::to_string(m_FlipbookID++);
		string FullID = "##Flipbook" + ID;
		sprintf_s(szID, "Flipbook %d", FlipbookCount++);

		ImGui::Text(szID);
		ImGui::SameLine(100);
		ImGui::SetNextItemWidth(400);
		ImGui::InputText(FullID.c_str(), (char*)m_vecFlipNames[i].c_str(), m_vecFlipNames[i].length(), ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);
	}

	if (ImGui::Button("Add Flipbook"))
	{
		// ListUI 를 활성화 시키기
		ListUI* pListUI = (ListUI*)CImGuiMgr::GetInst()->FindUI("##ListUI");
		pListUI->SetName("Components");
		pListUI->SetActive(true);

		// ListUI 에 넣어줄 문자열 정보 가져오기
		pListUI->AddItem("None");

		vector<wstring> vecAssetNames;
		CAssetMgr::GetInst()->GetAssetNames(ASSET_TYPE::FLIPBOOK, vecAssetNames);
		pListUI->AddItem(vecAssetNames);

		// 더블 클릭 시 호출시킬 함수 등록
		{
			pListUI->AddDynamicDoubleCliked(this, (EUI_DELEGATE_2)&FlipbookPlayerUI::SelectFlipbook);
		}
	}

	if (ImGui::Button("Delete Flipbook"))
	{
		GetTargetObject()->FlipbookPlayer()->PopFlipbook();
		m_FlipbookChanged = true;
	}
}

void FlipbookPlayerUI::SelectFlipbook(DWORD_PTR _ListUI, DWORD_PTR _SelectString)
{
	m_FlipbookChanged = true;

	// 리스트에서 더블킬릭한 항목의 이름을 받아온다.
	ListUI* pListUI = (ListUI*)_ListUI;
	string* pStr = (string*)_SelectString;

	CFlipbookPlayer* pFlipbookPlayer = GetTargetObject()->FlipbookPlayer();
	vector<Ptr<CFlipbook>> vecFlipbook = pFlipbookPlayer->GetvecFlipbook();

	if (*pStr == "None")
	{
		return;
	}

	// 해당 항목 에셋을 찾아서, MeshRenderComponent 가 해당 메시를 참조하게 한다.
	Ptr<CFlipbook> pFlipbook = CAssetMgr::GetInst()->FindAsset<CFlipbook>(wstring(pStr->begin(), pStr->end()));
	if (nullptr == pFlipbook)
		return;

	GetTargetObject()->FlipbookPlayer()->AddFlipbook(vecFlipbook.size(), pFlipbook);
}

void FlipbookPlayerUI::UpdateFlipbookName(vector<Ptr<CFlipbook>> _CurFlipbook)
{
	m_vecFlipNames.clear();

	for (int i = 0; i < _CurFlipbook.size(); ++i)
	{
		Ptr<CFlipbook> CurFlipbook = _CurFlipbook[i];
		wstring Key = CurFlipbook->GetKey();
		m_vecFlipNames.push_back(string(Key.begin(), Key.end()));
	}
}

