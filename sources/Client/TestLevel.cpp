#include "pch.h"
#include "TestLevel.h"
#include "pch.h"
#include "TestLevel.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CLayer.h>
#include <Engine/CGameObject.h>
#include <Engine/components.h>

#include <Engine/CAssetMgr.h>
#include <Engine/assets.h>

#include <Engine/CCollisionMgr.h>

#include <Scripts/CPlayerScript.h>
#include <Scripts/CMissileScript.h>
#include <Scripts/CCameraScript.h>
#include <Scripts/CWeaponScript.h>
#include <Scripts/CEditTileScript.h>

#include "CLevelSaveLoad.h"
void TestLevel::CreateTestLevel()
{
	// Texture 로딩하기
	Ptr<CTexture> pTex = CAssetMgr::GetInst()->Load<CTexture>(L"PlayerTex", L"Texture//Character.png");
	Ptr<CTexture> pAtlasTex = CAssetMgr::GetInst()->Load<CTexture>(L"TileMapTex", L"Texture\\TILE.bmp");
	Ptr<CTexture> pNoiseTex = CAssetMgr::GetInst()->Load<CTexture>(L"NoiseTex", L"Texture\\noise\\noise_03.png");

	// Sound 로딩하기
	//Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(L"Sound\\DM.wav", L"Sound\\DM.wav");
	//pSound->Play(0, 0.5f, true);


	CLevel* pLevel = new CLevel;

	// 테스트 레벨을 현재 레벨로 지정
	ChangeLevel(pLevel, LEVEL_STATE::STOP);
	wstring ContentPath = CPathMgr::GetInst()->GetContentPath();
	CLevel* LoadLevel = CLevelSaveLoad::LoadLevel(ContentPath + L"Level\\Main.lv");
	ChangeLevel(LoadLevel, LEVEL_STATE::PLAY);

	pLevel->GetLayer(0)->SetName(L"UI");
	pLevel->GetLayer(1)->SetName(L"Player");
	pLevel->GetLayer(2)->SetName(L"Enemy");
	pLevel->GetLayer(3)->SetName(L"Item");
	pLevel->GetLayer(4)->SetName(L"PlayerObject");
	pLevel->GetLayer(5)->SetName(L"EnemyObject");
	pLevel->GetLayer(6)->SetName(L"Wall");
	pLevel->GetLayer(7)->SetName(L"Background");

	CGameObject* pObject = nullptr;

	// ==========
	// MainCamera
	// ==========
	pObject = new CGameObject;
	pObject->SetName(L"MainCamera");
	pObject->AddComponent(new CCamera);
	pObject->AddComponent(new CCameraScript);

	// MainCamera 로 설정
	pObject->Camera()->SetPriority(0);

	// 모든 레이어를 찍도록 설정
	pObject->Camera()->LayerCheckAll();

	// 카메라 투영방식 설정
	pObject->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);

	pLevel->AddObject(0, pObject, false);

	// =================
	// 광원 오브젝트 추가
	// =================
	CGameObject* pLightObj = new CGameObject;
	pLightObj->SetName(L"Directional Light");
	pLightObj->AddComponent(new CLight2D);

	pLightObj->Transform()->SetRelativePos(-100.f, 0.f, 10.f);
	pLightObj->Light2D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
	pLightObj->Light2D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	pLightObj->Light2D()->SetRadius(200.f);

	pLevel->AddObject(0, pLightObj, false);
	

	// ======
	// Player
	// ======
	//CGameObject* pPlayer = new CGameObject;
	//pPlayer->SetName(L"PlayerBase");
	//pPlayer->AddComponent(new CMeshRender);
	//pPlayer->AddComponent(new CPlayerScript);
	//pPlayer->AddComponent(new CFlipbookPlayer);
	//pPlayer->AddComponent(new CCollider2D);

	//pPlayer->Transform()->SetRelativePos(0.f, 0.f, 10.f);
	//pPlayer->Transform()->SetRelativeScale(200.f, 200.f, 1.f);

	//pPlayer->Collider2D()->SetScale(Vec2(0.1f, 0.1f));

	//pPlayer->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	//Ptr<CMaterial> pMtlr = CAssetMgr::GetInst()->Load<CMaterial>(L"Material\\Default Material 0.mtrl", L"Material\\Default Material 0.mtrl");
	//pPlayer->MeshRender()->SetMaterial(pMtlr);
	//pPlayer->MeshRender()->GetMaterial()->SetTexParam(TEX_0, pTex);

	//pPlayer->FlipbookPlayer()->AddFlipbook(0, CAssetMgr::GetInst()->Load<CFlipbook>(L"Flipbook\\player\\idle\\PlayerIdle.flip", L"Flipbook\\player\\idle\\PlayerIdle.flip"));
	//pPlayer->FlipbookPlayer()->AddFlipbook(1, CAssetMgr::GetInst()->Load<CFlipbook>(L"Flipbook\\player\\walk\\PlayerWalk.flip", L"Flipbook\\player\\walk\\PlayerWalk.flip"));
	//pPlayer->FlipbookPlayer()->AddFlipbook(2, CAssetMgr::GetInst()->Load<CFlipbook>(L"Flipbook\\player\\attack\\PlayerAttack_Axe.flip", L"Flipbook\\player\\attack\\PlayerAttack_Axe.flip"));

	//pPlayer->FlipbookPlayer()->AddFlipbook(3, CAssetMgr::GetInst()->Load<CFlipbook>(L"Flipbook\\player\\idle\\PlayerIdle_M16.flip", L"Flipbook\\player\\idle\\PlayerIdle_M16.flip"));
	//pPlayer->FlipbookPlayer()->AddFlipbook(4, CAssetMgr::GetInst()->Load<CFlipbook>(L"Flipbook\\player\\walk\\PlayerWalk_M16.flip", L"Flipbook\\player\\walk\\PlayerWalk_M16.flip"));
	//pPlayer->FlipbookPlayer()->Play(0, 10, true);

	////Ptr<CPrefab> pPrefab = new CPrefab;
	////pPrefab->SetProtoObject(pTileMap);
	////CAssetMgr::GetInst()->AddAsset(L"TilePrefab", pPrefab);
	//pLevel->AddObject(3, pPlayer, false);


	//// =======
	//// m16
	//// =======

	//CGameObject* pM16 = new CGameObject;
	//pM16->SetName(L"M16");
	//pM16->AddComponent(new CMeshRender);
	//pM16->AddComponent(new CFlipbookPlayer);
	//pM16->AddComponent(new CCollider2D);
	//pM16->AddComponent(new CWeaponScript);

	//pM16->Transform()->SetRelativePos(20.f, 20.f, 10.f);
	//pM16->Transform()->SetRelativeScale(140.f, 140.f, 1.f);

	//pM16->Collider2D()->SetScale(Vec2(0.1f, 0.1f));
	//pM16->Collider2D()->SetName(L"Weapon");

	//pM16->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	//pMtlr = CAssetMgr::GetInst()->Load<CMaterial>(L"Material\\M16Mtrl.mtrl", L"Material\\M16Mtrl.mtrl");
	//pM16->MeshRender()->SetMaterial(pMtlr);
	//pM16->MeshRender()->GetMaterial()->SetTexParam(TEX_0, pTex);

	//pM16->FlipbookPlayer()->AddFlipbook(0, CAssetMgr::GetInst()->Load<CFlipbook>(L"Flipbook\\weapon\\M16.flip", L"Flipbook\\weapon\\M16.flip"));
	//pM16->FlipbookPlayer()->Play(0, 10, false);

	//pLevel->AddObject(4, pM16, false);

	//// =======
	//// TileMap
	//// =======
	CGameObject* pTileMap = new CGameObject;
	pTileMap->SetName(L"TileMap");
	pTileMap->AddComponent(new CTileMap);
	pTileMap->AddComponent(new CEditTileScript);

	pTileMap->Transform()->SetRelativePos(0.f, 0.f, 100.f);

	pTileMap->TileMap()->SetAtlasTexture(pAtlasTex);
	pTileMap->TileMap()->SetTileAtlasSize(64, 64);
	pTileMap->TileMap()->SetTileSize(64.f, 64.f);
	pTileMap->TileMap()->SetTileColRow(8, 6);

	pLevel->AddObject(0, pTileMap, false);

	// Particle Object
	//CGameObject* pParticle = new CGameObject;
	//pParticle->SetName(L"Particle");
	//pParticle->AddComponent(new CParticleSystem);

	//Ptr<CTexture> pParticleTex = CAssetMgr::GetInst()->Load<CTexture>(L"Particle_01", L"Texture\\particle\\TX_GlowScene_2.png");
	//pParticle->ParticleSystem()->SetParticleTexture(pParticleTex);

	//pLevel->AddObject(2, pParticle, false);

	//PostProcess
	CGameObject* pPostProcess = new CGameObject;
	pPostProcess->SetName(L"PostProcess");
	pPostProcess->AddComponent(new CMeshRender);
	
	pPostProcess->Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f));
	pPostProcess->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 1.f));

	pPostProcess->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	pPostProcess->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"NoiseMtrl"));

	pPostProcess->MeshRender()->GetMaterial()->SetTexParam(TEX_0, CAssetMgr::GetInst()->FindAsset<CTexture>(L"PostProcessTex"));
	pPostProcess->MeshRender()->GetMaterial()->SetTexParam(TEX_1, pNoiseTex);
	pLevel->AddObject(0, pPostProcess, false);
	//CreateObject(pPostProcess, 0, false);

	// 벽과 문
	CCollisionMgr::GetInst()->CollisionCheck(1, 2);
	CCollisionMgr::GetInst()->CollisionCheck(1, 4);
	
	// 플레이어
	CCollisionMgr::GetInst()->CollisionCheck(2, 3);
	CCollisionMgr::GetInst()->CollisionCheck(2, 4);
	CCollisionMgr::GetInst()->CollisionCheck(2, 6);
	CCollisionMgr::GetInst()->CollisionCheck(2, 7);
	CCollisionMgr::GetInst()->CollisionCheck(2, 8);

	// 적
	CCollisionMgr::GetInst()->CollisionCheck(3, 1);
	CCollisionMgr::GetInst()->CollisionCheck(3, 4);
	CCollisionMgr::GetInst()->CollisionCheck(3, 5);
	CCollisionMgr::GetInst()->CollisionCheck(3, 8);

	// 공격들
	CCollisionMgr::GetInst()->CollisionCheck(5, 1);
	CCollisionMgr::GetInst()->CollisionCheck(5, 8);
	CCollisionMgr::GetInst()->CollisionCheck(6, 1);
	CCollisionMgr::GetInst()->CollisionCheck(6, 8);
}
