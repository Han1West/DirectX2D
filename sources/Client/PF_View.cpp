#include "pch.h"
#include "PF_View.h"

PF_View::PF_View()
	: PF_Sub("PF_View")
	, m_AtlasTex(nullptr)
	, m_WidthSize(200.f)
	, m_WheelScale(1.f)
	, m_Ratio(0.f)
{
}

PF_View::~PF_View()
{
}

void PF_View::Render_Update()
{
	if (nullptr == m_AtlasTex)
		return;

	// 이미지	
	ImVec2 uv_min = ImVec2(0.0f, 0.0f);
	ImVec2 uv_max = ImVec2(1.0f, 1.0f);

	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 border_col = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);

	// 실제 해상도 대비 출력 Image 의 비율
	m_Ratio = (m_WidthSize * m_WheelScale) / m_AtlasTex->GetWidth();

	ImGui::Image(m_AtlasTex->GetSRV().Get(), ImVec2((m_WidthSize * m_WheelScale), m_AtlasTex->GetHeight() * m_Ratio)
		, uv_min, uv_max, tint_col, border_col);
}

void PF_View::SetAtlasTex(Ptr<CTexture> _Tex)
{
	if (m_AtlasTex == _Tex)
		return;

	m_AtlasTex = _Tex;
	m_WidthSize = 200.f;
}
