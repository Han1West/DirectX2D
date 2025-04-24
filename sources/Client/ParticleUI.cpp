#include "pch.h"
#include "ParticleUI.h"



#include <Engine/CMesh.h>
#include <Engine/CMaterial.h>
#include <Engine/CMeshRender.h>
#include <Engine/CParticleSystem.h>
#include <Engine/CAssetMgr.h>

#include "ListUI.h"
#include "TreeUI.h"
#include "CImGuiMgr.h"


ParticleUI::ParticleUI()
	: ComponentUI("ParticleUI", COMPONENT_TYPE::PARTICLE_SYSTEM)
{
}

ParticleUI::~ParticleUI()
{
}

void ParticleUI::Render_Update()
{
	ComponentTitle("Particle");
	
	CParticleSystem* pParticleSystem = GetTargetObject()->ParticleSystem();

	Ptr<CMesh> pMesh = pParticleSystem->GetMesh();
	Ptr<CMaterial> pMtrl = pParticleSystem->GetMaterial();

	string MeshName;
	if (nullptr == pMesh)
		MeshName = "None";
	else
	{
		MeshName = string(pMesh->GetKey().begin(), pMesh->GetKey().end());
	}

	ImGui::Text("Mesh");
	ImGui::SameLine(100);
	ImGui::SetNextItemWidth(150);
	ImGui::InputText("##ParticleMeshName", (char*)MeshName.c_str(), MeshName.length(), ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);

	if (ImGui::BeginDragDropTarget())
	{
		if (ImGui::AcceptDragDropPayload("ContentUI"))
		{
			const ImGuiPayload* pPayload = ImGui::GetDragDropPayload();
			TreeNode* pNode = *((TreeNode**)pPayload->Data);
			Ptr<CAsset> pAsset = (CAsset*)pNode->GetData();
			if (pAsset->GetAssetType() == ASSET_TYPE::MESH)
			{
				pParticleSystem->SetMesh((CMesh*)pAsset.Get());
				ImGui::SetWindowFocus(nullptr);
			}
		}

		ImGui::EndDragDropTarget();
	}


	ImGui::SameLine();
	if (ImGui::Button("##ParticleMeshBtn", ImVec2(18.f, 18.f)))
	{
		// ListUI �� Ȱ��ȭ ��Ű��
		ListUI* pListUI = (ListUI*)CImGuiMgr::GetInst()->FindUI("##ListUI");
		pListUI->SetName("Mesh");
		pListUI->SetActive(true);

		// ListUI �� �־��� ���ڿ� ���� ��������
		pListUI->AddItem("None");

		vector<wstring> vecAssetNames;
		CAssetMgr::GetInst()->GetAssetNames(ASSET_TYPE::MESH, vecAssetNames);
		pListUI->AddItem(vecAssetNames);

		// ���� Ŭ�� �� ȣ���ų �Լ� ���
		pListUI->AddDynamicDoubleCliked(this, (EUI_DELEGATE_2)&ParticleUI::SelectMesh);
	}


	string MtrlName;
	if (nullptr == pMtrl)
		MtrlName = "None";
	else
	{
		MtrlName = string(pMtrl->GetKey().begin(), pMtrl->GetKey().end());
	}

	ImGui::Text("Material");
	ImGui::SameLine(100);
	ImGui::SetNextItemWidth(150);
	ImGui::InputText("##ParticleMtrlName", (char*)MtrlName.c_str(), MtrlName.length(), ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);

	if (ImGui::BeginDragDropTarget())
	{
		if (ImGui::AcceptDragDropPayload("ContentUI"))
		{
			const ImGuiPayload* pPayload = ImGui::GetDragDropPayload();
			TreeNode* pNode = *((TreeNode**)pPayload->Data);
			Ptr<CAsset> pAsset = (CAsset*)pNode->GetData();
			if (pAsset->GetAssetType() == ASSET_TYPE::MATERIAL)
			{
				pParticleSystem->SetMaterial((CMaterial*)pAsset.Get());
				ImGui::SetWindowFocus(nullptr);
			}
		}

		ImGui::EndDragDropTarget();
	}


	ImGui::SameLine();
	if (ImGui::Button("##ParticleMtrlBtn", ImVec2(18.f, 18.f)))
	{
		// ListUI �� Ȱ��ȭ ��Ű��
		ListUI* pListUI = (ListUI*)CImGuiMgr::GetInst()->FindUI("##ListUI");
		pListUI->SetName("Material");
		pListUI->SetActive(true);

		// ListUI �� �־��� ���ڿ� ���� ��������
		pListUI->AddItem("None");

		vector<wstring> vecAssetNames;
		CAssetMgr::GetInst()->GetAssetNames(ASSET_TYPE::MATERIAL, vecAssetNames);
		pListUI->AddItem(vecAssetNames);

		// ���� Ŭ�� �� ȣ���ų �Լ� ���
		pListUI->AddDynamicDoubleCliked(this, (EUI_DELEGATE_2)&ParticleUI::SelectMaterial);
	}

	tParticleModule pModule = pParticleSystem->GetModule();

	// Spawn Module
	static bool Spawn = pModule.Module[(UINT)PARTICLE_MODULE::SPAWN];
	ImGui::Text("SpawnModule");
	ImGui::SameLine(140);
	ImGui::Checkbox("##Spawn", &Spawn);
	ImGui::BeginDisabled(Spawn == false);
	pModule.Module[(UINT)PARTICLE_MODULE::SPAWN] = Spawn;

	//int SpawnRate = (int)pModule->SpawnRate;
	//Vec4 SpawnColor = pModule->vSpawnColor;
	//float MinLife = (float)pModule->MinLife;
	//float MaxLife = (float)pModule->MaxLife;
	//Vec3 SpawnMinScale = pModule->vSpawnMinScale;
	//Vec3 SpawnMaxScale = pModule->vSpawnMaxScale;

	ImGui::Text("SpawnRate");
	ImGui::SameLine(140);
	ImGui::DragScalar("##SpawnRate", ImGuiDataType_U32, &pModule.SpawnRate);
	ImGui::Text("SpawnColor");
	ImGui::SameLine(140);
	ImGui::DragFloat4("##SpawnColor", pModule.vSpawnColor);
	ImGui::Text("MinLife");
	ImGui::SameLine(140);
	ImGui::DragFloat("##MinLife", &pModule.MinLife);
	ImGui::Text("MaxLife");
	ImGui::SameLine(140);
	ImGui::DragFloat("##MaxLife", &pModule.MaxLife);
	ImGui::Text("SpawnMinScale");
	ImGui::SameLine(140);
	ImGui::DragFloat3("##SpawnMinScale", pModule.vSpawnMinScale);
	ImGui::Text("SpawnMaxScale");
	ImGui::SameLine(140);
	ImGui::DragFloat3("##SpawnMaxScale", pModule.vSpawnMaxScale);


	const char* Shapes[] = { "Box", "Sphere" };
	//int Shape_idx = pModule.SpawnShape;
	const char* Shape_preview = Shapes[pModule.SpawnShape];

	ImGui::Text("SpawnShape");
	ImGui::SameLine(140);
	if (ImGui::BeginCombo("##SpawnShape", Shape_preview, 0))
	{
		for (int n = 0; n < IM_ARRAYSIZE(Shapes); n++)
		{
			const bool is_selected = (pModule.SpawnShape == n);
			if (ImGui::Selectable(Shapes[n], is_selected))
				pModule.SpawnShape = n;

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::Text("SpawnShapeScale");
	ImGui::SameLine(140);
	ImGui::DragFloat3("##SpawnShapeScaleX", pModule.SpawnShapeScale);

	const char* BlockShapes[] = { "Box", "Sphere" };
	//int Shape_idx = pModule.SpawnShape;
	const char* BlockShape_preview = BlockShapes[pModule.BlockSpawnShape];

	ImGui::Text("BlockSpawnShape");
	ImGui::SameLine(140);
	if (ImGui::BeginCombo("##BlockSpawnShape", Shape_preview, 0))
	{
		for (int n = 0; n < IM_ARRAYSIZE(BlockShapes); n++)
		{
			const bool is_selected = (pModule.BlockSpawnShape == n);
			if (ImGui::Selectable(BlockShapes[n], is_selected))
				pModule.BlockSpawnShape = n;

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::Text("BlockSpawnShapeScale X");
	ImGui::SameLine(140);
	ImGui::DragFloat3("##BlockSpawnShapeScale", pModule.BlockSpawnShapeScale);

	bool SpaceType = pModule.SpaceType;
	ImGui::Text("SpaceType");
	ImGui::SameLine(140);
	ImGui::Checkbox("##SpaceType", &SpaceType);
	pModule.SpaceType = SpaceType;
	ImGui::EndDisabled();

	ImGui::Text("");


	// SpawnBurst Module
	static bool SpawnBurst = pModule.Module[(UINT)PARTICLE_MODULE::SPAWN_BURST];
	ImGui::Text("SpawnBurstModule");
	ImGui::SameLine(140);
	ImGui::Checkbox("##SpawnBurst", &SpawnBurst);
	ImGui::BeginDisabled(SpawnBurst == false);
	pModule.Module[(UINT)PARTICLE_MODULE::SPAWN_BURST] = SpawnBurst;

	bool SpawnBrustRepeat = pModule.SpawnBurstRepeat;
	//int SpawnBurstCount = (int)pModule.SpawnBurstCount;
	//float SpawnBurstRepeatTime = (float)pModule.SpawnBurstRepeatTime;

	ImGui::Text("SpawnBurstRepeat");
	ImGui::SameLine(140);
	ImGui::Checkbox("##SpawnBurstRepeat", &SpawnBrustRepeat);
	pModule.SpawnBurstRepeat = SpawnBrustRepeat;
	ImGui::Text("SpawnBurstCount");
	ImGui::SameLine(140);
	ImGui::DragScalar("##SpawnBurstCount", ImGuiDataType_U32, &pModule.SpawnBurstCount);
	ImGui::Text("SpawnBurstRepeatTime");
	ImGui::SameLine(140);
	ImGui::DragFloat("##SpawnBurstRepeatTime", &pModule.SpawnBurstRepeatTime);
	ImGui::EndDisabled();

	ImGui::Text("");

	// Scale Module
	static bool Scale = pModule.Module[(UINT)PARTICLE_MODULE::SCALE];
	ImGui::Text("ScaleModule");
	ImGui::SameLine(140);
	ImGui::Checkbox("##Scale", &Scale);
	ImGui::BeginDisabled(Scale == false);
	pModule.Module[(UINT)PARTICLE_MODULE::SCALE] = Scale;

	//float StartScale = (float)pModule.StartScale;
	//float EndScale = (float)pModule.EndScale;

	ImGui::Text("StartScale");
	ImGui::SameLine(140);
	ImGui::DragFloat("##StartScale", &pModule.StartScale, 0.01f);
	ImGui::Text("EndScale");
	ImGui::SameLine(140);
	ImGui::DragFloat("##EndScale", &pModule.EndScale, 0.01f);

	ImGui::EndDisabled();

	ImGui::Text("");

	// AddVelocity Module
	bool AddVelocity = pModule.Module[(UINT)PARTICLE_MODULE::ADD_VELOCITY];
	ImGui::Text("AddVelocityModule");
	ImGui::SameLine(140);
	ImGui::Checkbox("##AddVelocity", &AddVelocity);
	ImGui::BeginDisabled(AddVelocity == false);
	pModule.Module[(UINT)PARTICLE_MODULE::ADD_VELOCITY] = AddVelocity;

	//Vec3 AddVelocityFixedDir = pModule.AddVelocityFixedDir;
	//float AddMinSpeed = (float)pModule.AddMinSpeed;
	//float AddMaxSpeed = (float)pModule.AddMaxSpeed;
	const char* VelocityType[] = { "Random", "FromCenter", "ToCenter", "Fixed"};
	int Velocity_Type_idx = pModule.AddVelocityType; 
	const char* Velocity_Type_preview_value = VelocityType[pModule.AddVelocityType];

	ImGui::Text("AddVelocityType");
	ImGui::SameLine(140);
	if (ImGui::BeginCombo("##AddVelocityType", Velocity_Type_preview_value, 0))
	{
		for (int n = 0; n < IM_ARRAYSIZE(VelocityType); n++)
		{
			const bool is_selected = (pModule.AddVelocityType == n);
			if (ImGui::Selectable(VelocityType[n], is_selected))
				pModule.AddVelocityType = n;

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::Text("AddVelocityFixedDir");
	ImGui::SameLine(140);
	ImGui::DragFloat3("##AddVelocityFixedDir", pModule.AddVelocityFixedDir, 0.1f);
	ImGui::Text("AddMinSpeed");
	ImGui::SameLine(140);
	ImGui::DragFloat("##AddMinSpeed", &pModule.AddMinSpeed);
	ImGui::Text("AddMaxSpeed");
	ImGui::SameLine(140);
	ImGui::DragFloat("##AddMaxSpeed", &pModule.AddMaxSpeed);

	ImGui::EndDisabled();

	// Drag Module
	bool Drag = pModule.Module[(UINT)PARTICLE_MODULE::DRAG];
	ImGui::Text("DragModule");
	ImGui::SameLine(140);
	ImGui::Checkbox("##Drag", &Drag);
	pModule.Module[(UINT)PARTICLE_MODULE::DRAG] = Drag;

	ImGui::BeginDisabled(Drag == false);

	ImGui::Text("DestNormalizedAge");
	ImGui::SameLine(140);
	ImGui::DragFloat("##DestNormalizedAge", &pModule.DestNormalizedAge, 0.01f);
	ImGui::Text("LimitSpeed");
	ImGui::SameLine(140);
	ImGui::DragFloat("##LimitSpeed", &pModule.LimitSpeed, 0.01f);

	ImGui::EndDisabled();


	ImGui::Text("");

	// NoiseForce Module
	bool NoiseForce = pModule.Module[(UINT)PARTICLE_MODULE::NOISE_FORCE];
	ImGui::Text("NoiseForce");
	ImGui::SameLine(140);
	ImGui::Checkbox("##NoiseForce", &NoiseForce);
	pModule.Module[(UINT)PARTICLE_MODULE::NOISE_FORCE] = NoiseForce;

	ImGui::BeginDisabled(NoiseForce == false);

	ImGui::Text("NoiseForceTerm");
	ImGui::SameLine(140);
	ImGui::DragFloat("##NoiseForceTerm", &pModule.NoiseForceTerm, 0.1f);
	ImGui::Text("NoiseForceScale");
	ImGui::SameLine(140);
	ImGui::DragFloat("##NoiseForceScale", &pModule.NoiseForceScale);

	ImGui::EndDisabled();


	ImGui::Text("");

	// Render Module
	bool Render = pModule.Module[(UINT)PARTICLE_MODULE::RENDER];
	ImGui::Text("RenderModule");
	ImGui::SameLine(140);
	ImGui::Checkbox("##Render", &Render);
	pModule.Module[(UINT)PARTICLE_MODULE::RENDER] = Render;

	ImGui::BeginDisabled(Render == false);

	bool FadeOut = (bool)pModule.FadeOut;
	bool VelocityAlignment = (bool)pModule.VelocityAlignment;

	ImGui::Text("EndColor");
	ImGui::SameLine(140);
	ImGui::DragFloat3("##EndColor", pModule.EndColor);
	ImGui::Text("FadeOut");
	ImGui::SameLine(140);
	ImGui::Checkbox("##FadeOut", &FadeOut);
	pModule.FadeOut = FadeOut;
	ImGui::Text("FadeOutStartRatio");
	ImGui::SameLine(140);
	ImGui::DragFloat("##FadeOutStartRatio", &pModule.FadeOutStartRatio, 0.01f);
	ImGui::Text("VelocityAlignment");
	ImGui::SameLine(140);
	ImGui::Checkbox("##VelocityAlignment", &VelocityAlignment);
	pModule.VelocityAlignment = VelocityAlignment;

	ImGui::EndDisabled();

	pParticleSystem->SetModule(pModule);


	ImGui::Text("");

}


void ParticleUI::SelectMesh(DWORD_PTR _ListUI, DWORD_PTR _SelectString)
{
	// ����Ʈ���� ����ų���� �׸��� �̸��� �޾ƿ´�.
	ListUI* pListUI = (ListUI*)_ListUI;
	string* pStr = (string*)_SelectString;

	if (*pStr == "None")
	{
		GetTargetObject()->MeshRender()->SetMesh(nullptr);
		return;
	}

	// �ش� �׸� ������ ã�Ƽ�, MeshRenderComponent �� �ش� �޽ø� �����ϰ� �Ѵ�.
	Ptr<CMesh> pMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(wstring(pStr->begin(), pStr->end()));
	if (nullptr == pMesh)
		return;

	GetTargetObject()->MeshRender()->SetMesh(pMesh);
}

void ParticleUI::SelectMaterial(DWORD_PTR _ListUI, DWORD_PTR _SelectString)
{
	// ����Ʈ���� ����ų���� �׸��� �̸��� �޾ƿ´�.
	ListUI* pListUI = (ListUI*)_ListUI;
	string* pStr = (string*)_SelectString;

	if (*pStr == "None")
	{
		GetTargetObject()->MeshRender()->SetMaterial(nullptr);
		return;
	}

	// �ش� �׸� ������ ã�Ƽ�, MeshRenderComponent �� �ش� �޽ø� �����ϰ� �Ѵ�.
	Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(wstring(pStr->begin(), pStr->end()));
	if (nullptr == pMtrl)
		return;

	GetTargetObject()->MeshRender()->SetMaterial(pMtrl);
}
