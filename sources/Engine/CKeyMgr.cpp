#include "pch.h"
#include "CKeyMgr.h"

#include "CEngine.h"

int g_arrKeyValue[(int)KEY::END]
=
{
	'W',
	'S',
	'A',
	'D',
	'Z',
	'Y',
	'R',

	VK_LEFT,
	VK_RIGHT,
	VK_UP,
	VK_DOWN,

	VK_SPACE,
	VK_RETURN,
	VK_ESCAPE,

	VK_LSHIFT,

	VK_NUMPAD1,
	VK_NUMPAD2,
	VK_NUMPAD3,
	VK_NUMPAD4,
	VK_NUMPAD5,
	VK_NUMPAD6,
	VK_NUMPAD7,
	VK_NUMPAD8,
	VK_NUMPAD9,
	VK_NUMPAD0,

	VK_LBUTTON,
	VK_RBUTTON,
	VK_MBUTTON
};




CKeyMgr::CKeyMgr()
{

}

CKeyMgr::~CKeyMgr()
{

}


void CKeyMgr::Init()
{
	for (int i = 0; i < (int)KEY::END; ++i)
	{
		m_vecKey.push_back(tKeyInfo{KEY_STATE::NONE, false});
	}
}

void CKeyMgr::Tick()
{
	// 포커싱중인 윈도우가 있는 경우에만
	if (GetFocus())
	{
		for (size_t i = 0; i < m_vecKey.size(); ++i)
		{
			// 키가 눌려있는지 확인
			if (GetAsyncKeyState(g_arrKeyValue[i]) & 0x8001)
			{
				// 키가 눌려있고, 이전에는 눌려있지 않았다.
				if (false == m_vecKey[i].PrevPressed)
				{
					m_vecKey[i].State = KEY_STATE::TAP;
				}

				// 키가 눌려있고, 이전에도 눌려있었다.
				else
				{
					m_vecKey[i].State = KEY_STATE::PRESSED;
				}

				m_vecKey[i].PrevPressed = true;
			}

			// 키가 눌려있지 않다면
			else
			{
				// 이전에는 눌려있었다.
				if (m_vecKey[i].PrevPressed)
				{
					m_vecKey[i].State = KEY_STATE::RELEASED;
				}
				else
				{
					m_vecKey[i].State = KEY_STATE::NONE;
				}

				m_vecKey[i].PrevPressed = false;
			}
		}

		m_MousePrevPos = m_MousePos;

		POINT Pos;
		GetCursorPos(&Pos);
		ScreenToClient(CEngine::GetInst()->GetMainWnd(), &Pos);
		m_MousePos = Vec2((float)Pos.x, (float)Pos.y);
		m_MouseDir = m_MousePos - m_MousePrevPos;
	}

	// 포커싱 중인 윈도우가 없으면
	else
	{
		for (size_t i = 0; i < m_vecKey.size(); ++i)
		{
			if (KEY_STATE::TAP == m_vecKey[i].State || KEY_STATE::PRESSED == m_vecKey[i].State)
			{
				m_vecKey[i].State = KEY_STATE::RELEASED;
			}

			else if (KEY_STATE::RELEASED == m_vecKey[i].State)
			{
				m_vecKey[i].State = KEY_STATE::NONE;
			}

			m_vecKey[i].PrevPressed = false;
		}
	}
}

Vec3 CKeyMgr::GetRelativeMousePos()
{
	Vec2 ScreenScale = CEngine::GetInst()->GetResoulution();
	float screenWidth = ScreenScale.x;
	float screenHeight = ScreenScale.y;

	float ndcX = (2.0f * m_MousePos.x) / screenWidth - 1.0f;
	float ndcY = 1.0f - (2.0f * m_MousePos.y) / screenHeight;
	float ndcZ = 1.0f;

	// 클립 공간 -> 뷰 공간
	Matrix matProj = g_Trans.matProj;
	Matrix invProj = XMMatrixInverse(nullptr, matProj);
	Vec4 clipSpacePos(ndcX, ndcY, ndcZ, 1.0f);
	Vec4 viewSpacePos = XMVector4Transform(clipSpacePos, invProj);
	viewSpacePos.x /= viewSpacePos.w;
	viewSpacePos.y /= viewSpacePos.w;
	viewSpacePos.z /= viewSpacePos.w;

	// 뷰 공간 -> 월드 공간
	Matrix matView = g_Trans.matView;
	Matrix invView = XMMatrixInverse(nullptr, matView);
	Vec4 worldSpacePos = XMVector4Transform(viewSpacePos, invView);

	return worldSpacePos;

	// 레이 계산
	//Vec3 rayOrigin = cameraPos;
	//Vec3 rayDir = XMVector3Normalize(Vec3(worldSpacePos.x, worldSpacePos.y, worldSpacePos.z) - cameraPos);

	// 평면과 교차점 계산 (XZ 평면)
	//float planeY = 0.0f;
	//float t = (planeY - rayOrigin.y) / rayDir.y;
	//return rayOrigin + rayDir * t;
}
