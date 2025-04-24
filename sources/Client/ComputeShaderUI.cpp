#include "pch.h"
#include "ComputeShaderUI.h"

ComputeShaderUI::ComputeShaderUI()
	: AssetUI("ComputeShader", ASSET_TYPE::COMPUTE_SHADER)
{
}

ComputeShaderUI::~ComputeShaderUI()
{
}

void ComputeShaderUI::Render_Update()
{
	AssetTitle();
}