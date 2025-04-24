#pragma once
#include "EditorUI.h"

class CPrefab;

class PF_View;

class PrefabEditor :
    public EditorUI
{
private:
    Ptr<CPrefab> m_Prefab;
    CGameObject* m_ProtoObject;

    char            m_Buffer[128];

    PF_View* m_View;

public:
    virtual void Render_Update() override;
    void Init();

public:
    PF_View* GetView() { return m_View; }

    void IsRender();

private:
    void SelectComponent(DWORD_PTR _ListUI, DWORD_PTR _SelectString);
    void DeleteComponent(DWORD_PTR _ListUI, DWORD_PTR _SelectString);
    

public:
    PrefabEditor();
    ~PrefabEditor();

    friend class ObjectEditor;
};

