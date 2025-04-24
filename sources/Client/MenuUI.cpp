#include "pch.h"
#include "MenuUI.h"

#include <Engine/CPathMgr.h>
#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CRenderMgr.h>
#include <Engine/CAssetMgr.h>
#include <Engine/assets.h>

#include "CLevelSaveLoad.h"

#include "CImGuiMgr.h"
#include "Inspector.h"

MenuUI::MenuUI()
	: EditorUI("MenuUI")
{
}

MenuUI::~MenuUI()
{
}

void MenuUI::Render()
{
	if (!IsActive())
		return;

    if (ImGui::BeginMainMenuBar())
    {        
        File();

        Level();

        GameObject();

        Editor();

        Asset();

        ImGui::EndMainMenuBar();
    }
}

void MenuUI::File()
{
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("Level Save"))
        {
            // 파일 경로 문자열
            wchar_t szFilePath[255] = {};

            OPENFILENAME Desc = {};

            Desc.lStructSize = sizeof(OPENFILENAME);
            Desc.hwndOwner = nullptr;
            Desc.lpstrFile = szFilePath;
            Desc.nMaxFile = 255;
            Desc.lpstrFilter = L"Level\0*.lv\0ALL\0*.*";
            Desc.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
            strFilePath += L"Level";
            Desc.lpstrInitialDir = strFilePath.c_str();

            if (GetSaveFileName(&Desc))
            {
                if (wcslen(szFilePath) != 0)
                {
                    CLevel* CurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
                    CLevelSaveLoad::SaveLevel(szFilePath, CurLevel);
                }
            }
        }

        if (ImGui::MenuItem("Level Load"))
        {
            wstring m_SelectedFilePath;
            OPENFILENAME ofn;
            wchar_t filePath[MAX_PATH] = L"";

            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = NULL;
            ofn.lpstrFile = filePath;
            ofn.nMaxFile = sizeof(filePath);
            ofn.lpstrFilter = (LPWSTR)"모든 파일\0*.*\0텍스트 파일\0*.TXT\0";
            ofn.nFilterIndex = 1;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
            strFilePath += L"Level";
            ofn.lpstrInitialDir = strFilePath.c_str();

            if (GetOpenFileName(&ofn))
            {
                m_SelectedFilePath = filePath;
            }

            if (m_SelectedFilePath == L"")
                return;

            CLevel* pLoadedLevel = CLevelSaveLoad::LoadLevel(m_SelectedFilePath);
            ChangeLevel(pLoadedLevel, LEVEL_STATE::STOP);


            // 레벨이 로드될때 Inspector 에서 보여주던 정보를 전부 제거한다. (삭제된 객체를 가리키고 있을 수 있기 때문)
            Inspector* pInspector = (Inspector*)CImGuiMgr::GetInst()->FindUI("Inspector");
            pInspector->SetTargetObject(nullptr);
        }

        ImGui::EndMenu();
    }
}

void MenuUI::Level()
{   
    CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
    LEVEL_STATE State = LEVEL_STATE::NONE;

    if (nullptr != pCurLevel)
        State = pCurLevel->GetState();

    bool IsNotPlay = State != LEVEL_STATE::PLAY;
    bool IsPlay = !IsNotPlay;
    bool IsPause = State == LEVEL_STATE::PAUSE;

    if (ImGui::BeginMenu("Level"))
    {        
        if (ImGui::MenuItem("Play", nullptr, nullptr, IsNotPlay))
        {
            CLevelSaveLoad::SaveLevel(CPathMgr::GetInst()->GetContentPath() + L"Level\\Temp.lv", pCurLevel);

            ChangeLevelState(LEVEL_STATE::PLAY);
        }

        if (ImGui::MenuItem("Pause", nullptr, nullptr, IsPlay))
        {
            ChangeLevelState(LEVEL_STATE::PAUSE);
        }

        if (ImGui::MenuItem("Stop", nullptr, nullptr, IsPlay || IsPause))
        {
            // 레벨이 정지될때 Inspector 에서 보여주던 정보를 전부 제거한다. (삭제된 객체를 가리키고 있을 수 있기 때문)
            Inspector* pInspector = (Inspector*)CImGuiMgr::GetInst()->FindUI("Inspector");
            pInspector->SetTargetObject(nullptr);

            CLevel* pLevel = CLevelSaveLoad::LoadLevel(CPathMgr::GetInst()->GetContentPath() + L"Level\\Temp.lv");
            ChangeLevel(pLevel, LEVEL_STATE::STOP);
        }

        ImGui::EndMenu();
    }

}

#include "AddObjectUI.h"

void MenuUI::GameObject()
{
    if (ImGui::BeginMenu("GameObject"))
    {
        if (ImGui::MenuItem("Add Prefab"))
        {
            CImGuiMgr::GetInst()->FindUI("AddObjectUI")->SetActive(true);

        }
        ImGui::EndMenu();
    }
}

void MenuUI::Editor()
{
    if (ImGui::BeginMenu("Editor"))
    {
        if (ImGui::MenuItem("Add Prefab"))
        {
            CImGuiMgr::GetInst()->FindUI("PrefabEditor")->SetActive(true);
        }
        ImGui::EndMenu();
    }
}

void MenuUI::Asset()
{
    if (ImGui::BeginMenu("Asset"))
    {
        if (ImGui::MenuItem("Create Empty Material", nullptr, nullptr))
        {
            Ptr<CMaterial> pMtrl = new CMaterial;
            wstring MtrlKey = GetNextMaterialName();
            CAssetMgr::GetInst()->AddAsset(MtrlKey, pMtrl);
        }
        ImGui::EndMenu();
    }
}


wstring MenuUI::GetNextMaterialName()
{
    wchar_t Bufff[255] = {};

    UINT i = 0;
    while (true)
    {        
        wsprintf(Bufff, L"Material\\Default Material %d.mtrl", i++);

        if (nullptr == CAssetMgr::GetInst()->FindAsset<CMaterial>(Bufff))
            break;
    }

    return Bufff;
}

