#pragma once
#include "EditorUI.h"
#include "PrefabEditor.h"


class PF_Sub :
    public EditorUI
{
private:
    PrefabEditor* m_Owner;

public:
    PrefabEditor* GetOwner() { return m_Owner; }
    class PF_View* GetView() { return m_Owner->GetView(); }
    


public:
    PF_Sub(const string& _ID);
    ~PF_Sub();

    friend class PrefabEditor;
};

