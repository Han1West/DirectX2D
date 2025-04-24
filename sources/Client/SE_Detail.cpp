#include "pch.h"
#include "SE_Detail.h"

#include "SE_AtlasView.h"
#include "TreeUI.h"
#include "ListUI.h"
#include "CImGuiMgr.h"

#include <Engine/CAssetMgr.h>

SE_Detail::SE_Detail()
	: SE_Sub("SE_Detail")
{
	m_NewSprite = new CSprite;
}

SE_Detail::~SE_Detail()
{
}

void SE_Detail::SetAtlasTex(Ptr<CTexture> _Tex)
{
	m_AtlasTex = _Tex;

	GetAtlasView()->SetAtlasTex(m_AtlasTex);
}


void SE_Detail::Render_Update()
{

	SpriteInfo();
	
}

void SE_Detail::Atlas()
{
	string TexName;

	if (nullptr != m_AtlasTex)
		TexName = string(m_AtlasTex->GetKey().begin(), m_AtlasTex->GetKey().end());

	ImGui::Text("Atlas Texture");
	ImGui::SameLine(120);
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("##AtlasTex", (char*)TexName.c_str(), ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);

	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ContentUI");
		if (payload)
		{
			const ImGuiPayload* pPayload = ImGui::GetDragDropPayload();
			TreeNode* pNode = *((TreeNode**)pPayload->Data);
			Ptr<CAsset> pAsset = (CAsset*)pNode->GetData();

			if (pAsset->GetAssetType() == ASSET_TYPE::TEXTURE)
			{
				SetAtlasTex((CTexture*)pAsset.Get());
			}
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();
	if (ImGui::Button("##AtlasTexBtn", ImVec2(18.f, 18.f)))
	{
		ListUI* pListUI = (ListUI*)CImGuiMgr::GetInst()->FindUI("##ListUI");
		pListUI->SetName("Texture");
		vector<wstring> vecTexNames;
		CAssetMgr::GetInst()->GetAssetNames(ASSET_TYPE::TEXTURE, vecTexNames);
		pListUI->AddItem(vecTexNames);
		pListUI->AddDynamicDoubleCliked(this, (EUI_DELEGATE_2)&SE_Detail::SelectTexture);
		pListUI->SetActive(true);
	}
}

void SE_Detail::AtlasInfo()
{
	// 해상도
	UINT width = 0;
	UINT height = 0;

	if (nullptr != m_AtlasTex)
	{
		width = m_AtlasTex->GetWidth();
		height = m_AtlasTex->GetHeight();
	}

	char buff[50] = {};
	sprintf_s(buff, "%d", width);

	ImGui::Text("Width");
	ImGui::SameLine(100);
	ImGui::InputText("##TextureWidth", buff, 50, ImGuiInputTextFlags_ReadOnly);

	sprintf_s(buff, "%d", height);
	ImGui::Text("Height");
	ImGui::SameLine(100);
	ImGui::InputText("##TextureHeight", buff, 50, ImGuiInputTextFlags_ReadOnly);
}

void SE_Detail::SpriteInfo()
{
   //파일을 열어 현재 선택한 텍스쳐의 경로를 가져온다.
   if(ImGui::Button("Load Texture"))
   {
   		OPENFILENAME ofn;
   		wchar_t filePath[MAX_PATH] = L"";

   		ZeroMemory(&ofn, sizeof(ofn));
   		ofn.lStructSize = sizeof(ofn);
   		ofn.hwndOwner = NULL; 
   		ofn.lpstrFile = filePath;
   		ofn.nMaxFile = sizeof(filePath);
   		ofn.lpstrFilter = (LPWSTR)"모든 파일\0*.*\0텍스트 파일\0*.TXT\0";
   		ofn.nFilterIndex = 1;
   		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

   		wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
   		strFilePath += L"Texture";
   		ofn.lpstrInitialDir = strFilePath.c_str();

   		if (GetOpenFileName(&ofn)) 
   		{
   			m_SelectedFilePath = filePath; // 선택한 경로 저장
   		}
		if (L"" == m_SelectedFilePath)
			return;
   		
   		m_LeftTop = Vec2(0.f, 0.f);
   		m_Slice = Vec2(0.f, 0.f);
   		m_Background = Vec2(200.f, 200.f);
   		m_Offset = Vec2(0.f, 0.f);

		wstring RelativePath = m_SelectedFilePath.substr(m_SelectedFilePath.find(L"Texture"));

		SetAtlasTex(CAssetMgr::GetInst()->Load<CTexture>(RelativePath, RelativePath));
   		memset(m_Buffer, 0, sizeof(m_Buffer));
		strncpy_s(m_Buffer, "New Sprite", sizeof(m_Buffer) - 1);

   		m_NewSprite = new CSprite;
   }

   // 파일을 열어 저장된 스프라이트를 불러온다.
   if (ImGui::Button("Load Sprite"))
   {
   		wstring SelectedSpritePath = L"";
   		OPENFILENAME ofn;
   		wchar_t filePath[MAX_PATH] = L"";

   		ZeroMemory(&ofn, sizeof(ofn));
   		ofn.lStructSize = sizeof(ofn);
   		ofn.hwndOwner = NULL;
   		ofn.lpstrFile = filePath;
   		ofn.nMaxFile = sizeof(filePath);
   		ofn.lpstrFilter = L"Sprite\0*.sprite\0ALL\0*.*";
   		ofn.nFilterIndex = 1;
   		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

   		wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
   		strFilePath += L"Sprite";
   		ofn.lpstrInitialDir = strFilePath.c_str();

   		if (GetOpenFileName(&ofn))
   		{
   			SelectedSpritePath = filePath; // 선택한 경로 저장
   		}
   		if (L"" == SelectedSpritePath)
   			return;

   		wstring RelativePath = SelectedSpritePath.substr(SelectedSpritePath.find(L"Sprite"));

   		m_NewSprite = CAssetMgr::GetInst()->Load<CSprite>(RelativePath, RelativePath);

   		//m_NewSprite->Load(SelectedSpritePath);
   		m_AtlasTex = m_NewSprite->GetAtlasTexture();
		SetAtlasTex(m_AtlasTex);

   		float width = m_AtlasTex->GetWidth();
   		float height = m_AtlasTex->GetHeight();


   		memset(m_Buffer, 0, sizeof(m_Buffer));
   		
   		// 선택한 스프라이트의 변수값들을 받아온다.
   		m_LeftTop		= Vec2(m_NewSprite->GetLeftTop().x * width, m_NewSprite->GetLeftTop().y * height);
   		m_Slice			= Vec2(m_NewSprite->GetSlice().x * width, m_NewSprite->GetSlice().y * height);
   		m_Background	= Vec2(m_NewSprite->GetBackground().x * width, m_NewSprite->GetBackground().y * height);
   		m_Offset		= Vec2(m_NewSprite->GetOffset().x * width, m_NewSprite->GetOffset().y * height);

		GetAtlasView()->SetRectPos(m_LeftTop, m_Slice);

		// Sprite의 이름을 받아온다.
   		size_t LastSlash = SelectedSpritePath.find_last_of(L"\\/");
   		size_t LastDot = SelectedSpritePath.find_last_of(L".");
   		wstring LoadedSpriteName = SelectedSpritePath.substr(LastSlash + 1, LastDot - LastSlash - 1);

   		for (size_t i = 0; i < LoadedSpriteName.size() && i < sizeof(m_Buffer) - 1; ++i) {
   			m_Buffer[i] = static_cast<char>(LoadedSpriteName[i]);
   		}
   }

   Atlas();
   
   AtlasInfo();



   // 현재 선택한 영역의 텍스쳐를 보여준다.

   ImGui::Text("CurrentSprite");

   // 스프라이트의 이름 출력
   ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); 
   ImGui::Text(m_Buffer);
   ImGui::PopStyleColor();

   ImVec2 imageSize(512, 512);  // 전체 이미지 크기
   ImVec2 uv_min(m_NewSprite->GetLeftTop().x - m_NewSprite->GetOffset().x  - 0.01f, m_NewSprite->GetLeftTop().y - m_NewSprite->GetOffset().y - 0.02f); // 이미지 시작점 (0.0f ~ 1.0f)
   ImVec2 uv_max(m_NewSprite->GetLeftTop().x+ m_NewSprite->GetSlice().x - m_NewSprite->GetOffset().x + 0.01f
   	, m_NewSprite->GetLeftTop().y + m_NewSprite->GetSlice().y - m_NewSprite->GetOffset().y + 0.02f); // 이미지 끝점 (0.0f ~ 1.0f)
   ImVec4 tint_col(1.0f, 1.0f, 1.0f, 1.0f); // 색상 적용
   ImVec4 border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);
   if (nullptr != m_AtlasTex.Get())
   {
		ImVec2 Pos = ImGui::GetCursorScreenPos();
   		ImGui::Image((ImTextureID)m_AtlasTex->GetSRV().Get(), imageSize, uv_min, uv_max, tint_col, border_col);

		// 이미지에 십자선 그리기
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		// 이미지 중심 계산
		ImVec2 center(Pos.x + imageSize.x * 0.5f, Pos.y + imageSize.y * 0.5f);

		// 선의 색상 및 두께
		ImU32 lineColor = IM_COL32(255, 0, 0, 255); // 빨간색
		float lineThickness = 1.5f;

		// 십자선 그리기
		drawList->AddLine(ImVec2(center.x, Pos.y), ImVec2(center.x, Pos.y + imageSize.y), lineColor, lineThickness); // 세로선
		drawList->AddLine(ImVec2(Pos.x, center.y), ImVec2(Pos.x + imageSize.x, center.y), lineColor, lineThickness); // 가로선
   }
   
   // 4개의 변수 값을 설정
   ImGui::Text("LeftTop");
   ImGui::SameLine(140);
   ImGui::InputFloat2("##LeftTop", m_LeftTop);


   ImGui::Text("Slice");
   ImGui::SameLine(140);
   ImGui::InputFloat2("##Slice", m_Slice);

   ImGui::Text("Background");
   ImGui::SameLine(140);
   ImGui::InputFloat2("##Background", m_Background);

   ImGui::Text("Offset");
   ImGui::SameLine(140);
   ImGui::InputFloat2("##Offset", m_Offset);


   Vec2 NewLeftTop = m_LeftTop;
   Vec2 NewSlice = m_Slice;
   Vec2 NewBackground = m_Background;
   Vec2 NewOffset = m_Offset;

   // 변수 세팅값 저장하기
   if (ImGui::Button("APPLY"))
   {
	   ApplySetting(m_AtlasTex, NewLeftTop, NewSlice, NewBackground, NewOffset);
   }


   // 생성한 스프라이트 파일로 저장하기
   if (ImGui::Button("SAVE"))
   {
	   //wstring strContentPath = CPathMgr::GetInst()->GetContentPath();

	   // 파일 경로 문자열
	   wchar_t szFilePath[255] = {};

	   OPENFILENAME Desc = {};

	   Desc.lStructSize = sizeof(OPENFILENAME);
	   Desc.hwndOwner = nullptr;
	   Desc.lpstrFile = szFilePath;
	   Desc.nMaxFile = 255;
	   Desc.lpstrFilter = L"Sprite\0*.sprite\0ALL\0*.*";
	   Desc.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	   wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	   strFilePath += L"Sprite";
	   Desc.lpstrInitialDir = strFilePath.c_str();

	   if (GetSaveFileName(&Desc))
	   {
		   if (wcslen(szFilePath) != 0)
		   {
			   m_NewSprite->Save(szFilePath);
		   }
	   }

		//Temp = m_Buffer;
		//wstring m_SpriteName(Temp.begin(), Temp.end());
		//wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + L"Sprite\\" + m_SpriteName + L".sprite";
		//m_NewSprite->Save(strFilePath);
		//CAssetMgr::GetInst()->AddAsset(strFilePath, m_NewSprite);
   }
}

void SE_Detail::SelectTexture(DWORD_PTR _ListUI, DWORD_PTR _SelectString)
{
	// 리스트에서 더블킬릭한 항목의 이름을 받아온다.
	ListUI* pListUI = (ListUI*)_ListUI;
	string* pStr = (string*)_SelectString;

	if (*pStr == "None")
	{
		SetAtlasTex(nullptr);
		return;
	}

	// 해당 항목 에셋을 찾아서, MeshRenderComponent 가 해당 메시를 참조하게 한다.
	Ptr<CTexture> pTex = CAssetMgr::GetInst()->FindAsset<CTexture>(wstring(pStr->begin(), pStr->end()));
	if (nullptr == pTex)
		return;

	SetAtlasTex(pTex);
}

void SE_Detail::SetSpriteInfo(Vec2 _LeftTop, Vec2 _RightBottom)
{
	m_LeftTop = _LeftTop;
	m_Slice = _RightBottom - _LeftTop;
	ApplySetting(m_AtlasTex, m_LeftTop, m_Slice, Vec2(100.f, 100.f), Vec2(0.f, 0.f));
}


void SE_Detail::ApplySetting(Ptr<CTexture> _Tex, Vec2 _LeftTop, Vec2 _Slice, Vec2 _Background, Vec2 _Offset)
{
	m_NewSprite->SetAtlasTexture(_Tex);
	m_NewSprite->SetLeftTop(_LeftTop);
	m_NewSprite->SetSlice(_Slice);
	m_NewSprite->SetBackground(_Background);
	m_NewSprite->SetOffset(_Offset);

	GetAtlasView()->SetRectPos(_LeftTop, _Slice);
}
