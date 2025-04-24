#pragma once
#include "CAsset.h"

// png, jpg, jpeg, tga, dds, 

class CTexture :
    public CAsset
{
private:
    ScratchImage                        m_Image; // 이미지 파일 로딩 및 저장기능
    ComPtr<ID3D11Texture2D>             m_Tex2D; // (ScratchImage)SysMem -> GPUMem  

    ComPtr<ID3D11RenderTargetView>      m_RTV; 
    ComPtr<ID3D11DepthStencilView>      m_DSV;
    ComPtr<ID3D11ShaderResourceView>    m_SRV;
    ComPtr<ID3D11UnorderedAccessView>   m_UAV;

    D3D11_TEXTURE2D_DESC                m_Desc;

    int                                 m_RecentSRVNum;
    int                                 m_RecentUAVNum;

public:
    UINT GetWidth() { return m_Desc.Width; }
	UINT GetHeight() { return m_Desc.Height; }

    tPixel* GetPixels() { return (tPixel*)m_Image.GetPixels(); }

    ComPtr<ID3D11Texture2D> GetTex2D() { return m_Tex2D; }
	ComPtr<ID3D11RenderTargetView> GetRTV()   { return m_RTV; }
    ComPtr<ID3D11DepthStencilView> GetDSV()   { return m_DSV; }
    ComPtr<ID3D11ShaderResourceView> GetSRV() { return m_SRV; }

public:
    void Binding(int _RegisterNum);
    static void Clear(int _RegisterNum);

    void Binding_SRV_CS(int _RegisterNum);
    void Binding_UAV_CS(int _RegisterNum);

    void Clear_SRV_CS();
    void Clear_UAV_CS();

private:
    virtual int Load(const wstring& _FilePath) override;
    virtual int Save(const wstring& _FilePath) override { return S_OK; }

    int Create(UINT _Width, UINT _Height, DXGI_FORMAT _PixelFormat, UINT _BindFlag, D3D11_USAGE _Usage = D3D11_USAGE_DEFAULT);
    int Create(ComPtr<ID3D11Texture2D> _Tex2D);

public:
    CLONE_DISABLE(CTexture);
    CTexture();
    ~CTexture();

    friend class CAssetMgr;
};

