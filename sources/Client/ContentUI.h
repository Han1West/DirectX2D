#pragma once
#include "EditorUI.h"

class ContentUI :
    public EditorUI
{
private:
    class TreeUI* m_Tree;
    vector<wstring> m_vecAssetPath; // Content ������ �ִ� ��� ���ҽ� ���
    
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

