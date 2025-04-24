#include "pch.h"
#include "CAssetMgr.h"

#include "CDevice.h"

void CAssetMgr::Init()
{
	CreateEngineMesh();
	CreateEngineTexture();
	CreateEngineGraphicShader();
	CreateEngineComputeShader();
	CreateEngineMaterial();
	CreateEngineSprite();
}

void CAssetMgr::CreateEngineMesh()
{
	Ptr<CMesh> pMesh = nullptr;

	// =========
	// PointMesh
	// =========
	Vtx v;
	UINT i = 0;

	pMesh = new CMesh(true);
	pMesh->Create(&v, 1, &i, 1);
	CAssetMgr::GetInst()->AddAsset(L"PointMesh", pMesh);

	// =========
	// Rect Mesh
	// 4 개의 정점의 위치 설계
	// 0 -- 1
	// | \  |
	// 3 -- 2   
	// =========	
	Vtx arrVtx[4] = {};
	arrVtx[0].vPos = Vec3(-0.5f, 0.5f, 0.f);
	arrVtx[0].vUV = Vec2(0.f, 0.f);
	arrVtx[0].vColor = Vec4(1.f, 0.f, 0.f, 1.f);

	arrVtx[1].vPos = Vec3(0.5f, 0.5f, 0.f);
	arrVtx[1].vUV = Vec2(1.f, 0.f);
	arrVtx[1].vColor = Vec4(0.f, 0.f, 1.f, 1.f);

	arrVtx[2].vPos = Vec3(0.5f, -0.5f, 0.f);
	arrVtx[2].vUV = Vec2(1.f, 1.f);
	arrVtx[2].vColor = Vec4(0.f, 1.f, 0.f, 1.f);

	arrVtx[3].vPos = Vec3(-0.5f, -0.5f, 0.f);
	arrVtx[3].vUV = Vec2(0.f, 1.f);
	arrVtx[3].vColor = Vec4(1.f, 0.f, 1.f, 1.f);

	// 인덱스
	UINT Idx[6] = { 0 , 1 , 2, 0 , 2 , 3 };

	// 클래스 or 구조체 + 가상함수
	const type_info& info = typeid(CMesh);
	const char* pName = info.name();

	pMesh = new CMesh(true);
	pMesh->Create(arrVtx, 4, Idx, 6);
	CAssetMgr::GetInst()->AddAsset(L"RectMesh", pMesh);

	// ==============
	// RectMesh_Debug
	// 4 개의 정점의 위치 설계
	// 0 -- 1
	// | \  |
	// 3 -- 2   
	// ==============
	UINT DebugRectIdx[5] = { 0 , 1 , 2, 3 , 0 };

	pMesh = new CMesh(true);
	pMesh->Create(arrVtx, 4, DebugRectIdx, 5);
	CAssetMgr::GetInst()->AddAsset(L"RectMesh_Debug", pMesh);


	// ==========
	// CircleMesh
	// ==========
	vector<Vtx>		vecVtx;
	vector<UINT>	vecIdx;
	

	UINT Slice = 40;
	float Radius = 0.5f;
	float Theta = 0.f;

	// 중심점
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vUV = Vec2(0.5f, 0.5f);
	vecVtx.push_back(v);

	for (UINT i = 0; i < Slice + 1; ++i)
	{
		v.vPos = Vec3(Radius * cosf(Theta), Radius * sinf(Theta), 0.f);
		v.vUV = Vec2(v.vPos.x + 0.5f, 1.f - (v.vPos.y + 0.5f));
		vecVtx.push_back(v);

		Theta += (XM_PI * 2.f) / Slice;
	}

	for (UINT i = 0; i < Slice; ++i)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 1);
		vecIdx.push_back(i + 2);
	}

	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	CAssetMgr::GetInst()->AddAsset(L"CircleMesh", pMesh);

	// =================
	// CircleMesh_Debug
	// =================	
	vecIdx.clear();
	for (size_t i = 0; i < vecVtx.size() - 1; ++i)
	{
		vecIdx.push_back(i + 1);
	}

	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	CAssetMgr::GetInst()->AddAsset(L"CircleMesh_Debug", pMesh);
}

void CAssetMgr::CreateEngineTexture()
{
	// NoiseTexture
	Load<CTexture>(L"NoiseTex", L"Texture//noise//noise_03.jpg");

	// PostPorcessTexture
	Vec2 vResolution = CDevice::GetInst()->GetRenderResolution();
	CreateTexture(L"PostProcessTex" , vResolution.x, vResolution.y
				 , DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE);
}

void CAssetMgr::CreateEngineGraphicShader()
{
	Ptr<CGraphicShader> pShader = nullptr;

	// ===========================
	// Std2DShader : 표준 2D 쉐이더
	// ===========================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\std2d.fx", "VS_Std2D");
	pShader->CreatePixelShader(L"Shader\\std2d.fx", "PS_Std2D");

	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetBSState(BS_TYPE::DEFAULT);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);

	CAssetMgr::GetInst()->AddAsset(L"Std2DShader", pShader);


	// ================================================
	// Std2DAlphaBlendShader : 표준 알파블렌드 2D 쉐이더
	// ================================================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\std2d.fx", "VS_Std2D");
	pShader->CreatePixelShader(L"Shader\\std2d.fx", "PS_Std2D_AlphaBlend");
	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetBSState(BS_TYPE::ALPHABLEND);
	//pShader->SetDSState(DS_TYPE::NO_TEST);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);
	CAssetMgr::GetInst()->AddAsset(L"Std2DAlphaBlendShader", pShader);


	// =================================
	// Std2D_PaperBurn : PaperBurn 쉐이더
	// =================================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\std2d.fx", "VS_Std2D");
	pShader->CreatePixelShader(L"Shader\\std2d.fx", "PS_Std2D_PaperBurn");

	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetBSState(BS_TYPE::DEFAULT);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);

	CAssetMgr::GetInst()->AddAsset(L"Std2DPaperBurnShader", pShader);


	// ==================================
	// TileMapShader : 타일맵 전용 쉐이더
	// ==================================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\tilemap.fx", "VS_TileMap");
	pShader->CreatePixelShader(L"Shader\\tilemap.fx", "PS_TileMap");

	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetBSState(BS_TYPE::DEFAULT);
	pShader->SetDSState(DS_TYPE::LESS);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);
	CAssetMgr::GetInst()->AddAsset(L"TileMapShader", pShader);

	// ===========
	// PostProcess
	// ===========
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\postprocess.fx", "VS_Post");
	pShader->CreatePixelShader(L"Shader\\postprocess.fx", "PS_Post");

	pShader->SetBSState(BS_TYPE::DEFAULT);
	pShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetRSState(RS_TYPE::CULL_NONE);
	
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);

	CAssetMgr::GetInst()->AddAsset(L"PostProcessShader", pShader);


	// ===========
	// Distortion
	// ===========
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\postprocess.fx", "VS_Distortion");
	pShader->CreatePixelShader(L"Shader\\postprocess.fx", "PS_Distortion");

	pShader->SetBSState(BS_TYPE::DEFAULT);
	pShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetRSState(RS_TYPE::CULL_NONE);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);

	CAssetMgr::GetInst()->AddAsset(L"DistortionShader", pShader);

	// ===========
	// Vortex
	// ===========
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\postprocess.fx", "VS_Vortex");
	pShader->CreatePixelShader(L"Shader\\postprocess.fx", "PS_Vortex");

	pShader->SetBSState(BS_TYPE::DEFAULT);
	pShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetRSState(RS_TYPE::CULL_NONE);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);

	CAssetMgr::GetInst()->AddAsset(L"VortexShader", pShader);

	// ===========
	// Noise
	// ===========
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\postprocess.fx", "VS_Noise");
	pShader->CreatePixelShader(L"Shader\\postprocess.fx", "PS_Noise");

	pShader->SetBSState(BS_TYPE::DEFAULT);
	pShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetRSState(RS_TYPE::CULL_NONE);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);

	CAssetMgr::GetInst()->AddAsset(L"NoiseShader", pShader);


	// ===========
	// Fade
	// ===========
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\postprocess.fx", "VS_Fade");
	pShader->CreatePixelShader(L"Shader\\postprocess.fx", "PS_Fade");

	pShader->SetBSState(BS_TYPE::DEFAULT);
	pShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetRSState(RS_TYPE::CULL_NONE);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);

	CAssetMgr::GetInst()->AddAsset(L"FadeShader", pShader);

	// ===========
	// Highlight
	// ===========
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\postprocess.fx", "VS_Highlight");
	pShader->CreatePixelShader(L"Shader\\postprocess.fx", "PS_Highlight");

	pShader->SetBSState(BS_TYPE::DEFAULT);
	pShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetRSState(RS_TYPE::CULL_NONE);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);

	CAssetMgr::GetInst()->AddAsset(L"HighlightShader", pShader);

	// ===========
	// Highlight InGame
	// ===========
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\postprocess.fx", "VS_HighlightIngame");
	pShader->CreatePixelShader(L"Shader\\postprocess.fx", "PS_HighlightIngame");

	pShader->SetBSState(BS_TYPE::DEFAULT);
	pShader->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetRSState(RS_TYPE::CULL_NONE);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);

	CAssetMgr::GetInst()->AddAsset(L"HighlightIngameShader", pShader);

	// =================================
	// DebugShapeShader : 디버그용 쉐이더
	// =================================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\debug.fx", "VS_DebugShape");
	pShader->CreatePixelShader(L"Shader\\debug.fx", "PS_DebugShape");
	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	CAssetMgr::GetInst()->AddAsset(L"DebugShapeShader", pShader);

	// ==============
	// ParticleShader
	// ==============
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\particle.fx", "VS_Particle");
	pShader->CreateGeometryShader(L"Shader\\particle.fx", "GS_Particle");
	pShader->CreatePixelShader(L"Shader\\particle.fx", "PS_Particle");

	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetBSState(BS_TYPE::ALPHABLEND);
	pShader->SetDSState(DS_TYPE::NO_WRITE);		

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_PARTICLE);
	CAssetMgr::GetInst()->AddAsset(L"ParticleShader", pShader);

	// ===========================
	// BloodShader 
	// ===========================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\blood.fx", "VS_Blood");
	pShader->CreatePixelShader(L"Shader\\blood.fx", "PS_Blood");

	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetBSState(BS_TYPE::ALPHABLEND);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);

	CAssetMgr::GetInst()->AddAsset(L"BloodShader", pShader);

	// ===========================
	// BulletShader 
	// ===========================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\bullet.fx", "VS_Bullet");
	pShader->CreatePixelShader(L"Shader\\bullet.fx", "PS_Bullet");

	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetBSState(BS_TYPE::ALPHABLEND);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);

	CAssetMgr::GetInst()->AddAsset(L"BulletShader", pShader);

	// ===========================
	// WeaponShader
	// ===========================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\weapon.fx", "VS_Weapon");
	pShader->CreatePixelShader(L"Shader\\weapon.fx", "PS_Weapon");

	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetBSState(BS_TYPE::ALPHABLEND);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);

	CAssetMgr::GetInst()->AddAsset(L"WeaponShader", pShader);

	// ===========================
	// CursorShader
	// ===========================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\cursor.fx", "VS_Cursor");
	pShader->CreatePixelShader(L"Shader\\cursor.fx", "PS_Cursor");

	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetBSState(BS_TYPE::ALPHABLEND);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);

	CAssetMgr::GetInst()->AddAsset(L"CursorShader", pShader);

	// ===========================
	// UIShader
	// ===========================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\UI.fx", "VS_UI");
	pShader->CreatePixelShader(L"Shader\\UI.fx", "PS_UI");

	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetBSState(BS_TYPE::ALPHABLEND);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);

	CAssetMgr::GetInst()->AddAsset(L"UIShader", pShader);

	// ===========================
	// UIShader
	// ===========================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\UI.fx", "VS_UI");
	pShader->CreatePixelShader(L"Shader\\UI.fx", "PS_MainBackground");

	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetBSState(BS_TYPE::ALPHABLEND);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);

	CAssetMgr::GetInst()->AddAsset(L"MainBackgroundShader", pShader);

	// ===========================
	// StartUIShader
	// ===========================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\startUI.fx", "VS_StartUI");
	pShader->CreatePixelShader(L"Shader\\startUI.fx", "PS_StartUI");

	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetBSState(BS_TYPE::ALPHABLEND);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);

	CAssetMgr::GetInst()->AddAsset(L"StartUIShader", pShader);


	// ===========================
	// ExitUIShader
	// ===========================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"Shader\\exitUI.fx", "VS_ExitUI");
	pShader->CreatePixelShader(L"Shader\\exitUI.fx", "PS_ExitUI");

	pShader->SetRSState(RS_TYPE::CULL_NONE);
	pShader->SetBSState(BS_TYPE::ALPHABLEND);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);

	CAssetMgr::GetInst()->AddAsset(L"ExitUIShader", pShader);
}

#include "CParticleTickCS.h"

void CAssetMgr::CreateEngineComputeShader()
{
	AddAsset<CComputeShader>(L"ParticleTickCS", new CParticleTickCS);
}

void CAssetMgr::CreateEngineMaterial()
{
	Ptr<CMaterial> pMtrl = nullptr;

	// Std2DMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"Std2DMtrl");
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"Std2DShader"));
	AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);
	
	// Std2DAlphaBlendMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"Std2DAlphaBlendMtrl");
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"Std2DAlphaBlendShader"));
	AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);

	// Std2DPaperBurnMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"Std2DPaperBurnMtrl");
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"Std2DPaperBurnShader"));
	AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);

	// TileMapMaterial
	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"TileMapMtrl");
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"TileMapShader"));
	AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);

	// PostProcessMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"PostProcessMtrl");
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"PostProcessShader"));
	pMtrl->SetTexParam(TEX_0, FindAsset<CTexture>(L"PostProcessTex"));
	AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);

	// DistortionMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"DistortionMtrl");
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"DistortionShader"));
	pMtrl->SetTexParam(TEX_0, FindAsset<CTexture>(L"PostProcessTex"));
	pMtrl->SetTexParam(TEX_1, FindAsset<CTexture>(L"NoiseTex"));
	AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);
	
	// VortexMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"VortexMtrl");
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"VortexShader"));
	pMtrl->SetTexParam(TEX_0, FindAsset<CTexture>(L"PostProcessTex"));
	AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);	

	// DebugShapeMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"DebugShapeMtrl");
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"DebugShapeShader"));
	AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);

	// ParticleMtrl 	
	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"ParticleMtrl");
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"ParticleShader"));
	AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);

	// NoiseMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetName(L"NoiseMtrl");
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"NoiseShader"));
	pMtrl->SetTexParam(TEX_0, FindAsset<CTexture>(L"PostProcessTex"));
	//pMtrl->SetTexParam(TEX_1, FindAsset<CTexture>(L"NoiseTex"));
	AddAsset<CMaterial>(pMtrl->GetName(), pMtrl);
}

void CAssetMgr::CreateEngineSprite()
{
	//Ptr<CTexture> pLinkTex = Load<CTexture>(L"LinkAtlas", L"Texture\\link.png");

	//Ptr<CFlipbook> pFlipbook = new CFlipbook(true);
	//Ptr<CSprite> pSprite = nullptr;

	//for (int i = 0; i < 10; ++i)
	//{
	//	pSprite = new CSprite(true);
	//	pSprite->SetAtlasTexture(pLinkTex);
	//	pSprite->SetLeftTop(Vec2(i * 120.f, 520.f));
	//	pSprite->SetSlice(Vec2(120.f, 130.f));
	//	pSprite->SetBackground(Vec2(200.f, 200.f));

	//	if (i == 3)
	//		pSprite->SetOffset(Vec2(10.f, 0.f));

	//	wchar_t szKey[255] = {};
	//	swprintf_s(szKey, 255, L"Sprite\\LinkWalkDown_%d.sprite", i);

	//	// Sprite 등록
	//	AddAsset(szKey, pSprite);

	//	// flipbook 에 Sprite 추가
	//	pFlipbook->AddSprite(pSprite);
	//			
	//	// Sprite 저장
	//	pSprite->Save(CPathMgr::GetInst()->GetContentPath() + pSprite->GetKey());
	//}

	//// flipbook 등록
	//AddAsset(L"Flipbook\\LinkWalkDown.flip", pFlipbook);

	//// Flipbook 저장
	//pFlipbook->Save(CPathMgr::GetInst()->GetContentPath() + pFlipbook->GetKey());

	//Ptr<CFlipbook> pFlipbook = new CFlipbook(true);

	//Ptr<CSprite> pSprite = new CSprite;
	//pSprite->Load(L"D:\\DirectX\\OutputFile\\Content\\Sprite\\idle_down.sprite");

	//// flipbook 에 Sprite 추가
	//pFlipbook->AddSprite(pSprite);

	//// flipbook 등록
	//AddAsset(L"Flipbook\\LinkWalkDown.flip", pFlipbook);

	//// Flipbook 저장
	//pFlipbook->Save(CPathMgr::GetInst()->GetContentPath() + pFlipbook->GetKey());
}
