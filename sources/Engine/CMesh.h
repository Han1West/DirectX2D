#pragma once
#include "CAsset.h"

class CMesh :
    public CAsset
{
private:
    ComPtr<ID3D11Buffer>		m_VB;
    ComPtr<ID3D11Buffer>		m_IB;

    Vtx*                        m_VtxSysMem;
    UINT*                       m_IdxSysMem;

    D3D11_BUFFER_DESC           m_VBDesc;
    UINT                        m_VtxCount;

    D3D11_BUFFER_DESC           m_IBDesc;
    UINT                        m_IdxCount;

private:
    virtual int Load(const wstring& _FilePath) override { return S_OK; }
    virtual int Save(const wstring& _FilePath) override { return S_OK; }

public:
    int Create(Vtx* _pVtx, UINT _VtxCount, UINT* _pIdx, UINT _IdxCount);
    void Binding();

    void Render();
    void Render_Particle(int _Count);

public:
    CLONE_DISABLE(CMesh);   
    CMesh(bool _bEngineRes = false);
    ~CMesh();
};

