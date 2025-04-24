#pragma once
#include "CEntity.h"


class CComponent;
class CRenderComponent;
class CScript;

class CGameObject :
    public CEntity
{
private:
    CComponent*             m_arrCom[(UINT)COMPONENT_TYPE::END];
    CRenderComponent*       m_RenderCom;    // 1���� ���ӿ�����Ʈ ��ü�� 1���� ����������Ʈ�� ���� �� �ִ�.
    vector<CScript*>        m_vecScripts;   // ���� ��ũ��Ʈ��

    CGameObject*            m_Parent;       // �θ� ������Ʈ
    vector<CGameObject*>    m_vecChild;     // �ڽ� ������Ʈ��

    int                     m_LayerIdx;     // ������Ʈ�� �����ִ� ���̾� �ε��� ��ȣ, -1 : ���Ҽ�
    bool                    m_Dead;         // ������Ʈ�� ���°� ���� ���� ��������

public:
    void Begin();
    void Tick();
    void FinalTick();
    void Render();

public:
    void AddComponent(CComponent* _Component);
    void AddChild(CGameObject* _Child);
    void DeleteComponent(COMPONENT_TYPE _Type);
    void DeleteScirpt(wstring& _SciprtName);


    CGameObject* GetParent() { return m_Parent; }
    CComponent* GetComponent(COMPONENT_TYPE _Type) { return m_arrCom[(UINT)_Type]; }
    CRenderComponent* GetRenderComponent() { return m_RenderCom; }

    int GetLayerIdx() { return m_LayerIdx; }
    bool IsDead() { return m_Dead; }
    bool IsAncestor(CGameObject* _Other);

    const vector<CGameObject*>& GetChild() { return m_vecChild; }
    const vector<CScript*>& GetScripts() { return m_vecScripts; }

    class CTransform* Transform() { return (CTransform*)GetComponent(COMPONENT_TYPE::TRANSFORM); }
    class CMeshRender* MeshRender() { return (CMeshRender*)GetComponent(COMPONENT_TYPE::MESHRENDER); }
    class CCamera* Camera() { return (CCamera*)GetComponent(COMPONENT_TYPE::CAMERA); }
    class CCollider2D* Collider2D() { return (CCollider2D*)GetComponent(COMPONENT_TYPE::COLLIDER2D); }
    class CFlipbookPlayer* FlipbookPlayer() { return (CFlipbookPlayer*)GetComponent(COMPONENT_TYPE::FLIPBOOKPLAYER); }
    class CTileMap* TileMap() { return (CTileMap*)GetComponent(COMPONENT_TYPE::TILEMAP); }
    class CLight2D* Light2D() { return (CLight2D*)GetComponent(COMPONENT_TYPE::LIGHT2D); }
    class CParticleSystem* ParticleSystem() { return (CParticleSystem*)GetComponent(COMPONENT_TYPE::PARTICLE_SYSTEM); }

private:
    void DisconnectWithLayer();
    void DisconnecntWithParent();
    void RegisterAsParent();

public:
    CLONE(CGameObject);
    CGameObject();
    CGameObject(const CGameObject& _Origin);
    ~CGameObject();

    friend class CLayer;
    friend class CTaskMgr;
};

