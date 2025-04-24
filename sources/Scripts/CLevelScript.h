#pragma once
#include <Engine/CScript.h>

struct tTempInfo
{
    WEAPON_TYPE CurWeapon;
    MASK_TYPE   CurMask;
    bool        EquippedGun;
    int         CurWeaponRounds;
};

class CSound;

class CLevelScript :
    public CScript
{
private:
    Ptr<CSound> m_BGM;

    wstring m_RelativePath;
    wstring m_NextLevelPath;
    wstring m_PrevLevelPath;

    int m_CurFloor;
    int m_StageRound;
    int m_BgmPosition;
    int m_CurBGMIdx;

    bool m_IsStart;
    bool m_IsFinal;
    bool m_bFloorClear;
    bool m_bLevelClear;
    bool m_bHome;
    bool m_bAlreadyClear;

    tTempInfo m_TempInfo;

public:
    virtual void Begin() override;
    virtual void Tick() override;
    virtual void BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;

public:
    void SetFloor(int _Floor) { m_CurFloor = _Floor; }
    void SetStart(bool _Start) { m_IsStart = _Start; }
    void SetFinal(bool _Final) { m_IsFinal = _Final; }
    void SetNextLevelPath(wstring _Path) { m_NextLevelPath = _Path; }
    void SetPrevLevelPath(wstring _Path) { m_PrevLevelPath = _Path; }
    void SetLevelEnd(bool _End) { m_bLevelClear = _End; }
    void SetHome(bool _Home) { m_bHome = _Home; }
    void SetStageRound(int _Round) { m_StageRound = _Round; }
    void SetBGM(Ptr<CSound> _BGM) { m_BGM = _BGM; }
    void SetAlreadyClear(bool _Clear) { m_bAlreadyClear = _Clear; }
    void SetBGMIdx(int _Idx) { m_CurBGMIdx = _Idx; }

    int GetFloor() { return m_CurFloor; }
    int GetStageRound() { return m_StageRound; }
    int GetBGMIdx() { return m_CurBGMIdx; }
    bool IsStart() { return m_IsStart; }
    bool IsFinal() { return m_IsFinal; }
    bool IsFloorClear() { return m_bFloorClear; }
    bool IsLevelClear() { return m_bLevelClear; }
    bool IsHome() { return m_bHome; }
    bool IsAlreadyClear() { return m_bAlreadyClear; }
    wstring GetRealtivePath() { return m_RelativePath; }
    wstring GetNextLevelPath() { return m_NextLevelPath; }
    wstring GetPrevLevelPath() { return m_PrevLevelPath; }

    Ptr<CSound> GetBGM() { return m_BGM; }
    tTempInfo GetTempInfo() { return m_TempInfo; }

public:
    void CheckFloorClear();
    void SaveTempLevel();

public:
    CLONE(CLevelScript);
    CLevelScript();
    ~CLevelScript();
};

