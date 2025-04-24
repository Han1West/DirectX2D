#pragma once
#include "EditorUI.h"

class TreeNode;
class CGameObject;

class Outliner :
    public EditorUI
{
private:
    class TreeUI* m_Tree;
    bool    m_bShowPopup;

public:
    virtual void Render_Update() override;

private:
    void RenewGameObject();
    void AddGameObject(TreeNode* _ParentNode, CGameObject* _Object);

    void SelectGameObject(DWORD_PTR _TreeNode);
    void DeleteNodeObject(DWORD_PTR _TreeNode);    
    void DragDrop(DWORD_PTR _DragNode, DWORD_PTR _DropNode);

public:
    Outliner();
    ~Outliner();
};

