#include "pch.h"
#include "StageEditor.h"

#include "CImGuiMgr.h"
#include "SE_PreloadView.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CGameObject.h>
#include <Engine/CTileMap.h>
#include <Engine/CKeyMgr.h>

#include <Engine/components.h>
#include <Engine/assets.h>

#include <Scripts/CEditTileScript.h>
#include <Scripts/CScriptMgr.h>
#include <Scripts/CDoorScript.h>

static const char* sitems[] = { "Tile", "Object" };
static int sIdx;

StageEditor::StageEditor()
	: EditorUI("StageEditor")
	, m_CurAtlasName()
	, m_PreLoadView(nullptr)
	, m_TileAtlasSize(0.f, 0.f)
	, m_TileSize(0.f, 0.f)
	, m_TileColRow(0, 0)
	, m_bPopup(false)
	, m_bDrag(false)
	, m_NBuffer {}
	, m_Idx()
	, m_bEdit(false)
{
}

StageEditor::~StageEditor()
{
}


void StageEditor::Init()
{
	m_PreLoadView = (SE_PreloadView*)CImGuiMgr::GetInst()->FindUI("SE_PreloadView");

	m_PreLoadView->m_Owner = this;
}

void StageEditor::Render_Update()
{
	m_PreLoadView->SetActive(IsActive());

	string strAtlas;

	// 콤보 박스 생성
	if (ImGui::BeginCombo("##Select ObjType", sitems[m_Idx]))
	{
		for (int i = 0; i < (sizeof(sitems) / sizeof(char*)); ++i)
		{
			bool isSelected = (m_Idx == i);

			if (ImGui::Selectable(sitems[i], isSelected))
			{
				m_Idx = i;
			}
			// 선택된 항목 강조
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	ImGui::BeginDisabled(m_Idx == 1);
	TileUI();
	ImGui::EndDisabled();

	ImGui::BeginDisabled(m_Idx == 0);
	ObjectUI();
	ImGui::EndDisabled();


	if (m_CurAtlasName != L"")
	{
		strAtlas = string(m_CurAtlasName.begin(), m_CurAtlasName.end());
	}

	ImGui::Text("Cur Atlas");
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
	ImGui::Text(strAtlas.c_str());
	ImGui::PopStyleColor();
	
	if(m_CurTexture != nullptr)
	{
		ImVec2 imageSize(512, 512);  // 전체 이미지 크기
		ImVec2 uv_min(0.f, 0.f); // 이미지 시작점 (0.0f ~ 1.0f)
		ImVec2 uv_max(1.f, 1.f); // 이미지 끝점 (0.0f ~ 1.0f)
		ImVec4 tint_col(1.0f, 1.0f, 1.0f, 1.0f); // 색상 적용
		ImVec4 border_col(0.0f, 0.0f, 0.0f, 0.0f);

		ImGui::Image(m_CurTexture->GetSRV().Get(), imageSize, uv_min, uv_max, tint_col, border_col);
	}


	if (ImGui::Button("Edit"))
	{
		ChangeMode();
		m_bEdit = !m_bEdit;
	}

	if (m_bEdit)
	{
		TileEdit();
	}

	if (ImGui::Button("Drag Object"))
	{
		m_bDrag = !m_bDrag;
	}

	if (m_bDrag)
	{
		ObjDrag();
	}

	ShowPopup();
}


void StageEditor::TileUI()
{
	ImGui::Text("Set TileRowCol");
	ImGui::InputFloat2("##TileColRow", m_TileColRow);

	ImGui::Text("Set TileSize");
	ImGui::InputFloat2("##TileSize", m_TileSize);

	ImGui::Text("Set TileAtlasSize");
	ImGui::InputFloat2("##TileAtlasSize", m_TileAtlasSize);

	if (ImGui::Button("Add New TileMap"))
	{
		if (nullptr == m_CurTexture)
		{
			m_bPopup = true;
		}

		else
		{
			CGameObject* pTileMap = new CGameObject;
			pTileMap->AddComponent(new CTileMap);
			pTileMap->AddComponent(new CEditTileScript);

			pTileMap->SetName(L"TileMap");
			pTileMap->Transform()->SetRelativePos(0.f, 0.f, 100.f);

			//m_CurTexture = CAssetMgr::GetInst()->Load<CTexture>(m_CurAtlasName, m_CurAtlasName);

			pTileMap->TileMap()->SetAtlasTexture(m_CurTexture);
			pTileMap->TileMap()->SetTileColRow(m_TileColRow.x, m_TileColRow.y);
			pTileMap->TileMap()->SetTileSize(m_TileSize.x, m_TileSize.y);
			pTileMap->TileMap()->SetTileAtlasSize(m_TileAtlasSize.x, m_TileAtlasSize.y);

			CreateObject(pTileMap, 0, false);
		}
	}
}

void StageEditor::ObjectUI()
{
	ImGui::Text("Set Name");
	ImGui::InputText("##ObjName", m_NBuffer, sizeof(m_NBuffer));

	string ObjName = m_NBuffer;

	ImGui::Text("Set Pos");
	ImGui::InputFloat2("##ObjPos", m_ObjPos);

	ImGui::Text("Set Scale");
	ImGui::InputFloat2("##ObjScale", m_ObjScale);


	ImGui::Text("SetDeco");
	ImGui::SameLine(100);
	ImGui::Checkbox("##SetDeco", &m_bDeco);

	ImGui::Text("SetWall");
	ImGui::SameLine(100);
	ImGui::Checkbox("##SetWall", &m_bWall);

	ImGui::Text("SetRenderWall");
	ImGui::SameLine(100);
	ImGui::Checkbox("##SetRenderWall", &m_bWallCorner);


	if (ImGui::Button("Add New Object"))
	{
		if (nullptr == m_CurTexture)
		{
			m_bPopup = true;
		}

		else
		{
			// 재질 생성
			Ptr<CMaterial> pMtrl = new CMaterial;
			wstring MtrlKey = L"Material\\" + wstring(ObjName.begin(), ObjName.end()) + L".mtrl";


			CGameObject* pObject = new CGameObject;
			pObject->AddComponent(new CMeshRender);

			pObject->Transform()->SetRelativePos(Vec3(m_ObjPos.x, m_ObjPos.y, 0.f));
			pObject->Transform()->SetRelativeScale(Vec3(m_ObjScale.x, m_ObjScale.y, 0.f));
			pObject->SetName(wstring(ObjName.begin(), ObjName.end()));

			pObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
			pObject->MeshRender()->SetMaterial(pMtrl);
			pObject->MeshRender()->GetMaterial()->SetShader(CAssetMgr::GetInst()->FindAsset<CGraphicShader>(L"Std2DShader"));
			pObject->MeshRender()->GetMaterial()->SetTexParam(TEX_0, m_CurTexture);
			
			if (nullptr == (CAssetMgr::GetInst()->Load<CMaterial>(MtrlKey, MtrlKey)))
			{
				CAssetMgr::GetInst()->AddAsset(MtrlKey, pMtrl);
				pMtrl->Save(CPathMgr::GetInst()->GetContentPath() + MtrlKey);
			}
			else
			{
				pMtrl = CAssetMgr::GetInst()->Load<CMaterial>(MtrlKey, MtrlKey);
				pObject->MeshRender()->SetMaterial(pMtrl);
			}

			if (pObject->GetName() == L"Door")
			{
				pObject->AddComponent(new CDoorScript);
				pObject->AddComponent(new CCollider2D);
				pObject->Collider2D()->SetName(L"Door");
				pObject->Collider2D()->SetScale(Vec2(1.f, 1.f));
				CreateObject(pObject, 1, false);
				return;
			}
			if (m_bDeco)
			{
				CreateObject(pObject, 7, false);
			}

			if (m_bWall)
			{
				pObject->AddComponent(new CCollider2D);
				pObject->Collider2D()->SetName(L"Wall");
				pObject->Collider2D()->SetScale(Vec2(1.f, 1.f));
				CreateObject(pObject, 1, false);
			}
			
			if(m_bWallCorner)
			{
				CreateObject(pObject, 1, false);
			}

			if (!m_bDeco && !m_bWall && !m_bWallCorner)
			{
				pObject->AddComponent(new CCollider2D);
				pObject->Collider2D()->SetName(L"StgObj");
				pObject->Collider2D()->SetScale(Vec2(1.f, 1.f));
				CreateObject(pObject, 8, false);
			}			
		}
	}
}

void StageEditor::ChangeMode()
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	//pCurLevel->SetEdit(pCurLevel->IsEdit());

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		vector<CGameObject*> vecCurObjects = pCurLevel->GetLayer(i)->GetParentObjects();
		for (UINT j = 0; j < vecCurObjects.size(); ++j)
		{
			if (vecCurObjects[j]->GetName() == L"TileMap")
			{
				vector<CScript*> pScripts = vecCurObjects[j]->GetScripts();
				if (pScripts[0]->GetScriptType() == SCRIPT_TYPE::EDITTILESCRIPT)
				{
					CEditTileScript* TileScript = dynamic_cast<CEditTileScript*>(pScripts[0]);
					TileScript->SetActive(!TileScript->IsActive());
					TileScript->SetChange(true);
				}
			}
		}
	}
}


void StageEditor::TileEdit()
{
	ImGui::Text("Set Idx");
	ImGui::InputInt("##TileIdx", &m_TileIdx);

	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	CGameObject* pTileMapObj = nullptr;

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		vector<CGameObject*> vecCurObjects = pCurLevel->GetLayer(i)->GetParentObjects();
		for (UINT j = 0; j < vecCurObjects.size(); ++j)
		{
			if (vecCurObjects[j]->GetName() == L"TileMap")
			{
				pTileMapObj = vecCurObjects[j];
			}
		}
	}

	// 현재 타일맵이 있다면
	if (pTileMapObj != nullptr)
	{
		Vec2 m_TileAtlasColRow = pTileMapObj->TileMap()->GetTileAtlasColRow();

		int a = m_TileAtlasColRow.x;
		int b = m_TileAtlasColRow.y;

		Vec2 Slice = Vec2(1.f / m_TileAtlasColRow.x, 1.f / m_TileAtlasColRow.y);
		Vec2 LeftTop = Vec2(Slice.x * (m_TileIdx % (int)m_TileAtlasColRow.x), Slice.y * (m_TileIdx / (int)m_TileAtlasColRow.x));

		if (ImGui::Button("Apply"))
		{
			vector<CScript*> pScripts = pTileMapObj->GetScripts();
			if (pScripts[0]->GetScriptType() == SCRIPT_TYPE::EDITTILESCRIPT)
			{
				CEditTileScript* TileScript = dynamic_cast<CEditTileScript*>(pScripts[0]);
				TileScript->SetTileInfo(LeftTop, Slice);
			}
		}


		ImGui::Text("Current Tile");
		ImVec2 imageSize(64, 64);  // 전체 이미지 크기
		ImVec2 uv_min(LeftTop.x, LeftTop.y); // 이미지 시작점 (0.0f ~ 1.0f)
		ImVec2 uv_max(LeftTop.x + Slice.x, LeftTop.y + Slice.y); // 이미지 끝점 (0.0f ~ 1.0f)
		ImVec4 tint_col(1.0f, 1.0f, 1.0f, 1.0f); // 색상 적용
		ImVec4 border_col(0.0f, 0.0f, 0.0f, 0.0f);

		ImGui::Image(m_CurTexture->GetSRV().Get(), imageSize, uv_min, uv_max, tint_col, border_col);
	}

}

void StageEditor::ObjDrag()
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	Vec3 MousePos = CKeyMgr::GetInst()->GetRelativeMousePos();
	Vec3 ObjPos = Vec3(0.f, 0.f, 0.f);
	vector<CGameObject*> vecObj = {};

	string ObjName;


	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		vector<CGameObject*> vecCurObjects = pCurLevel->GetLayer(i)->GetParentObjects();
		for (UINT j = 0; j < vecCurObjects.size(); ++j)
		{
			if (vecCurObjects[j]->GetName() == L"Funiture" || vecCurObjects[j]->GetName() == L"Wall" || vecCurObjects[j]->GetName() == L"Door")
			{
				vecObj.push_back(vecCurObjects[j]);
			}
		}
	}

	// 해당하는 오브젝트들이 있다면
	if (0 < vecObj.size())
	{
		for (UINT i = 0; i < vecObj.size(); ++i)
		{
			ObjPos = vecObj[i]->Transform()->GetRelativePos();
			Vec3 ObjScale = vecObj[i]->Transform()->GetRelativeScale();
			if (KEY_TAP(KEY::LBTN))
			{

			}

			if (KEY_PRESSED(KEY::LBTN))
			{
				ObjName = string(vecObj[i]->GetName().begin(), vecObj[i]->GetName().end());
				if (ObjPos.x - ObjScale.x / 2.f < MousePos.x && MousePos.x < ObjPos.x + ObjScale.x / 2.f
					&& ObjPos.y - ObjScale.y / 2.f < MousePos.y && MousePos.y < ObjPos.y + ObjScale.y / 2.f)
				{
					vecObj[i]->Transform()->SetRelativePos(Vec3(MousePos.x, MousePos.y, ObjPos.z));
				}
			}

			if (KEY_RELEASED(KEY::LBTN))
			{
				vecObj[i]->Transform()->SetRelativePos(vecObj[i]->Transform()->GetRelativePos());
			}
		}


		ImGui::Text("Cur Obj");
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.3f, 1.0f, 1.0f));
		ImGui::Text(ObjName.c_str());
		ImGui::PopStyleColor();



		ImGui::Text("CurObjPos");
		ImGui::InputFloat3("##CurObjPos", ObjPos);
	}
}


void StageEditor::ShowPopup()
{
	if (m_bPopup)
	{
		ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_Always); // 가로 400, 세로 200
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::OpenPopup("Warning");
	}

	if (ImGui::BeginPopupModal("Warning", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		// 현재 창의 크기 가져오기
		ImVec2 window_size = ImGui::GetWindowSize();
		const char* text = "No Texture";
		ImVec2 text_size = ImGui::CalcTextSize(text);


		ImGui::SetCursorPosX((window_size.x - text_size.x) / 2);
		ImGui::SetCursorPosY((window_size.y - text_size.y) / 2);

		ImGui::Text("%s", text);
		ImVec2 button_size = ImVec2(40, 0);

		ImGui::SetCursorPosX((window_size.x - button_size.x) / 2);
		ImGui::SetCursorPosY(window_size.y - 30.f);
		if (ImGui::Button("OK", button_size))
		{
			m_bPopup = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}