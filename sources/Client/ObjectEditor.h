#pragma once
#include "EditorUI.h"

class CPrefab;
class CGameObject;

class ObjectEditor :
    public EditorUI
{
private:
    Ptr<CPrefab> m_Prefab;
    CGameObject* m_ProtoObject;

    char            m_Buffer[128];

public:
    virtual void Render_Update() override;

public:
    ObjectEditor();
    ~ObjectEditor();
};

