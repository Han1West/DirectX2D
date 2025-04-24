#pragma once

#include "singleton.h"

#include <FontEngine/FW1FontWrapper.h>
#include <FontEngine/FW1CompileSettings.h>

#ifdef _DEBUG
#pragma comment(lib, "FontEngine/FW1FontWrapper_debug.lib")
#else
#pragma comment(lib, "FontEngine/FW1FontWrapper.lib"))
#endif

#define FONT_RGBA(r, g, b, a) (((((BYTE)a << 24 ) | (BYTE)b << 16) | (BYTE)g << 8) | (BYTE)r)


class CFontMgr :
    public singleton<CFontMgr>
{
    SINGLE(CFontMgr);
public:
    wstring m_str;

private:
    IFW1Factory* m_FW1Factory;
    IFW1FontWrapper* m_FontWrapper;
    IDWriteFactory* pDWriteFactory;
    IDWriteTextFormat* pTextFormat;

public:
    void Init();
    void Render();
    void DrawFont(const wchar_t* _pStr, float _fPosX, float _fPosY, float _fFontSize, UINT _Color);

    bool AddFontFromFile(const wstring _FontPath);

    void SetStr(wstring& _Str) { m_str = _Str; }
};

