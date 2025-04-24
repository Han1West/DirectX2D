#include "pch.h"
#include "FlipbookEditor.h"

#include <Engine/CPathMgr.h>
#include <Engine/CAssetMgr.h>
#include <Engine/CTimeMgr.h>
#include <Engine/CSprite.h>
#include <Engine/CFlipbook.h>
#include <Engine/CTexture.h>


FlipbookEditor::FlipbookEditor()
	: EditorUI("FlipbookEditor")
	, m_Play(false)
	, m_Idx(1)
	, m_LoadedSprite(nullptr)
	, m_NewFlipbook(nullptr)
{
	m_LoadedSprite = new CSprite;
	m_NewFlipbook = new CFlipbook;
	strncpy_s(m_FLBuffer, "Empty Flipbook", sizeof(m_FLBuffer) - 1);
}

FlipbookEditor::~FlipbookEditor()
{
}

void FlipbookEditor::Render_Update()
{
	// 플립북에 추가할 스프라이트를 불러온다.
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

		memset(m_SPBuffer, 0, sizeof(m_SPBuffer));

		wstring RelativePath = SelectedSpritePath.substr(SelectedSpritePath.find(L"Sprite"));

		m_LoadedSprite = CAssetMgr::GetInst()->Load<CSprite>(RelativePath, RelativePath);
			//Load(SelectedSpritePath);
		m_CurrentTexture = m_LoadedSprite->GetAtlasTexture();

		size_t LastSlash = SelectedSpritePath.find_last_of(L"\\/");
		size_t LastDot = SelectedSpritePath.find_last_of(L".");
		wstring LoadedSpriteName = SelectedSpritePath.substr(LastSlash + 1, LastDot - LastSlash - 1);

		for (size_t i = 0; i < LoadedSpriteName.size() && i < sizeof(m_SPBuffer) - 1; ++i) {
			m_SPBuffer[i] = static_cast<char>(LoadedSpriteName[i]);
		}
	}

	ImGui::SameLine(100);

	// 저장된 플립북을 불러온다.
	if (ImGui::Button("Load Flipbook"))
	{
		wstring SelectedFlipbookPath = L"";
		OPENFILENAME ofn;
		wchar_t filePath[MAX_PATH] = L"";

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = filePath;
		ofn.nMaxFile = sizeof(filePath);
		ofn.lpstrFilter = L"Flipbook\0*.flip\0ALL\0*.*";
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
		strFilePath += L"Flipbook";
		ofn.lpstrInitialDir = strFilePath.c_str();

		if (GetOpenFileName(&ofn))
		{
			SelectedFlipbookPath = filePath; // 선택한 경로 저장
		}
		if (L"" == SelectedFlipbookPath)
			return;


		wstring RelativePath = SelectedFlipbookPath.substr(SelectedFlipbookPath.find(L"Flipbook"));

		m_NewFlipbook = CAssetMgr::GetInst()->Load<CFlipbook>(RelativePath, RelativePath);

		//m_NewFlipbook->Load(SelectedFlipbookPath);
		m_LoadedSprite = m_NewFlipbook->GetSprite(0);
		m_CurrentTexture = m_LoadedSprite->GetAtlasTexture();
		m_Idx = 1;

		memset(m_FLBuffer, 0, sizeof(m_FLBuffer));

		float width = m_CurrentTexture->GetWidth();
		float height = m_CurrentTexture->GetHeight();

		m_OffsetUV = Vec2(m_LoadedSprite->GetOffset().x * width, m_LoadedSprite->GetOffset().y * height);

		size_t LastSlash = SelectedFlipbookPath.find_last_of(L"\\/");
		size_t LastDot = SelectedFlipbookPath.find_last_of(L".");
		wstring LoadedFlipbookName = SelectedFlipbookPath.substr(LastSlash + 1, LastDot - LastSlash - 1);

		for (size_t i = 0; i < LoadedFlipbookName.size() && i < sizeof(m_FLBuffer) - 1; ++i) {
			m_FLBuffer[i] = static_cast<char>(LoadedFlipbookName[i]);
		}
	}
	
	ImGui::SameLine(220);
	
	// 불러온 Sprite들과 Flipbook을 모두 날려버린다.
	if (ImGui::Button("Clear"))
	{
		memset(m_SPBuffer, 0, sizeof(m_SPBuffer));
		strncpy_s(m_FLBuffer, "Empty Flipbook", sizeof(m_FLBuffer) - 1);
		m_NewFlipbook = new CFlipbook;
		m_Idx = 1;
		m_CurrentTexture = nullptr;
	}

	int Max_Idx = m_NewFlipbook->GetMaxSprite();

	// 현재 FlipbookName
	ImGui::Text("CurrentFlipbook");
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
	ImGui::Text(m_FLBuffer);
	ImGui::PopStyleColor();


	// 현재 Sprite 이미지를 출력해준다.
	ImGui::Text("CurrentSprite");
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
	ImGui::Text(m_SPBuffer);
	ImGui::PopStyleColor();

	ImVec2 imageSize(512, 512);  // 전체 이미지 크기
	ImVec2 uv_min(m_LoadedSprite->GetLeftTop().x - m_LoadedSprite->GetOffset().x - 0.01f
				, m_LoadedSprite->GetLeftTop().y - m_LoadedSprite->GetOffset().y - 0.02f); // 이미지 시작점 (0.0f ~ 1.0f)
	ImVec2 uv_max(m_LoadedSprite->GetLeftTop().x + m_LoadedSprite->GetSlice().x - m_LoadedSprite->GetOffset().x + 0.01f
		, m_LoadedSprite->GetLeftTop().y + m_LoadedSprite->GetSlice().y - m_LoadedSprite->GetOffset().y + 0.02f); // 이미지 끝점 (0.0f ~ 1.0f)
	ImVec4 tint_col(1.0f, 1.0f, 1.0f, 1.0f); // 색상 적용
	ImVec4 border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);
	if (nullptr != m_CurrentTexture.Get())
	{
		ImVec2 Pos = ImGui::GetCursorScreenPos();
		ImGui::Image((ImTextureID)m_CurrentTexture->GetSRV().Get(), imageSize, uv_min, uv_max, tint_col, border_col);

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
	
	// 현재 Sprite를 Flipbook에 추가한다.
	if (ImGui::Button("Add Sprite"))
	{
		Ptr<CSprite> NewSprite = new CSprite;
		//NewSprite = CAssetMgr::GetInst()->Load<CSprite>(m_LoadedSprite->GetKey(), m_LoadedSprite->GetRelativePath());
		
		NewSprite->CopySprite(m_LoadedSprite);
		CAssetMgr::GetInst()->AddAsset(m_LoadedSprite->GetRelativePath(), NewSprite);

		m_NewFlipbook->AddSprite(NewSprite);
		m_Idx = Max_Idx + 1;
	}

	ImGui::SameLine(100);

	// 마지막 Sprite를 Flipbook에서 삭제한다.
	if (ImGui::Button("Delete Sprite"))
	{
		if (m_Idx == Max_Idx)
			m_Idx -= 1;
		if (Max_Idx > 0)
			m_NewFlipbook->PopSprite();
		else
			return;
	}
		
	// 현재 Sprite의 Offset을 변경시킨다.
	ImGui::Text("OffsetUV");
	ImGui::InputFloat2("##OffsetUV", m_OffsetUV);
	
	Vec2 NewOffset = m_OffsetUV;


	if (ImGui::Button("Apply"))
	{
		if (Max_Idx > 1)
		{
			m_NewFlipbook->GetSprite(m_Idx - 1)->SetOffset(NewOffset);
			wstring RelativePath = m_NewFlipbook->GetSprite(m_Idx - 1)->GetRelativePath();
			wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + RelativePath;
			Ptr<CSprite> CurSprite = m_NewFlipbook->GetSprite(m_Idx - 1);

			CurSprite->Save(strFilePath);
		}

	}
		
	ImGui::Text("Current Idx : %d", m_Idx);
	ImGui::SameLine(150);
	ImGui::Text("Max Idx : %d", Max_Idx);
	
	//wstring CurSprite = m_LoadedSprite->GetRelativePath();
	//ImGui::Text("Current Sprite: %s", CurSprite.c_str());

	// Index값을 바꿔 해당하는 Index의 Sprite를 출력하도록 한다.
	ImGui::Text("Input Index");
	if (ImGui::InputInt("##Index", &m_Idx))
	{
		if (m_Idx < 1)
			m_Idx = Max_Idx;
		if (m_Idx > Max_Idx)
			m_Idx = 1;

		if (Max_Idx > 1)
		{
			m_LoadedSprite = m_NewFlipbook->GetSprite(m_Idx - 1);
			m_CurrentTexture = m_LoadedSprite->GetAtlasTexture();
			float Width = m_CurrentTexture->GetWidth();
			float height = m_CurrentTexture->GetHeight();

			m_OffsetUV = Vec2(m_LoadedSprite->GetOffset().x * Width, m_LoadedSprite->GetOffset().y * height);
			
			wstring LoadedSpriteName = m_LoadedSprite->GetKey();
			for (size_t i = 0; i < LoadedSpriteName.size() && i < sizeof(m_SPBuffer) - 1; ++i) {
				m_SPBuffer[i] = static_cast<char>(LoadedSpriteName[i]);
			}
			
		}
	}

	// 재생시킨다.
	if (ImGui::Button("Play"))
	{
		if (Max_Idx > 1)
		{
			m_Play = true;
			m_Idx = 1;
		}
	}

	if (m_Play == true)
	{
		m_Time += EngineDT;
		if (m_Time > 0.2f)
		{
			m_Idx += 1;
			m_Time = 0.f;
		}
			
		if (m_Idx > Max_Idx)
		{
			m_Play = false;
			m_Idx = 1;
		}
		else
		{
			m_LoadedSprite = m_NewFlipbook->GetSprite(m_Idx - 1);
			m_CurrentTexture = m_LoadedSprite->GetAtlasTexture();
		}
		wstring LoadedSpriteName = m_LoadedSprite->GetKey();
		for (size_t i = 0; i < LoadedSpriteName.size() && i < sizeof(m_SPBuffer) - 1; ++i) {
			m_SPBuffer[i] = static_cast<char>(LoadedSpriteName[i]);
		}
	}

	// 이름을 지정한다.
	//char inputBuffer[128] = "";
	//string Temp = "";
	//ImGui::Text("FlipbookName");
	//ImGui::InputText("##FlipbookName", m_Buffer, IM_ARRAYSIZE(inputBuffer));

	// 생성한 플립북 파일로 저장하기
	if (ImGui::Button("SAVE"))
	{
		// 파일 경로 문자열
		wchar_t szFilePath[255] = {};

		OPENFILENAME Desc = {};

		Desc.lStructSize = sizeof(OPENFILENAME);
		Desc.hwndOwner = nullptr;
		Desc.lpstrFile = szFilePath;
		Desc.nMaxFile = 255;
		Desc.lpstrFilter = L"Flipbook\0*.flip\0ALL\0*.*";
		Desc.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
		strFilePath += L"Flipbook";
		Desc.lpstrInitialDir = strFilePath.c_str();

		if (GetSaveFileName(&Desc))
		{
			if (wcslen(szFilePath) != 0)
			{
				m_NewFlipbook->Save(szFilePath);
			}
		}

		/*Temp = m_Buffer;
		wstring m_FlipbookName(Temp.begin(), Temp.end());
		wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + L"Flipbook\\" + m_FlipbookName + L".flip";
		m_NewFlipbook->Save(strFilePath);*/
	}
}