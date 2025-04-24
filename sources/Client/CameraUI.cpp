#include "pch.h"
#include "CameraUI.h"

#include <Engine/CCamera.h>
#include <Engine/CRenderMgr.h>

CameraUI::CameraUI()
	:ComponentUI("CamereUI", COMPONENT_TYPE::CAMERA)
{
}

CameraUI::~CameraUI()
{
}

void CameraUI::Render_Update()
{
	ComponentTitle("Camera");

	CCamera* pCamera = GetTargetObject()->Camera();

	ImGui::Text("Projection Type");
	ImGui::SameLine(120);

	// PROJ_TYPE
	const char* szProjType[2] = { "Pesrpective", "Orthograpic" };
	int ProjType = pCamera->GetProjType();
	ImGui::Combo("##combo", &ProjType, szProjType, 2);
	if (ProjType != (int)pCamera->GetProjType())
		pCamera->SetProjType((PROJ_TYPE)ProjType);

	// Far
	ImGui::Text("Far");
	ImGui::SameLine(120);
	float Far = pCamera->GetFar();
	ImGui::InputFloat("##Far", &Far);
	
	// Far 가 Near(1) 보다 작거나 같으면 안된다. 시야 범위는 Near에서 Far이기 때문
	if (Far <= 1.f)
		Far = 1.1f;
	if (Far != pCamera->GetFar())
		pCamera->SetFar(Far);

	// Width, AspectRatio

	float Width = pCamera->GetWidth();
	float AspectRatio = pCamera->GetAspectRatio();

	ImGui::Text("Width");
	ImGui::SameLine(120);
	ImGui::InputFloat("##Width", &Width);
	pCamera->SetWidth(Width);

	ImGui::Text("AspectRatio");
	ImGui::SameLine(120);
	ImGui::DragFloat("##AspectRatio", &AspectRatio, 0.01f);
	pCamera->SetAspectRatio(AspectRatio);

	// 현재 카메라 투영 옵션이 직교 투영이면 비활성화
	ImGui::BeginDisabled(ProjType == (int)PROJ_TYPE::ORTHOGRAPHIC);
	// 시야각 (원근 투영 전용)
	float FOV = pCamera->GetFOV();
	ImGui::Text("Field Of View");
	ImGui::SameLine(120);
	FOV = (FOV * 180.f) / XM_PI;
	ImGui::DragFloat("##Fov", &FOV, 0.01f);
	FOV = (FOV * XM_PI) / 180.f;
	pCamera->SetFOV(FOV);
	ImGui::EndDisabled();

	// 현재 카메라 투영 옵션이 원근 투영이면 비활성화
	ImGui::BeginDisabled(ProjType == (int)PROJ_TYPE::PERSPECTIVE);

	// 직교 투영 전용
	float Scale = pCamera->GetScale();
	ImGui::Text("Scale");
	ImGui::SameLine(120);
	ImGui::DragFloat("##Scale", &Scale, 0.01f);
	// 직교 투영 배율이 0 이하로 내려가면 안된다.
	if (Scale <= 0.f)
		Scale = 0.1f;
	pCamera->SetFOV(Scale);
	ImGui::EndDisabled();


	// 공통
	// LayerCheck 
	UINT LayerCheck = (int) pCamera->GetLayerCheck();
	ImGui::Text("LayerCheck");
	int ClickedIdx = -1;
	for (int i = 0; i < MAX_LAYER; ++i) { 
		bool bit_set = (LayerCheck & (1 << i)) != 0;  // i번째 비트가 켜져 있는지 확인
		if (ImGui::Checkbox(std::to_string(i).c_str(), &bit_set))
		{
			ClickedIdx = i;
			if (bit_set)
			{
				LayerCheck |= (1 << i);  // 비트를 1로 설정
			}
			else    
			{
				LayerCheck &= ~(1 << i);  // 비트를 0으로 설정
			}
			pCamera->LayerCheck(ClickedIdx);
		}
		if (i % 8 != 7) 
			ImGui::SameLine();  // 8개의 체크박스를 한 줄x`에 배치
	}
	ImGui::Text("Updated Value: 0x%08X", LayerCheck);

	// 카메라 우선순위
	int Priority = pCamera->GetPriority();
	int PrevPrior = Priority;
	ImGui::Text("Priority");
	ImGui::SameLine(120);
	if (ImGui::InputInt("##Priorty", &Priority))
	{
		if (Priority <= -1)
			Priority == -1;
		pCamera->SetPriority(Priority);
	}
	
}

