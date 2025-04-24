#include "pch.h"
#include "CTimeMgr.h"

#include "CEngine.h"

#include "CKeyMgr.h"
#include "CFontMgr.h"

CTimeMgr::CTimeMgr()
	: m_Frequency{}
	, m_CurrentCount{}
	, m_PrevCount{}	
	, m_fDT(0.f)
	, m_Time(0.f)
	, m_fEngineDT(0.f)
	, m_fEngineTime(0.f)
	, m_FPS(0)
	, m_Second(0.)
	, m_IsStop(true)
{

}

CTimeMgr::~CTimeMgr()
{

}

void CTimeMgr::Init()
{
	QueryPerformanceFrequency(&m_Frequency);
	QueryPerformanceCounter(&m_PrevCount);
}

void CTimeMgr::Tick()
{
	QueryPerformanceCounter(&m_CurrentCount);

	m_fEngineDT = (float)(m_CurrentCount.QuadPart - m_PrevCount.QuadPart) / (float)m_Frequency.QuadPart;

	// DT 보정, 60 분의 1 보다 크지 못하게 보정
	if ((1.f / 60.f) < m_fEngineDT)
		m_fEngineDT = (1.f / 60.f);
		
	// 시간 누적
	m_fEngineTime += m_fEngineDT;

	// 시간 누적
	m_Second += m_fEngineDT;

	// 함수 호출횟수
	++m_FPS;

	if (1. < m_Second)
	{
		m_Second -= 1.;

		// 윈도우 타이틀에 FPS 랑 DeltaTime 표시
		//wchar_t buff[256] = {};
		//swprintf_s(buff, 256, L"FPS : %d, DeltaTime : %f", m_FPS, m_fDT);
		//SetWindowText(CEngine::GetInst()->GetMainWnd(), buff);

		m_FPS = 0;
	}

	m_PrevCount = m_CurrentCount;

	if (!m_IsStop)
	{
		m_fDT = m_fEngineDT;
		m_Time += m_fDT;
	}	


	// GlobalData 갱신
	g_Data.DeltaTime = m_fDT;
	g_Data.Time = m_Time;

	g_Data.DT_Engine = m_fEngineDT;
	g_Data.Time_Engine = m_fEngineTime;

	//Vec3 CurMousePos = CKeyMgr::GetInst()->GetRelativeMousePos();
	//swprintf_s(m_MousePos, L" 가나다라 X: %f , Y: %f ", CurMousePos.x, CurMousePos.y);
}


void CTimeMgr::Render()
{
	//CFontMgr::GetInst()->DrawFont(m_MousePos, 10, 20, 16, FONT_RGBA(255, 20, 20, 255));
	//CFontMgr::GetInst()->DrawFont(L"Test", 100, 100, 100, FONT_RGBA(255, 10, 10, 255));
}
