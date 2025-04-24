#include "pch.h"
#include "CMesh.h"

#include "CDevice.h"

CMesh::CMesh(bool _bEngineRes)
	: CAsset(ASSET_TYPE::MESH, _bEngineRes)
	, m_VtxSysMem(nullptr)
	, m_IdxSysMem(nullptr)
	, m_VBDesc{}
	, m_VtxCount(0)
	, m_IBDesc{}
	, m_IdxCount(0)
{
}

CMesh::~CMesh()
{
	DELETE_ARR(m_VtxSysMem);
	DELETE_ARR(m_IdxSysMem);
}

void CMesh::Binding()
{
	// 렌더링할 정점 정보 전달
	UINT Stride = sizeof(Vtx);
	UINT Offset = 0;

	CONTEXT->IASetVertexBuffers(0, 1, m_VB.GetAddressOf(), &Stride, &Offset);
	CONTEXT->IASetIndexBuffer(m_IB.Get(), DXGI_FORMAT_R32_UINT, 0);	
}

void CMesh::Render()
{
	Binding();

	CONTEXT->DrawIndexed(m_IdxCount, 0, 0);
}

void CMesh::Render_Particle(int _Count)
{
	Binding();	
	
	CONTEXT->DrawIndexedInstanced(m_IdxCount, _Count, 0, 0, 0);
}

int CMesh::Create(Vtx* _Vtx, UINT _VtxCount, UINT* _pIdx, UINT _IdxCount)
{
	assert(nullptr == m_VB && nullptr == m_IB);

	m_VtxCount = _VtxCount;
	m_IdxCount = _IdxCount;

	m_VtxSysMem = new Vtx[m_VtxCount];
	memcpy(m_VtxSysMem, _Vtx, sizeof(Vtx) * m_VtxCount);

	m_IdxSysMem = new UINT[m_IdxCount];
	memcpy(m_IdxSysMem, _pIdx, sizeof(UINT) * m_IdxCount);

	// 정점 데이터를 SysMem -> GPU Mem 로 이동
	m_VBDesc.ByteWidth = sizeof(Vtx) * _VtxCount;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.Usage = D3D11_USAGE_DEFAULT;

	// 생성시킬 버퍼의 초기데이터 전달용
	D3D11_SUBRESOURCE_DATA tSub = {};
	tSub.pSysMem = _Vtx;

	// 버텍스 용도의 ID3D11Buffer 객체 생성
	if (FAILED(DEVICE->CreateBuffer(&m_VBDesc, &tSub, m_VB.GetAddressOf())))
	{
		return E_FAIL;
	}

	// 인덱스
	m_IBDesc.ByteWidth = sizeof(UINT) * m_IdxCount;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.Usage = D3D11_USAGE_DEFAULT;

	// 생성시킬 버퍼의 초기데이터 전달용	
	tSub.pSysMem = _pIdx;

	// 인덱스 용도의 ID3D11Buffer 객체 생성
	if (FAILED(DEVICE->CreateBuffer(&m_IBDesc, &tSub, m_IB.GetAddressOf())))
	{
		return E_FAIL;
	}
}
