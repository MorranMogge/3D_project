#include "DeferredRenderer.h"

DeferredRenderer::DeferredRenderer(int width, int height)
	:width(width), height(height)
{

}

DeferredRenderer::~DeferredRenderer()
{
}

bool DeferredRenderer::setComputeShader(ID3D11ComputeShader* cShader)
{
	return ((this->cShader=cShader)!=nullptr);
}

bool DeferredRenderer::initiateDeferredRenderer()
{

	return false;
}

bool DeferredRenderer::setEverything(ID3D11UnorderedAccessView* uaView, ID3D11Buffer* buffers[], ID3D11ShaderResourceView* srvs[], ID3D11RenderTargetView* rtvs[])
{
	this->uaView = uaView;
	//this->buffers = buffers;
	//this->srvs = srvs;

	return true;
}

void DeferredRenderer::updateBuffers(CamData camData, ImGuiValues imGuiStuff)
{
}

void DeferredRenderer::ComputeShaderPass()
{
}
