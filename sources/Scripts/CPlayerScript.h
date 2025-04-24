#pragma once
#include <Engine/CScript.h>

class CPrefab;

enum class WEAPON_TYPE;

enum class PLAYER_STATE
{
    IDLE,
    WALK,
    ATTACK,
    ATTACK_DELAY,
    MOUNT,
    KILLING,
    AMBUSHING,
    KICKING,
    KILLBIKER,
    PAUSERELEASD,

    WAIT,
};

class CPlayerScript :
    public CScript
{
private:
    float   m_PlayerSpeed;
    float   m_AmbushingTime;
    float   m_GunSoundDurationTime;

    float   m_AttDelay;
    float   m_AttCreateDelay;
    float   m_BulletDelay;
    float   m_KillAttackDelay;
    float   m_FadeTime;

    Ptr<CTexture>   m_TargetTex;
    Ptr<CPrefab>    m_Prefab;    

    int             m_ActionedKey;
    int             m_KillingCount;
    int             m_CurWeaponRounds;
    int             m_AdditionRounds;

    bool            m_EquipGun;
    bool            m_FirstBullet;
    bool            m_IsMaskOn;
    bool            m_bMakeGunSound;
    bool            m_bFade;
    bool            m_bMakeFade;
    bool            m_bAttackSound;
    bool            m_bKilling;

    bool            m_bPause;
    bool            m_bDead;


    WEAPON_TYPE     m_CurWeapon;
    PLAYER_STATE    m_State;
    MASK_TYPE       m_Mask;

    CGameObject*    m_TargetEnemy;
    CGameObject*    m_Cursor;

    vector<Vec3>    m_CollisionDir;
    vector<tTileGridCell> m_vecTileGrid;
    vector<bool>          m_vecKeyCheck;

public:
    virtual void Begin() override;
    virtual void Tick() override;
    virtual void BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;
    
public:
    WEAPON_TYPE GetPlayerWeaponType() { return m_CurWeapon; }

    void SetMask(MASK_TYPE _Mask) { m_Mask = _Mask; }
    void SetWeapon(WEAPON_TYPE _Weapon) { m_CurWeapon = _Weapon; }
    void SetRounds(int _Rounds) { m_CurWeaponRounds = _Rounds; }
    void SetEquipGun(bool _Gun) { m_EquipGun = _Gun; }
    void SetFade(bool _Fade) { m_bFade = _Fade; m_bMakeFade = false; }
    void SetActionKey(int _Key) { m_ActionedKey = _Key; }
    void SetPause(bool _Pause) { m_bPause = _Pause; }
    void SetState(PLAYER_STATE _State) { m_State = _State; }

    MASK_TYPE GetMask() { return m_Mask; }
    int GetCurRounds() { return m_CurWeaponRounds; }
    int GetCurrentKillingCount() { return m_KillingCount; }
    int GetCurrentActionedKey() { return m_ActionedKey; }

    bool IsEquippedGun() { return m_EquipGun; }

    vector<tTileGridCell> GetTileGrid() { return m_vecTileGrid; }
    bool IsMakeGunSound() { return m_bMakeGunSound; }

    bool IsPlayerDead() { return m_bDead; }
    bool IsPaued() { return m_bPause; }

private:
    void EquipWeapon(CGameObject* _Weapon);

    void PlayIdleFlip();
    void PlayWalkFlip();
    void PlayAttackFlip();
    void PlayMountFlip(WEAPON_TYPE _Weapon);
    void PlayKillFlip(int _Idx);
    void PlayDeadFlip();
   

private:
    void PlayerMove(Vec3 _Pos);
    void PlayerAttack();
    void PlayerThrow();
    void PlayerMount(CGameObject* _Enemy);
    void PlayerKilling(WEAPON_TYPE _WeaponType);

    void MakeBodyAfterKill();
    void KeyCheck();

    void MakeMeleeAttack();
    void MakeRangeAttack(bool _IsShotgun);

private:
    vector<Vec3> CalcWallVector(Vec3 _PlayerPos, Vec3 _WallPos, Vec3 _WallScale, Vec3 _WallRotation);
    void RegisterMask();
    void AdjustMask();

    void RestartCurFloor();
    void ActivePause();

public:
    CLONE(CPlayerScript);
    CPlayerScript();
    ~CPlayerScript();
};

