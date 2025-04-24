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
	
	// Far �� Near(1) ���� �۰ų� ������ �ȵȴ�. �þ� ������ Near���� Far�̱� ����
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

	// ���� ī�޶� ���� �ɼ��� ���� �����̸� ��Ȱ��ȭ
	ImGui::BeginDisabled(ProjType == (int)PROJ_TYPE::ORTHOGRAPHIC);
	// �þ߰� (���� ���� ����)
	float FOV = pCamera->GetFOV();
	ImGui::Text("Field Of View");
	ImGui::SameLine(120);
	FOV = (FOV * 180.f) / XM_PI;
	ImGui::DragFloat("##Fov", &FOV, 0.01f);
	FOV = (FOV * XM_PI) / 180.f;
	pCamera->SetFOV(FOV);
	ImGui::EndDisabled();

	// ���� ī�޶� ���� �ɼ��� ���� �����̸� ��Ȱ��ȭ
	ImGui::BeginDisabled(ProjType == (int)PROJ_TYPE::PERSPECTIVE);

	// ���� ���� ����
	float Scale = pCamera->GetScale();
	ImGui::Text("Scale");
	ImGui::SameLine(120);
	ImGui::DragFloat("##Scale", &Scale, 0.01f);
	// ���� ���� ������ 0 ���Ϸ� �������� �ȵȴ�.
	if (Scale <= 0.f)
		Scale = 0.1f;
	pCamera->SetFOV(Scale);
	ImGui::EndDisabled();


	// ����
	// LayerCheck 
	UINT LayerCheck = (int) pCamera->GetLayerCheck();
	ImGui::Text("LayerCheck");
	int ClickedIdx = -1;
	for (int i = 0; i < MAX_LAYER; ++i) { 
		bool bit_set = (LayerCheck & (1 << i)) != 0;  // i��° ��Ʈ�� ���� �ִ��� Ȯ��
		if (ImGui::Checkbox(std::to_string(i).c_str(), &bit_set))
		{
			ClickedIdx = i;
			if (bit_set)
			{
				LayerCheck |= (1 << i);  // ��Ʈ�� 1�� ����
			}
			else    
			{
				LayerCheck &= ~(1 << i);  // ��Ʈ�� 0���� ����
			}
			pCamera->LayerCheck(ClickedIdx);
		}
		if (i % 8 != 7) 
			ImGui::SameLine();  // 8���� üũ�ڽ��� �� ��x`�� ��ġ
	}
	ImGui::Text("Updated Value: 0x%08X", LayerCheck);

	// ī�޶� �켱����
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

