#include "pch.h"
#include "TileMapUI.h"

#include <Engine/CTileMap.h>

TileMapUI::TileMapUI()
	: ComponentUI("TileMapUI", COMPONENT_TYPE::TILEMAP)
{
}

TileMapUI::~TileMapUI()
{
}


void TileMapUI::Render_Update()
{
	CTileMap* pTileMap = GetTargetObject()->TileMap();

	Vec2 m_TileColRow = pTileMap->GetTileColRow();
	Vec2 m_TileSize = pTileMap->GetTileSize();
	Vec2 m_TileAtlasSize = pTileMap->GetTileAtlasSize();
	Ptr<CTexture> m_AtlasTexture = pTileMap->GetAtlasTexutre();

	ImGui::Text("Set TileColRow");
	ImGui::InputFloat2("##TileColRow", m_TileColRow);
	pTileMap->SetTileColRow(m_TileColRow.x, m_TileColRow.y);

	ImGui::Text("Set TileSize");
	ImGui::InputFloat2("##TileSize", m_TileSize);
	pTileMap->SetTileSize(m_TileSize.x, m_TileSize.y);

	ImGui::Text("Set TileAtlasSize");
	ImGui::InputFloat2("##TileAtlasSize", m_TileAtlasSize);
	pTileMap->SetTileAtlasSize(m_TileAtlasSize.x, m_TileAtlasSize.y);


	ImVec2 imageSize(216, 216);  // 전체 이미지 크기
	ImVec2 uv_min(0.f, 0.f); // 이미지 시작점 (0.0f ~ 1.0f)
	ImVec2 uv_max(1.f, 1.f); // 이미지 끝점 (0.0f ~ 1.0f)
	ImVec4 tint_col(1.0f, 1.0f, 1.0f, 1.0f); // 색상 적용
	ImVec4 border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);

	ImGui::Image(m_AtlasTexture->GetSRV().Get(), imageSize, uv_min, uv_max, tint_col, border_col);
}
