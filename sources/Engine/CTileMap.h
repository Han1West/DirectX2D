#pragma once
#include "CRenderComponent.h"

#include "CTexture.h"


struct tTileInfo
{
    Vec2 LeftTopUV;
    Vec2 SliceUV;
};


class CStructuredBuffer;

class CTileMap :
    public CRenderComponent
{
private:
    // Ÿ�� ��� ����
    int                 m_Col;          
    int                 m_Row;

    // Ÿ�� 1���� ũ��
    float               m_TileSizeX;
    float               m_TileSizeY;

    // Ÿ�� ��Ʋ��
    Ptr<CTexture>       m_TileAtlas;

    // ��Ʋ�� �ؽ��Ŀ��� Ÿ�� 1���� �����ϴ� ũ��(Pixel)
    UINT                m_TileAtlasSizeX;
    UINT                m_TileAtlasSizeY;

    // ��Ʋ�� �ؽ��Ŀ��� Ÿ���� ���� ���� ��� ����
    UINT                m_TileAtlasCol;
    UINT                m_TileAtlasRow;


    vector<tTileInfo>   m_vecTileInfo;  // Ÿ�� ������  
    CStructuredBuffer*  m_Buffer;       // Ÿ�� �����͸� GPU �� ������ �� ��� �� ����


public:
    void SetTileColRow(UINT _Col, UINT _Row);
    void SetTileSize(float _TileSizeX, float _TileSizeY);
    void SetAtlasTexture(Ptr<CTexture> _Tex) { m_TileAtlas = _Tex; }
    void SetTileAtlasSize(UINT _X, UINT _Y);
    void SetTileAtlasColRow(UINT _Col, UINT _Row);

    void SetTileInfo(Vec2 _LeftTop, Vec2 _Slice, int _Idx);

    vector<tTileInfo> GetTileInfo() { return m_vecTileInfo; }

    Vec2 GetTileColRow() { return Vec2(m_Col, m_Row); }
    Vec2 GetTileSize() { return Vec2(m_TileSizeX, m_TileSizeY); }
    Vec2 GetTileAtlasSize() { return Vec2(m_TileAtlasSizeX, m_TileAtlasSizeY); }
    Vec2 GetTileAtlasColRow() { return Vec2(m_TileAtlasCol, m_TileAtlasRow); }

    Ptr<CTexture> GetAtlasTexutre() { return m_TileAtlas; }


private:
    void CalcObjectScale();

public:
    virtual void Init() override; 
    virtual void FinalTick() override {};
    virtual void Render() override;
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _FILE) override;

public:
    CLONE(CTileMap);
    CTileMap();
    CTileMap(const CTileMap& _Origin);
    ~CTileMap();
};

