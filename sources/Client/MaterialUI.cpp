#include "pch.h"
#include "MaterialUI.h"

#include <Engine/CPathMgr.h>
#include <Engine/CAssetMgr.h>

#include "CImGuiMgr.h"
#include "ListUI.h"
#include "TreeUI.h"

MaterialUI::MaterialUI()
	: AssetUI("Material", ASSET_TYPE::MATERIAL)
	, m_Tex(nullptr)
	, m_Idx(0)
	, m_TexChange(true)
{
}

MaterialUI::~MaterialUI()
{
}

void MaterialUI::Render_Update()
{
	AssetTitle();

	Ptr<CMaterial> pMtrl = (CMaterial*)GetAsset().Get();
	Ptr<CGraphicShader> pShader = pMtrl->GetShader();

	if (m_TexChange)
	{
		m_Tex = pMtrl->GetParamTex(m_Idx);
		m_TexChange = false;
	}

	string ShaderName;
	string MtrlName;

	if (nullptr == pShader)
		ShaderName = "None";
	else
	{
		ShaderName = string(pShader->GetKey().begin(), pShader->GetKey().end());
	}

	MtrlName = string(pMtrl->GetKey().begin(), pMtrl->GetKey().end());


	ImGui::Text("Shader");
	ImGui::SameLine(100);
	ImGui::SetNextItemWidth(150);
	ImGui::InputText("##MeshName", (char*)ShaderName.c_str(), ShaderName.length(), ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);

	if (ImGui::BeginDragDropTarget())
	{
		if (ImGui::AcceptDragDropPayload("ContentUI"))
		{
			const ImGuiPayload* pPayload = ImGui::GetDragDropPayload();
			TreeNode* pNode = *((TreeNode**)pPayload->Data);
			Ptr<CAsset> pAsset = (CAsset*)pNode->GetData();
			if (pAsset->GetAssetType() == ASSET_TYPE::GRAPHIC_SHADER)
			{
				pMtrl->SetShader((CGraphicShader*)pAsset.Get());
				ImGui::SetWindowFocus(nullptr);
			}
		}

		ImGui::EndDragDropTarget();
		m_TexChange = true;
	}

	ImGui::SameLine();
	if (ImGui::Button("##ShaderBtn", ImVec2(18.f, 18.f)))
	{
		// ListUI 를 활성화 시키기
		ListUI* pListUI = (ListUI*)CImGuiMgr::GetInst()->FindUI("##ListUI");
		pListUI->SetName("GraphicShader");
		pListUI->SetActive(true);

		// ListUI 에 넣어줄 문자열 정보 가져오기
		pListUI->AddItem("None");

		vector<wstring> vecAssetNames;
		CAssetMgr::GetInst()->GetAssetNames(ASSET_TYPE::GRAPHIC_SHADER, vecAssetNames);
		pListUI->AddItem(vecAssetNames);

		// 더블 클릭 시 호출시킬 함수 등록
		pListUI->AddDynamicDoubleCliked(this, (EUI_DELEGATE_2)&MaterialUI::SelectGraphicShader);
	}

	// 현재 Material
	ImGui::Text("Current Material");
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
	ImGui::Text(MtrlName.c_str());
	ImGui::PopStyleColor();

	ImGui::Text("Input Name");
	ImGui::SameLine(150);
	ImGui::SetNextItemWidth(200);
	ImGui::InputText("##MtrlName", m_Buffer, IM_ARRAYSIZE(m_Buffer));


	// Texture
	ImGui::Text("Input Texture");


	ImVec2 uv_min = ImVec2(0.0f, 0.0f);
	ImVec2 uv_max = ImVec2(1.0f, 1.0f);
	ImVec4 tint_col = ImVec4(1.f, 1.f, 1.f, 1.f);
	ImVec4 border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);



	if (nullptr == m_Tex)
		ImGui::Image(nullptr, ImVec2(100.f, 100.f), uv_min, uv_max, tint_col, border_col);
	else
		ImGui::Image(m_Tex->GetSRV().Get(), ImVec2(100.f, 100.f), uv_min, uv_max, tint_col, border_col);

	if (ImGui::BeginDragDropTarget())
	{
		if (ImGui::AcceptDragDropPayload("ContentUI"))
		{
			const ImGuiPayload* pPayload = ImGui::GetDragDropPayload();
			TreeNode* pNode = *((TreeNode**)pPayload->Data);
			Ptr<CAsset> pAsset = (CAsset*)pNode->GetData();
			if (pAsset->GetAssetType() == ASSET_TYPE::TEXTURE)
			{
				m_Tex = ((CTexture*)pAsset.Get());
				ImGui::SetWindowFocus(nullptr);
			}
		}
		ImGui::EndDragDropTarget();
	}
	

	ImGui::Text("Input Parameter Idx");
	if (ImGui::InputInt("##ParamIdx", &m_Idx))
	{
		m_TexChange = true;
	}
	if (ImGui::Button("Bind"))
	{
		pMtrl->SetTexParam((TEX_PARAM)m_Idx, m_Tex);
	}
	

	// 재질을 파일로 저장하기
	if (ImGui::Button("SAVE"))
	{
		Ptr<CMaterial> pMtrl = (CMaterial*)GetAsset().Get();
		assert(pMtrl.Get());
		
		//wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + pMtrl->GetKey();
		//pMtrl->Save(strFilePath);
		string Temp = m_Buffer;
		wstring MtrlName(Temp.begin(), Temp.end());
		pMtrl->SetKey(MtrlName);
		wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + L"Material\\" + MtrlName + L".mtrl";
		pMtrl->Save(strFilePath);
	}
}


void MaterialUI::SelectGraphicShader(DWORD_PTR _ListUI, DWORD_PTR _SelectString)
{
	// 리스트에서 더블킬릭한 항목의 이름을 받아온다.
	ListUI* pListUI = (ListUI*)_ListUI;
	string* pStr = (string*)_SelectString;

	Ptr<CMaterial> pMtrl = (CMaterial*)GetAsset().Get();
	if (nullptr == pMtrl)
		return;

	if (*pStr == "None")
	{
		pMtrl->SetShader(nullptr);
		return;
	}

	// 해당 항목 에셋을 찾아서, MeshRenderComponent 가 해당 메시를 참조하게 한다.
	Ptr<CGraphicShader> pShader = CAssetMgr::GetInst()->FindAsset<CGraphicShader>(wstring(pStr->begin(), pStr->end()));
	if (nullptr == pShader)
		return;

	pMtrl->SetShader(pShader);
}