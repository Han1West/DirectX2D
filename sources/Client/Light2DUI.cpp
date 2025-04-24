#include "pch.h"
#include "Light2DUI.h"

#include <Engine/CLight2D.h>

Light2DUI::Light2DUI()
	: ComponentUI("Light2DUI", COMPONENT_TYPE::LIGHT2D)
{
}

Light2DUI::~Light2DUI()
{
}

void Light2DUI::Render_Update()
{
	ComponentTitle("Light2D");
	CLight2D* pLight2D = GetTargetObject()->Light2D();

	tLight2DInfo Light2DInfo = pLight2D->GetLight2DInfo();

	// Color
	Vec3 Color = Light2DInfo.vColor;
	ImGui::Text("Color widget:");
	ImGui::SameLine();
	ImGui::ColorPicker4("MyColor##1", (float*)&Color);  // RGB °ª ÆíÁý
	pLight2D->SetLightColor(Color);

	// LightType
	const char* szLightType[3] = { "Directional", "Point", "Spot" };
	int LightType = Light2DInfo.Type;
	ImGui::Combo("##combo", &LightType, szLightType, 3);
	if (LightType != Light2DInfo.Type)
		pLight2D->SetLightType((LIGHT_TYPE)LightType);

	// Radius
	ImGui::BeginDisabled(LightType == (int)LIGHT_TYPE::DIRECTIONAL);
	float Radius = Light2DInfo.Radius;
	ImGui::Text("Radius");
	ImGui::SameLine(120);
	ImGui::InputFloat("##Radius", &Radius);
	pLight2D->SetRadius(Radius);
	ImGui::EndDisabled();
	
	// Angle
	ImGui::BeginDisabled(LightType == (int)LIGHT_TYPE::DIRECTIONAL || LightType == (int)LIGHT_TYPE::POINT);
	float Angle = Light2DInfo.Angle;
	ImGui::Text("Angle");
	ImGui::SameLine(120);
	ImGui::InputFloat("##Angle", &Angle);
	pLight2D->SetAngle(Angle);
	ImGui::EndDisabled();


}
