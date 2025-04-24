#pragma once
#include <Engine/CScript.h>

class CState;

class CFatEnemyScript :
    public CScript
{
private:
    Vec3    m_SpawnPos;
    Vec3    m_DestPos;

    CState* m_CurState;
    STATE_TYPE m_StartState;

    float m_AccTime;
    float m_BleedTime;
    float m_SpawnTime;

    int m_HP;
    bool m_Bleeding;
    bool m_DeadBleeding;
    bool m_MakePool;
    bool m_bDead;
    

public:
    void SetState(CState* _State) { m_CurState = _State; }
    void SetDestination(Vec3 _Pos) { m_DestPos = _Pos; }
    
    STATE_TYPE GetStateType() {
        if (m_CurState == nullptr)
            return STATE_TYPE::NONE;

        return m_CurState->GetStateType();
    }

    CState* GetState() { return m_CurState; }

    Vec3 GetSpawnPos() { return m_SpawnPos; }
    Vec3 GetDestPos() { return m_DestPos; }

    STATE_TYPE GetStartState() { return m_StartState; }

    bool IsDead() { return m_bDead; }

public:
    virtual void Begin() override;
    virtual void Tick() override;
    virtual void BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;

public:
    void PlayIdleFlip();
    void PlayWalkFlip();
    void PlayAttackFlip();
    void PlaySearchFlip();
    void PlayDyingFlip();
    void PlayDeadFlip(int _Idx);

public:
    void MakeBlood();
    void MakePool();

public:
    CLONE(CFatEnemyScript);
    CFatEnemyScript();
    ~CFatEnemyScript();
};

