#pragma once
#include "ComponentUI.h"

class CEnemyScript;
class CMaskScript;
class CFatEnemyScript;
class CLevelScript;
class CChangeFloorScript;

class ScriptUI :
    public ComponentUI
{
private:
    CScript* m_TargetScript;
    bool    m_IsEnemy;
    bool    m_IsFatEnemy;
    bool    m_IsMask;
    bool    m_IsLevel;
    bool    m_IsChangeFloor;

    int	m_StateIdx;
    int	m_WeaponIdx;
    int m_MaskIdx;

    char m_BufferNext[128];
    char m_BufferPrev[128];

    vector<string> m_Weapons;
    vector<string> m_Masks;

    CEnemyScript* m_EnemyScript;
    CFatEnemyScript* m_FatEnemyScript;
    CMaskScript* m_MaskScript;
    CLevelScript* m_LevelScript;
    CChangeFloorScript* m_ChangeFloorScript;

public:
    void SetScript(CScript* _Script);

    void SetWeaponUI();
    void SetStateUI();
    void FindEnemyScript();

    void SetMaskUI();
    void FindMaskScript();

    void SetLevelUI();
    void SetNextLevelUI();
    void FindLevelScript();

    void SetbNextUI();
    void FindChangeFloorScript();

public:
    virtual void Render_Update() override;

public:
    ScriptUI();
    ~ScriptUI();
};

