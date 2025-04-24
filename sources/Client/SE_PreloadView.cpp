#include "pch.h"
#include "SE_PreloadView.h"

#include <Engine/CPathMgr.h>
#include <Engine/CTexture.h>
#include <Engine/CAssetMgr.h>

SE_PreloadView::SE_PreloadView()
	: EditorUI("SE_PreloadView")
	, m_First(true)
	, m_Owner(nullptr)
{
}

SE_PreloadView::~SE_PreloadView()
{
}

void SE_PreloadView::Render_Update()
{
	if (m_First)
	{
		wstring StageAssetPath = CPathMgr::GetInst()->GetContentPath();
		StageAssetPath += L"Texture\\stage\\";
		FindStageAsset(StageAssetPath);
		m_First = false;                           
	}

	string selectedAsset = string(m_SelectedAsset.begin(), m_SelectedAsset.end());

	ImGui::Text("Selected Asset");
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.9f, 0.4f, 1.0f));
	ImGui::Text(selectedAsset.c_str());
	ImGui::PopStyleColor();

	if (ImGui::Button("LoadToTile"))
	{
		Ptr<CTexture> pCurTex = CAssetMgr::GetInst()->Load<CTexture>(m_SelectedAsset, m_SelectedAsset);
		m_Owner->SetAtlas(pCurTex);
	}

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));	
	ImGui::Text("State Asset List");
	ImGui::PopStyleColor();


	ImVec2 scroll_size = ImVec2(0.0f, 0.0f);
	ImGui::BeginChild("Texture List", scroll_size, true, ImGuiWindowFlags_HorizontalScrollbar);
	int Count = 0;
	for (int i = 0; i < m_vecStageAsset.size(); ++i)
	{
		Ptr<CTexture> pCurTex = CAssetMgr::GetInst()->Load<CTexture>(m_vecStageAsset[i], m_vecStageAsset[i]);

		if (m_vecbSelected[i] == true)
		{			
			ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(255, 0, 0, 255));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 4.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
		}

		ImVec2 imageSize(pCurTex->GetWidth(), pCurTex->GetHeight());  // 전체 이미지 크기
		ImVec2 uv_min(0.f, 0.f); // 이미지 시작점 (0.0f ~ 1.0f)
		ImVec2 uv_max(1.f, 1.f); // 이미지 끝점 (0.0f ~ 1.0f)
		ImVec4 tint_col(1.0f, 1.0f, 1.0f, 1.0f); // 색상 적용
		ImVec4 border_col(0.43f, 0.43f, 0.50f, 0.50f); //= ImGui::GetStyleColorVec4(ImGuiCol_Border);

		if (ImGui::ImageButton(pCurTex->GetSRV().Get(), imageSize, uv_min, uv_max, 1, border_col, tint_col))
		{
			m_SelectedAsset = m_vecStageAsset[i];
		}
		
		if (m_vecbSelected[i] == true)
		{
			ImGui::PopStyleVar(2);
			ImGui::PopStyleColor();
		}

		if (m_SelectedAsset == m_vecStageAsset[i])
		{
			m_vecbSelected[i] = true;
		}
		else
		{
			m_vecbSelected[i] = false;
		}


		Count++;
		if (Count < 3)
			ImGui::SameLine();
		else
			Count = 0;
	}
	ImGui::EndChild();

}

void SE_PreloadView::FindStageAsset(const wstring& _FolderPath)
{
	wstring Path = _FolderPath + L"*.*";

	WIN32_FIND_DATA FindData = {};

	HANDLE hHandle = FindFirstFile(Path.c_str(), &FindData);

	// FindNextFile 은 더이상 찾을게 없을 때 false를 반환한다.
	while (FindNextFile(hHandle, &FindData))
	{
		// Folder인지 확인한다.
		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// .. 폴더는 제외
			if (!wcscmp(FindData.cFileName, L".."))
			{
				continue;
			}

			// 다른 폴더의 경우 재귀함수로 다시 탐색해간다.
			FindStageAsset(_FolderPath + FindData.cFileName + L"\\");
		}

		// 찾은 파일이 폴더가 아닌 경우
		else
		{
			wstring FilePath = _FolderPath + FindData.cFileName;
			wstring ContentPath = CPathMgr::GetInst()->GetContentPath();
			wstring RelativePath = FilePath.substr(ContentPath.length(), FilePath.length());

			m_vecStageAsset.push_back(RelativePath);
			m_vecbSelected.push_back(false);
		}
	}
}
