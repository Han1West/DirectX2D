#pragma once
#include "singleton.h"

class CLevel;

class CLevelMgr :
    public singleton<CLevelMgr>
{
    SINGLE(CLevelMgr);
private:
    CLevel*     m_CurLevel;
    bool        m_IsEdit;


public:
    CLevel* GetCurrentLevel() { return m_CurLevel; }
    void RegisterObject(class CGameObject* _Object);
    CGameObject* FindObjectByName(const wstring& _Name);
    void ChangeLevelState(LEVEL_STATE _NextState);
    void ChangeLevel(CLevel* _NextLevel, LEVEL_STATE _NextLevelState);
    
    void SetEdit(bool _Edit) { m_IsEdit = _Edit; }
    bool IsEdit() { return m_IsEdit; }

private:
    void SetCursorVisiblilty(bool _visible);

public:
    void Init();
    void Progress();    

    friend class CTaskMgr;
};

