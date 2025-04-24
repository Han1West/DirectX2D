#include "pch.h"
#include "Collider2DUI.h"

#include <Engine/CGameObject.h>
#include <Engine/CCollider2D.h>

Collider2DUI::Collider2DUI()
	: ComponentUI("Collider2DUI", COMPONENT_TYPE::COLLIDER2D)
{
}

Collider2DUI::~Collider2DUI()
{
}

void Collider2DUI::Render_Update()
{
	ComponentTitle("Collider2D");

	CCollider2D* pCollider2D = GetTargetObject()->Collider2D();
	wstring Name = pCollider2D->GetName();
	string CurColliderName = string(pCollider2D->GetName().begin(), pCollider2D->GetName().end());

	if (!strcmp(CurColliderName.c_str(), ""))
	{
		CurColliderName = "unspecified";
	}

	// Collider Name
	ImGui::Text("Name");
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
	ImGui::Text(CurColliderName.c_str());
	ImGui::PopStyleColor();

	Vec2 vOffset = pCollider2D->GetOffset();
	Vec2 vScale = pCollider2D->GetScale();
	bool bIndependent = pCollider2D->IsIndependentScale();

	string Temp = "";
	ImGui::Text("ColliderName");
	ImGui::SetNextItemWidth(100);
	ImGui::InputText("##FlipbookName", m_Buffer, IM_ARRAYSIZE(m_Buffer));
	Temp = m_Buffer;
	ImGui::SameLine(110);
	if (ImGui::Button("Change Name"))
	{
		wstring NewColliderName = wstring(Temp.begin(), Temp.end());
		pCollider2D->SetName(NewColliderName);
	}


	ImGui::Text("Scale");
	ImGui::SameLine(100);
	ImGui::DragFloat2("##ColliderScale", vScale, 0.1f);
	pCollider2D->SetScale(vScale);

	ImGui::Text("Offset");
	ImGui::SameLine(100);
	ImGui::DragFloat2("##ColliderOffset", vOffset);
	pCollider2D->SetOffset(vOffset);

	ImGui::Text("Independent");
	ImGui::SameLine(100);
	ImGui::Checkbox("##Independent", &bIndependent);
	pCollider2D->SetIndependentScale(bIndependent);
}