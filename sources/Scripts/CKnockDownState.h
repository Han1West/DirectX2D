#pragma once
#include "CState.h"

class CKnockDownState :
    public CState
{
public:
    float m_RecoverTime;
    float m_RecoveringTime;
    float m_KnockBackTime;

    bool m_IsMount;

    Vec2 m_Dir;

public:
    void SetMount(bool _Mount) { m_IsMount = _Mount; }

public:
    virtual void Enter() override;
    virtual void FinalTick() override;
    virtual void Exit() override;

public:
    CLONE(CKnockDownState);
    CKnockDownState();
    ~CKnockDownState();

};

