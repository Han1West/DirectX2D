#pragma once
#include <Engine/CScript.h>

class CCameraScript :
    public CScript
{
private:
    float   m_CameraSpeed;
    float   m_ShakeTime;

    Vec3   m_MaximamPos;
    float  m_DirX;
    float  m_DirY;

    bool   m_bSearch;
    bool   m_bShake;

public:
    virtual void Tick() override;
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;
public:
    void SetShake(bool _Shake) { m_bShake = _Shake; }

private:
    void CameraOrthgraphicMove();
    void CameraPerspectiveMove();

    void UpdateShake();
    void OffShake();
public:
    CLONE(CCameraScript);
    CCameraScript();
    ~CCameraScript();
};

