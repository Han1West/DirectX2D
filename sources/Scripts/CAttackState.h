#pragma once
#include "CState.h"
class CAttackState :
    public CState
{
public:
    float m_AttStartDelay;
    float m_AttDelay;
    float m_AttCreateDelay;

    float m_DetectRange;

    bool m_AttStart;
    bool m_MeleeAttCreate;
    bool m_RangeAttCreate;
    bool m_FirstBullet;

public:
    virtual void Enter() override;
    virtual void FinalTick() override;
    virtual void Exit() override;

public:
    void MeleeAttack();
    void RangeAttack();

    void MakeMeleeAttack();
    void MakeRangerAttack(bool _IsShotgun, int _WeaponRounds);

public:
    CLONE(CAttackState);
    CAttackState();
    ~CAttackState();

};

