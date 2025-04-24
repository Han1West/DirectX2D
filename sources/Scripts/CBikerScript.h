#pragma once
#include <Engine/CScript.h>

enum class BIKER_STATE
{
    COMPUTER,
    IDLE,
    WALK,
    ATTACK,
    KILL,
    THROW,
    PRY,
    DOWN,
    GETUP,
    CRAWL,
    BASH,
};

class CSound;

class CBikerScript :
    public CScript
{
private:
    Ptr<CSound> m_BossBGM;

    Vec3        m_PlayerPos;
    Vec3        m_PlayerRot;
    Vec3        m_BikerPos;
    Vec3        m_BikerRot;
    Vec2        m_DirToPlayer;

    BIKER_STATE m_State;

    float       m_Speed;
    float       m_DetectRange;
    float       m_KillRange;
    
    float       m_AttackDelay;
    float       m_AccTime;

    int         m_AttackCount;
    int         m_DownCount;

    bool        m_bEquipWeapon;
    bool        m_bStart;
    bool        m_bPlayFlip;
    bool        m_CanBash;
    bool        m_bDead;
    bool        m_bBGMChange;
    bool        m_bSound;
    bool        m_bDrawSound;

    bool        m_XBlock;
    bool        m_YBlock;

    bool        m_CrawlActiveScript;
    bool        m_ActiveScript;
    bool        m_ScriptPPActive;
    bool        m_StartSciptActive;
    int         m_ScriptCount;

public:
    virtual void Begin() override;
    virtual void Tick() override;
    virtual void BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;

public:
    void Kill();
    void Attack();
    void WalkTo();
    void Pry();
    void GetUp();
    void Down();
    void Crawl();
    void Bash();

    void ChangeState(BIKER_STATE _State);

private:
    void MakeMeleeAttack();
    void ThrowWeapon();

    Vec2 GetCrawlDir();

    void MakeBlood();
    void MakePool();

private:
    void PlayComputeFlip();
    void PlayWalkFlip(bool _Equip);
    void PlayAttackFlip(int _Idx);
    void PlayGetUpFlip();
    void PlayPryFlip();
    void PlayDownFlip(int _Idx);
    void PlayBashFlip();
    void PlayIdleFlip();
    void PlayCrawlFlip(int _Idx);

private:
    void PrintScript(int _Idx);

public:
    bool CanBash() { return m_CanBash; }
    bool IsActiveScript() { return m_ActiveScript; }

public:
    CLONE(CBikerScript);
    CBikerScript();
    ~CBikerScript();
};

