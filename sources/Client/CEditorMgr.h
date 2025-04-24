#pragma once
#include <Engine\singleton.h>

class CGameObjectEx;

class CEditorMgr :
    public singleton<CEditorMgr>
{
    SINGLE(CEditorMgr);
private:
    vector<CGameObjectEx*>    m_vecEditorObj;

public:
    void Init();
    void Progress();
};

