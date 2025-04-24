#include "pch.h"
#include "CMeshRender.h"

#include "assets.h"
#include "CTransform.h"
#include "CFlipbookPlayer.h"

CMeshRender::CMeshRender()
	: CRenderComponent(COMPONENT_TYPE::MESHRENDER)
{
}

CMeshRender::~CMeshRender()
{
}

void CMeshRender::FinalTick()
{
}

void CMeshRender::Render()
{
	if (FlipbookPlayer())
	{
		FlipbookPlayer()->Binding();
	}

	// 위치정보
	Transform()->Binding();

	// 사용할 쉐이더
	GetMaterial()->Binding();
	
	// 렌더링
	GetMesh()->Render();

	if (FlipbookPlayer())
	{
		FlipbookPlayer()->Clear();
	}
}
