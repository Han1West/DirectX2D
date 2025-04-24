#pragma once
#include "EditorUI.h"

class ContentUI :
    public EditorUI
{
private:
    class TreeUI* m_Tree;
    vector<wstring> m_vecAssetPath; // Content 폴더에 있는 모든 리소스 경로
    
public:
    virtual void Render_Update() override;
    void Reset();

private:
    void RenewContent();
    void ReloadContent();
    void FindAssetPath(const wstring& _FolderPath);

    void SelectAsset(DWORD_PTR _TreeNode);
    ASSET_TYPE GetAssetType(const wstring& _Path);

public:
    ContentUI();
    ~ContentUI();
};

