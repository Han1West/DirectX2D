#include "pch.h"
#include "CFontMgr.h"

#include "CDevice.h"
#include "CPathMgr.h"

CFontMgr::CFontMgr()
	: m_FW1Factory(nullptr)
	, m_FontWrapper(nullptr)
{
}

CFontMgr::~CFontMgr()
{
	if (nullptr != m_FW1Factory)
		m_FW1Factory->Release();

	if (nullptr != m_FontWrapper)
		m_FontWrapper->Release();
}

void CFontMgr::Init()
{
	wstring ContentPath = CPathMgr::GetInst()->GetContentPath();

	//AddFontFromFile(ContentPath + L"Font\\Retro Computer_DEMO.ttf");

	if (FAILED(FW1CreateFactory(FW1_VERSION, &m_FW1Factory)))
	{
		assert(nullptr);
	}

	if (FAILED(m_FW1Factory->CreateFontWrapper(DEVICE, L"Retro Computer", &m_FontWrapper)))
	{
		assert(nullptr);
	}

}

void CFontMgr::Render()
{
	DrawFont(m_str.c_str(), 65, 560, 45, FONT_RGBA(106, 193, 239, 255));
}

void CFontMgr::DrawFont(const wchar_t* _pStr, float _fPosX, float _fPosY, float _fFontSize, UINT _Color)
{
	m_FontWrapper->DrawString(
		CONTEXT,
		_pStr,				// String
		_fFontSize,			// Font size
		_fPosX,				// X position
		_fPosY,				// Y position
		_Color,				// Text color, 0xAaBbGgRr
		FW1_RESTORESTATE    // Flags (for example FW1_RESTORESTATE to keep context states unchanged)
	);
}

bool CFontMgr::AddFontFromFile(const wstring _FontPath)
{
	int result = AddFontResourceEx(_FontPath.c_str(), FR_PRIVATE, 0);
	SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
	return result > 0;
}
