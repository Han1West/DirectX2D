#pragma once
#include <Engine/CScript.h>

class CEditorCamScript :
    public CScript
{
private:
    float   m_CameraSpeed;

public:
    virtual void Tick() override;

private:
    void CameraOrthgraphicMove();
    void CameraPerspectiveMove();

    virtual void SaveComponent(FILE* _File) override {}
    virtual void LoadComponent(FILE* _FILE) override {}

public:
    CLONE_DISABLE(CEditorCamScript);
    CEditorCamScript();
    ~CEditorCamScript();
};

