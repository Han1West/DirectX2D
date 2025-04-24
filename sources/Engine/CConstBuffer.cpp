#include "pch.h"
#include "CConstBuffer.h"

#include "CDevice.h"

CConstBuffer::CConstBuffer(CB_TYPE _Type)
	: m_Desc{}
	, m_Type(_Type)
{
}

CConstBuffer::~CConstBuffer()
{
}

int CConstBuffer::Create(UINT _BufferSize)	
{
	assert(!(_BufferSize % 16));

	m_Desc.ByteWidth = _BufferSize;
	m_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	m_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_Desc.Usage = D3D11_USAGE_DYNAMIC;

	// 버텍스 용도의 ID3D11Buffer 객체 생성
	if (FAILED(DEVICE->CreateBuffer(&m_Desc, nullptr, m_CB.GetAddressOf())))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CConstBuffer::Binding()
{
	CONTEXT->VSSetConstantBuffers((UINT)m_Type, 1, m_CB.GetAddressOf());
	CONTEXT->HSSetConstantBuffers((UINT)m_Type, 1, m_CB.GetAddressOf());
	CONTEXT->DSSetConstantBuffers((UINT)m_Type, 1, m_CB.GetAddressOf());
	CONTEXT->GSSetConstantBuffers((UINT)m_Type, 1, m_CB.GetAddressOf());
	CONTEXT->PSSetConstantBuffers((UINT)m_Type, 1, m_CB.GetAddressOf());
}

void CConstBuffer::Binding_CS()
{
	CONTEXT->CSSetConstantBuffers((UINT)m_Type, 1, m_CB.GetAddressOf());
}

void CConstBuffer::SetData(void* _pData, UINT _DataSize)
{
	D3D11_MAPPED_SUBRESOURCE tMapSub = {};
	
	CONTEXT->Map(m_CB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &tMapSub);

	if (_DataSize == 0)
	{
		_DataSize = m_Desc.ByteWidth;
	}

	memcpy(tMapSub.pData, _pData, _DataSize);

	CONTEXT->Unmap(m_CB.Get(), 0);
}

