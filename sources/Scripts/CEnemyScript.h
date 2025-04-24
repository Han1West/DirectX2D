#pragma once
#include <Engine/CScript.h>


class CState;

class CEnemyScript :
    public CScript
{
private:
    CState*         m_CurState;
    STATE_TYPE      m_StartState;

    Vec3            m_SpawnPos;
    Vec3            m_DestPos;

    int             m_CurWeaponInt;
    WEAPON_TYPE     m_CurWeaponType;
    int             m_CurWeaponRounds;

    float   m_SpawnTime;

    bool            m_EquipGun;
    bool            m_DeadByDoor;
    bool            m_Bleeding;
    bool            m_MakePool;
    bool            m_IsMounted;
    bool            m_bGiveup;
    bool            m_bDead;
    bool            m_bPlayerMakeBody;


public:
    virtual void Begin() override;
    virtual void Tick() override;
    virtual void BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;

public:
    void SetState(CState* _State) { m_CurState = _State; }
    void SetWeapon(int _Weapon); 
    void SetMount(bool _Mount) { m_IsMounted = _Mount; }
    void SetDestination(Vec3 _Pos) { m_DestPos = _Pos; }
    void SetWeaponRounds(int _Rounds) { m_CurWeaponRounds = _Rounds; }
    void SetGiveup(bool _Giveup) { m_bGiveup = _Giveup; }
    void SetPlayerMake(bool _Make) { m_bPlayerMakeBody = _Make; }

    STATE_TYPE GetStateType() { 
        if (m_CurState == nullptr)
            return STATE_TYPE::NONE;

        return m_CurState->GetStateType(); }
    WEAPON_TYPE GetWeaponType() { return m_CurWeaponType; }
    CState* GetState() { return m_CurState; }

    bool IsDeadByDoor() { return m_DeadByDoor; }
    bool IsEquippedGun() { return m_EquipGun; }
    bool IsGivenUP() { return m_bGiveup; }
    bool IsDead() { return m_bDead; }
    
    int GetCurWeaponRounds() { return m_CurWeaponRounds; }

    Vec3 GetSpawnPos() { return m_SpawnPos; }
    Vec3 GetDestPos() { return m_DestPos; }

    STATE_TYPE GetStartState() { return m_StartState; }

public:
    void EquipWeapon(CGameObject* _Weapon);

    void PlayIdleFlip();
    void PlayWalkFlip();
    void PlayAttackFlip();
    void PlaySearchFlip();
    void PlaySpecialFlip(int _Idx); // 0 : GetUp, 1 : KnockDown, 2 : LeanDown, 3 : LeanUp , 4: GiveUP
    void PlayDyingFlip(int _Idx);
    void PlayDeadFlip(int _Idx);

    void DropWeapon();

    void MakeBlood();
    void MakePool();

private:
    float CalcWallVecPos(Vec3 _WallPos, Vec3 _WallScale, Vec3 _WallRotation);
    
public:
    CLONE(CEnemyScript);
    CEnemyScript();
    ~CEnemyScript();
};

