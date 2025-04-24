#pragma once
#include "CShader.h"


class CGraphicShader :
    public CShader
{
private:  
    ComPtr<ID3DBlob>			    m_VSBlob;
    ComPtr<ID3DBlob>			    m_GSBlob;
    ComPtr<ID3DBlob>			    m_PSBlob;

    ComPtr<ID3D11VertexShader>	    m_VS;
    ComPtr<ID3D11GeometryShader>	m_GS;
    ComPtr<ID3D11PixelShader>	    m_PS;
    
    ComPtr<ID3D11InputLayout>	    m_Layout;
    D3D11_PRIMITIVE_TOPOLOGY        m_Topology;

    RS_TYPE                         m_RSType;   // 레스터라이져 스테이트 옵션
    BS_TYPE                         m_BSType;   // 블랜드 스테이트 옵션
    DS_TYPE                         m_DSType;   // DepthStencilState 옵션


    SHADER_DOMAIN                   m_Domain;



public:
    int CreateVertexShader(const wstring& _RelativePath, const string& _FuncName);
    int CreateGeometryShader(const wstring& _RelativePath, const string& _FuncName);
    int CreatePixelShader(const wstring& _RelativePath, const string& _FuncName);

public:
    void SetTopology(D3D11_PRIMITIVE_TOPOLOGY _Topology) { m_Topology = _Topology; }
    void SetRSState(RS_TYPE _Type) { m_RSType = _Type; }
    void SetBSState(BS_TYPE _Type) { m_BSType = _Type; }
    void SetDSState(DS_TYPE _Type) { m_DSType = _Type; }

    void SetDomain(SHADER_DOMAIN _Domain) { m_Domain = _Domain; }
    SHADER_DOMAIN GetDomain() { return m_Domain; }

    void Binding();

public:
    CLONE_DISABLE(CGraphicShader);
    CGraphicShader();
    ~CGraphicShader();
};

