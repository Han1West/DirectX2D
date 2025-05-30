#include "pch.h"
#include "CImGuiMgr.h"

#include <Engine/CEngine.h>
#include <Engine/CDevice.h>
#include <Engine/CKeyMgr.h>
#include <Engine/CLevelMgr.h>
#include <Engine/CPathMgr.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"


#include "Inspector.h"
#include "ListUI.h"
#include "MenuUI.h"
#include "ContentUI.h"
#include "Outliner.h"
#include "FlipbookEditor.h"
#include "SpriteEditor.h"
#include "SE_AtlasView.h"
#include "SE_Detail.h"
#include "PrefabEditor.h"
#include "StageEditor.h"
#include "AddObjectUI.h"
#include "PF_View.h"
#include "SE_PreloadView.h"

#include "EditorUI.h"

CImGuiMgr::CImGuiMgr()
    : m_hNotify(nullptr)
{

}

CImGuiMgr::~CImGuiMgr()
{    
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();  

    DeleteMap(m_mapUI);
}

int CImGuiMgr::Init()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;
    //io.ConfigViewportsNoDefaultParent = true;
    //io.ConfigDockingAlwaysTabBar = true;
    //io.ConfigDockingTransparentPayload = true;
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(CEngine::GetInst()->GetMainWnd());
    ImGui_ImplDX11_Init(DEVICE, CONTEXT);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);
      
    // Editor 용도 UI 생성
    CreateEditorUI();

    // Content 폴더에 발생하는 변경점을 감시하는 커널 객체 생성
    wstring ContentPath = CPathMgr::GetInst()->GetContentPath();

    m_hNotify = FindFirstChangeNotification(ContentPath.c_str(), true
        , FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME
        | FILE_ACTION_ADDED | FILE_ACTION_REMOVED);

    return S_OK;
}

void CImGuiMgr::Progress()
{
    Tick();

    Render();
    
    ObserveContent();
}


void CImGuiMgr::Tick()
{
    for (const auto& pair : m_mapUI)
    {
        pair.second->Tick();
    }
}

void CImGuiMgr::Render()
{
    // Render Start
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Demo    
    ImGui::ShowDemoWindow();

    for (const auto& pair : m_mapUI)
    {
        pair.second->Render();
    }

    // 화면 내부 UI 들 렌더링
    ImGui::Render();   
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // 화면 외부로 벗어난 별도의 윈도우에 그려지는 UI 들 렌더링
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

EditorUI* CImGuiMgr::FindUI(const string& _ID)
{
    map<string, EditorUI*>::iterator iter = m_mapUI.find(_ID);

    if (iter == m_mapUI.end())
        return nullptr;

    return iter->second;
}


void CImGuiMgr::ObserveContent()
{
    // 알림이 온게 있는지 확인하는 함수
    DWORD dwWaitStatus = WaitForSingleObject(m_hNotify, 0);

    // 알림이 왔음
    if (dwWaitStatus == WAIT_OBJECT_0)
    {
        ContentUI* pUI = (ContentUI*)FindUI("Content");
        pUI->Reset();
        
        FindNextChangeNotification(m_hNotify);
    }
    
}


void CImGuiMgr::CreateEditorUI()
{
    EditorUI* pUI = nullptr;
     
    // Inspector
    pUI = new Inspector;
    m_mapUI.insert(make_pair(pUI->GetID(), pUI));
    //((Inspector*)pUI)->SetTargetObject(CLevelMgr::GetInst()->FindObjectByName(L"Player"));

    // ListUI
    pUI = new ListUI;
    pUI->SetActive(false);
    pUI->SetModal(true);
    m_mapUI.insert(make_pair(pUI->GetID(), pUI));

    // MenuUI
    pUI = new MenuUI;
    pUI->SetActive(true);    
    m_mapUI.insert(make_pair(pUI->GetID(), pUI));

    // Outliner
    pUI = new Outliner;
    pUI->SetActive(true);
    m_mapUI.insert(make_pair(pUI->GetID(), pUI));

    // ContentUI
    pUI = new ContentUI;
    pUI->SetActive(true);
    m_mapUI.insert(make_pair(pUI->GetID(), pUI));

    // SE_AtlasView
    SE_AtlasView* pSE_AtlasView = new SE_AtlasView;
    pSE_AtlasView->SetActive(false);
    m_mapUI.insert(make_pair(pSE_AtlasView->GetID(), pSE_AtlasView));

    // SE_Detail
    SE_Detail* pSE_Detail = new SE_Detail;
    pSE_Detail->SetActive(false);
    m_mapUI.insert(make_pair(pSE_Detail->GetID(), pSE_Detail));

    // SpriteEditor
    SpriteEditor* pEditorUI = new SpriteEditor;
    pEditorUI->Init();
    pEditorUI->SetActive(false);
    m_mapUI.insert(make_pair(pEditorUI->GetID(), pEditorUI));

    // FlipbookEditor
    pUI = new FlipbookEditor;
    pUI->SetActive(false);
    m_mapUI.insert(make_pair(pUI->GetID(), pUI));

    // PF_View
    pUI = new PF_View;
    pUI->SetActive(false);
    m_mapUI.insert(make_pair(pUI->GetID(), pUI));

    // PrefabEditor
    PrefabEditor* pPrefabUI = new PrefabEditor;
    pPrefabUI->Init();
    pPrefabUI->SetActive(false);
    m_mapUI.insert(make_pair(pPrefabUI->GetID(), pPrefabUI));

    // SE_PreloadView
    pUI = new SE_PreloadView;
    pUI->SetActive(false);
    m_mapUI.insert(make_pair(pUI->GetID(), pUI));

    // StageEditor
    StageEditor* pSE = new StageEditor;
    pSE->Init();
    pSE->SetActive(false);
    m_mapUI.insert(make_pair(pSE->GetID(), pSE));

    // ObjectAddUI
    pUI = new AddObjectUI;
    pUI->SetActive(false);
    m_mapUI.insert(make_pair(pUI->GetID(), pUI));
}

