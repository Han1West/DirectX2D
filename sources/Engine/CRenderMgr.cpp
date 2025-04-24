#include "pch.h"
#include "CRenderMgr.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CCamera.h"
#include "CTransform.h"
#include "CLight2D.h"

#include "CStructuredBuffer.h"

#include "CMeshRender.h"
#include "CAssetMgr.h"
#include "CTimeMgr.h"

CRenderMgr::CRenderMgr()
	: m_DbgObj(nullptr)
	, m_EditorCam(nullptr)
	, m_Light2DBuffer(nullptr)	
	, m_IsEditor(false)
{
	m_Light2DBuffer = new CStructuredBuffer;
	m_Light2DBuffer->Create(sizeof(tLight2DInfo), 2, SB_TYPE::SRV_ONLY, true);
}

CRenderMgr::~CRenderMgr()
{
	DELETE(m_DbgObj);
	DELETE(m_Light2DBuffer);
}

void CRenderMgr::Init()
{
	// DebugRender 용 DummyObject
	m_DbgObj = new CGameObject;
	m_DbgObj->AddComponent(new CMeshRender);
	m_DbgObj->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"DebugShapeMtrl"));
	
	// PostProcess 용 텍스쳐
	m_PostProcessTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"PostProcessTex");
}

void CRenderMgr::Render()
{
	// 렌더링 시작
	RenderStart();
	
	if (m_IsEditor)
	{
		Render_Editor();
	}	
	else
	{
		Render_Play();
	}

	// DebugRender
	Render_Debug();	
}

void CRenderMgr::RenderStart()
{
	Ptr<CTexture> pRTTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"RenderTargetTex");
	Ptr<CTexture> pDSTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"DepthStencilTex");

	// Target Clear
	Vec4 vClearColor = GetDynamicColor();
	CONTEXT->ClearRenderTargetView(pRTTex->GetRTV().Get(), vClearColor);
	CONTEXT->ClearDepthStencilView(pDSTex->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	Vec2 vResolution = CDevice::GetInst()->GetRenderResolution();

	// ViewPort 설정 - Window 화면 영역 지정
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	viewport.Width = vResolution.x;
	viewport.Height = vResolution.y;

	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;

	CONTEXT->RSSetViewports(1, &viewport);

	// Rendering 목적지를 지정
	CONTEXT->OMSetRenderTargets(1, pRTTex->GetRTV().GetAddressOf(), pDSTex->GetDSV().Get());


	// 데이터 및 리소스 바인딩
	Binding();
}

void CRenderMgr::Binding()
{
	static CConstBuffer* pCB = CDevice::GetInst()->GetCB(CB_TYPE::GLOBAL);

	g_Data.Light2DCount = (int)m_vecLight2D.size();

	pCB->SetData(&g_Data);
	pCB->Binding();
	pCB->Binding_CS();

	// 2D 광원 정보 바인딩
	static vector<tLight2DInfo> vecLight2DInfo;
	for (size_t i = 0; i < m_vecLight2D.size(); ++i)
	{
		vecLight2DInfo.push_back(m_vecLight2D[i]->GetLight2DInfo());
	}

	// 데이터를 받을 구조화버퍼의 크기가 모자라면, Resize 한다.
	if (m_Light2DBuffer->GetElementCount() < vecLight2DInfo.size())
	{
		m_Light2DBuffer->Create(sizeof(tLight2DInfo), (UINT)vecLight2DInfo.size(), SB_TYPE::SRV_ONLY, true);
	}

	// 광원 1개 이상인 경우 구조화 버퍼로 데이터 이동
	if (!vecLight2DInfo.empty())
	{
		m_Light2DBuffer->SetData(vecLight2DInfo.data(), vecLight2DInfo.size());
		m_Light2DBuffer->Binding(13);
	}

	m_vecLight2D.clear();
	vecLight2DInfo.clear();
}

void CRenderMgr::Render_Debug()
{
	list<tDebugShapeInfo>::iterator iter = m_DbgList.begin();

	for (; iter != m_DbgList.end(); )
	{
		// 모양 설정
		switch ((*iter).Shape)
		{
		case DEBUG_SHAPE::RECT:
			m_DbgObj->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh_Debug"));
			break;
		case DEBUG_SHAPE::CIRCLE:
			m_DbgObj->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"CircleMesh_Debug"));
			break;
		case DEBUG_SHAPE::CROSS:
			break;
		case DEBUG_SHAPE::LINE:
			break;		
		}

		// 위치설정
		if ((*iter).matWorld == XMMatrixIdentity())
		{
			m_DbgObj->Transform()->SetRelativePos((*iter).WorldPos);
			m_DbgObj->Transform()->SetRelativeScale((*iter).Scale);
			m_DbgObj->Transform()->SetRelativeRotation((*iter).Rotation);
			m_DbgObj->Transform()->FinalTick();
		}
		else
		{
			m_DbgObj->Transform()->SetWorldMat((*iter).matWorld);
		}
				

		// 색상 설정
		m_DbgObj->MeshRender()->GetMaterial()->SetScalarParam(VEC4_0, (*iter).Color);

		if ((*iter).DepthTest)
		{
			m_DbgObj->MeshRender()->GetMaterial()->GetShader()->SetDSState(DS_TYPE::NO_WRITE);			
		}
		else
		{
			m_DbgObj->MeshRender()->GetMaterial()->GetShader()->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);			
		}

		// 렌더
		m_DbgObj->Render();

		(*iter).Time += DT;
		if ((*iter).Duration <= (*iter).Time)
		{
			iter = m_DbgList.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void CRenderMgr::Render_Play()
{
	// 레벨 내에 카메라로 레벨 렌더링
	for (size_t i = 0; i < m_vecCam.size(); ++i)
	{
		m_vecCam[i]->SortObject();
		m_vecCam[i]->Render();
	}
}

void CRenderMgr::Render_Editor()
{
	m_EditorCam->SortObject();
	m_EditorCam->Render();
}

void CRenderMgr::RegisterCamera(CCamera* _Cam, UINT _Priority)
{
	if (-1 == _Priority)
	{
		vector<CCamera*>::iterator iter = m_vecCam.begin();
		for (; iter !=  m_vecCam.end(); ++iter)
		{
			if (*iter == _Cam)
			{
				m_vecCam.erase(iter);
				return;
			}
		}
	}

	else
	{
		if (m_vecCam.size() <= _Priority)
		{
			m_vecCam.resize(_Priority + 1);
		}		

		assert(!m_vecCam[_Priority]);

		m_vecCam[_Priority] = _Cam;
	}
}

void CRenderMgr::CopyRenderTarget()
{
	Ptr<CTexture> pRTTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"RenderTargetTex");
	CONTEXT->CopyResource(m_PostProcessTex->GetTex2D().Get(), pRTTex->GetTex2D().Get());
}


Vec4 CRenderMgr::GetDynamicColor()
{
	// 색상 정의
	Vec4 greenColor = Vec4(0.0f, 1.0f, 0.0f, 1.0f); // 초록색
	Vec4 purpleColor = Vec4(1.0f, 0.0f, 1.0f, 1.0f); // 보라색
	Vec4 blueColor = Vec4(0.0f, 0.0f, 1.0f, 1.0f); // 파란색

	// 색상 변화 주기 설정
	float cycleDuration = 6.0f; // 6초 주기로 색상이 바뀌도록 설정
	float t = fmod(g_Data.Time, cycleDuration) / cycleDuration; // 0.0에서 1.0 사이의 값으로 변환

	// 색상 보간
	Vec4 currentColor;
	if (t < 1.0f / 3.0f)
	{
		currentColor = Lerp(greenColor, purpleColor, t * 3.0f); // 초록색에서 보라색으로
	}
	else if (t < 2.0f / 3.0f)
	{
		currentColor = Lerp(purpleColor, blueColor, (t - 1.0f / 3.0f) * 3.0f); // 보라색에서 파란색으로
	}
	else
	{
		currentColor = Lerp(blueColor, greenColor, (t - 2.0f / 3.0f) * 3.0f); // 파란색에서 초록색으로
	}

	// 먹구름 낀 느낌을 주기 위해 회색조 추가
	Vec4 grayScale = Vec4(0.2f, 0.2f, 0.2f, 0.2f); // 회색조의 강도 (0.0f ~ 1.0f)
	currentColor = Lerp(currentColor, grayScale, 0.5f);


	return currentColor;
}

Vec4 CRenderMgr::Lerp(const Vec4& a, const Vec4& b, float t)
{
	return Vec4(
		a.x + (b.x - a.x) * t,
		a.y + (b.y - a.y) * t,
		a.z + (b.z - a.z) * t,
		a.w + (b.w - a.w) * t
	);
}


void CRenderMgr::DeleteDbgObj(wstring _Name)
{
	list<tDebugShapeInfo>::iterator iter = m_DbgList.begin();
	
	for (; iter != m_DbgList.end(); )
	{
		if ((*iter).Name == _Name)
		{
			iter = m_DbgList.erase(iter);
		}
		else
		{
			++iter;
		}
	}

}
