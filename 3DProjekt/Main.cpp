#include <Windows.h>
#include <d3d11.h>
#include <iostream>
#include <DirectXMath.h>
#include <chrono>
#include <vector>
#include <time.h>

#include "WindowHelper.h"
#include "D3D11Helper.h"
#include "PipelineHelper.h"
#include "ConstantBuffer.h"
#include "LightBuffer.h"
#include "Material.h"
#include "SceneObject.h"
#include "cBuffer.h"
#include "ObjParser.h"
#include "Camera.h"
#include "TextureLoader.h"
#include "memoryLeakChecker.h"
#include "DeferredRenderer.h"
#include "ParticleHandler.h"

#include "imGUI\imconfig.h"
#include "imGUI\imgui.h"
#include "imGUI\imgui_impl_dx11.h"
#include "imGUI\imgui_internal.h"
#include "imGUI\imstb_rectpack.h"
#include "imGUI\imstb_textedit.h"
#include "imGUI\imstb_truetype.h"
#include "imGUI\imgui_impl_win32.h"


using namespace DirectX;





void newImGui(float bgClr[], ImGuiValues& imGuiStuff, bool& noIndexing);
bool createImGuiBuffer(ID3D11Device* device, ID3D11Buffer*& imGuiBuffer, struct ImGuiValues& imGuiStuff);
bool createTextures(ID3D11Device* device, ID3D11Texture2D* &texture, ID3D11ShaderResourceView* &srv, ID3D11RenderTargetView*& rtv, int width, int height);
bool CreateRenderTargetViews(ID3D11Device* device, IDXGISwapChain* swapChain, ID3D11Texture2D* buffer, ID3D11RenderTargetView*& rtv);
bool createCamBuffer(ID3D11Device* device, ID3D11Buffer*& camBuffer, struct CamData& camData);
void handleImGui(float xyz[], float rot[], float scale[], float rotSpeed[], bool &rotation, bool &normal, bool &test, float &fps);
void Render(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* &rtv, ID3D11DepthStencilView* dsView, D3D11_VIEWPORT& viewport, ID3D11VertexShader* vShader, ID3D11PixelShader* pShader, ID3D11InputLayout* inputLayout, ID3D11SamplerState* samplerState, Camera camera, CamData camData, ID3D11Buffer*& camBuffer, std::vector<SceneObject> &objects, ID3D11RenderTargetView* *rtvs , ID3D11ComputeShader* cShader, ID3D11UnorderedAccessView* uaView, ID3D11ShaderResourceView** srvs,bool &test, float clearColour[], ImGuiValues &imGuiStuff, ID3D11Buffer* imGuiBuffer, ParticleHandler &particles);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstace, _In_ LPWSTR lpCmdLine, _In_ int nCmdShhow)
{
	srand((unsigned)time(0));
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	std::vector<newObjThing> newObj;
	materialChecker material;

	//First we set some values
	float xyzPos[3] = { 0.f,0.f,0.0f,};
	float xyzRot[3] = { 0.f,0.f,0.f };
	float xyzRotSpeed[3] = { 1.f,1.f,1.f };
	float xyzScale[3] = { 1.f,1.f,1.f };
	float testValues[3] = { 0.0f,10.0f,0.0f };
	float testValues2[3] = { 10.0f,5.0f,10.0f };
	bool x = true;
	bool rotation = false;
	bool normal = false;
	bool test = true;
	float bgColour[4] = { 0.0, 0.0, 0.0, 0.0 };

	//Window size
	//const UINT WIDTH = 32*32;
	//const UINT HEIGHT = 32*18;

	const UINT WIDTH = 48 * 32;
	const UINT HEIGHT = 32 * 27;

	int verticeCounter = 0;

	//Framerate
	float fps = 144.f;

	HWND window;

	std::chrono::time_point<std::chrono::system_clock> start;
	start = std::chrono::system_clock::now();

	std::vector<SceneObject> objects;
	Camera cam;
	CamData camData;
	
	ImGuiValues imGuiStuff;

	if (!SetupWindow(hInstance, WIDTH, HEIGHT, nCmdShhow, window))
	{
		std::cerr << "Failed to setup window!" << std::endl;
		return -1;
	}

	//Basic stuff
	ID3D11Device			* device;
	ID3D11DeviceContext		* immediateContext;
	IDXGISwapChain			* swapChain;
	ID3D11DepthStencilView	* dsView;
	ID3D11InputLayout		* inputLayout;
	ID3D11RenderTargetView	* rtv; //Switch to 3 later
	ID3D11SamplerState		* samplerState;
	D3D11_VIEWPORT			  viewport;

	//Shaders
	ID3D11VertexShader		* vShader;
	ID3D11PixelShader		* pShader;
	ID3D11ComputeShader		* csShader;
	ID3D11PixelShader		* geometryPass; //Unsure if to keep

	//Deferred rendering
	ID3D11UnorderedAccessView* uaView;
	ID3D11RenderTargetView	* gBufferRtvs[5];
	ID3D11Texture2D			* gBuffers[5];
	ID3D11ShaderResourceView* gBufferSrvs[5];
	ID3D11Texture2D			* dsTexture;
	ID3D11ShaderResourceView* missingTexture;
	

	ID3D11Buffer			* values;

	//Other
	ID3D11Buffer* camBuf;
	ID3D11Buffer* imGuiBuffer;
	
	ParticleHandler particles;

	if (!SetupD3D11(WIDTH, HEIGHT, window, device, immediateContext, swapChain, rtv, dsTexture, dsView, viewport, uaView))
	{
		std::cerr << "Failed to setup d3d11!" << std::endl;
		return -1;
	}

	if (!SetupPipeline(device, vShader, pShader, csShader, inputLayout, samplerState))
	{
		std::cerr << "Failed to setup pipeline!" << std::endl;
		return -1;
	}

	for (int i = 0; i < 5; i++)
	{
		if (!createTextures(device, gBuffers[i], gBufferSrvs[i], gBufferRtvs[i], WIDTH, HEIGHT))
		{
			return 10;
		}
	}
	
	

	cam.SetPosition(0, 0, -3);
	createCamBuffer(device, camBuf, camData);
	cam.CreateCBuffer(immediateContext, device);

	particles.InitiateHandler(immediateContext, device, &cam);
	createImGuiBuffer(device, imGuiBuffer, imGuiStuff);

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, immediateContext);

	newerReadModels(device, missingTexture, material, newObj);

	MSG msg = {};

	for (int i = 0; i < 4; i++)
	{
		objects.push_back(SceneObject(newObj[i]));
		objects[i].initiateObject(immediateContext, device, &newObj[i].mesh, &newObj[i].indices);
	}


	
	objects[1].setWorldPos(testValues);
	objects[0].setWorldPos(testValues2);
	testValues[1] = -1;
	objects[3].setWorldPos(testValues);
	testValues[0] = 0.25f;
	testValues[1] = 0.25f;
	testValues[2] = 0.25f;
	//objects[4].setWorldPos(0, 20, 0);

	objects[1].setScale(testValues);

	for (auto& o : objects)
	{
		if (!o.setVertexBuffer(device))
		{
			return -6;
		}
	}

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}


		//This allows us to update depending on time since last frame (in this case 144 times per second)
		if (((std::chrono::duration<float>)(std::chrono::system_clock::now() - start)).count() > 1.0f / fps)
		{
			cam.moveCamera(immediateContext, cam, 1.f / fps);
			
			Render(immediateContext, rtv, dsView, viewport, vShader, pShader, inputLayout, 
				samplerState, cam, camData, camBuf, objects, gBufferRtvs, csShader, uaView, gBufferSrvs, test, bgColour, imGuiStuff, imGuiBuffer, particles);

			//handleImGui(xyzPos, xyzRot,xyzScale, xyzRotSpeed, rotation, normal, test, fps);
			
			/*if (rotation) 
			{ 
				xyzRot[0] += xyzRotSpeed[0] * 0.01 * 144/fps;
				if (xyzRot[0] >= XM_2PI) xyzRot[0] = 0;
				xyzRot[1] += xyzRotSpeed[1] * 0.01 * 144 / fps;
				if (xyzRot[1] >= XM_2PI) xyzRot[1] = 0;
				xyzRot[2] += xyzRotSpeed[2] * 0.01 * 144 / fps;
				if (xyzRot[2] >= XM_2PI) xyzRot[2] = 0;
			}
			objects[0].setScale(xyzScale);
			objects[0].setRot(xyzRot);
			objects[0].setWorldPos(xyzPos);*/

			

			swapChain->Present(0, 0);

			start = std::chrono::system_clock::now();
		}
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//Release system
	device->Release();
	immediateContext->Release();
	swapChain->Release();
	rtv->Release();
	dsView->Release();

	//Pipeline components
	inputLayout->Release();
	samplerState->Release();

	//Release shaders
	vShader->Release();
	pShader->Release();
	csShader->Release();

	//Buffers
	camBuf->Release();
	imGuiBuffer->Release();
	cam.noMoreMemoryLeaks();

	dsTexture->Release();
	
	//Deferred rendering
	for (int i = 0; i < 5; i++)
	{
		gBufferRtvs[i]->Release();
	}
	for (int i = 0; i < 5; i++)
	{
		gBufferSrvs[i]->Release();
	}
	for (int i = 0; i < 5; i++)
	{
		gBuffers[i]->Release();
	}
	uaView->Release();
	
	missingTexture->Release();

	

	//Releases all textures
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i].releaseCom();
	}
	for (int i = 0; i < material.textureSrvs.size(); i++)
	{
		material.textureSrvs[i]->Release();
	}

	return 0;
}

bool createCamBuffer(ID3D11Device* device, ID3D11Buffer*& camBuffer, struct CamData& camData)
{
	camData.cameraPosition = XMFLOAT3(0.0f, 0.0f, -20.0f);
	camData.padding = 0.0f;

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = sizeof(CamData);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = (void*)&camData;
	data.SysMemPitch = data.SysMemPitch = 0; // 1D resource 

	HRESULT hr = device->CreateBuffer(&desc, &data, &camBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool createImGuiBuffer(ID3D11Device* device, ID3D11Buffer*& imGuiBuffer, struct ImGuiValues& imGuiStuff)
{
	imGuiStuff.imposition = false;
	imGuiStuff.imnormal = false;
	imGuiStuff.imcolour = false;
	imGuiStuff.impadd = false;

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = sizeof(ImGuiValues);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &imGuiStuff;
	data.SysMemPitch = data.SysMemPitch = 0; // 1D resource 

	HRESULT hr = device->CreateBuffer(&desc, &data, &imGuiBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void handleImGui(float xyz[], float rot[], float scale[], float rotSpeed[], bool &rotation, bool &normal, bool& test, float &fps)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	{
		bool begun = ImGui::Begin("Testing");
		if (begun)
		{
			ImGui::SliderFloat("X pos", &xyz[0], -15.0f, 15.0f);
			ImGui::SliderFloat("Y pos", &xyz[1], -15.0f, 15.0f);
			ImGui::SliderFloat("Z pos", &xyz[2], -15.0f, 15.0f);
			ImGui::SliderFloat("X rot", &rot[0], -XM_2PI, XM_2PI);
			ImGui::SliderFloat("Y rot", &rot[1], -XM_2PI, XM_2PI);
			ImGui::SliderFloat("Z rot", &rot[2], -XM_2PI, XM_2PI);
			ImGui::SliderFloat("X scale", &scale[0], -5.0f, 5.0f);
			ImGui::SliderFloat("Y scale", &scale[1], -5.0f, 5.0f);
			ImGui::SliderFloat("Z scale", &scale[2], -5.0f, 5.0f);
			ImGui::SliderFloat("X rotSpeed", &rotSpeed[0], -15.0f, 15.0f);
			ImGui::SliderFloat("Y rotSpeed", &rotSpeed[1], -15.0f, 15.0f);
			ImGui::SliderFloat("Z rotSpeed", &rotSpeed[2], -15.0f, 15.0f);
			ImGui::Checkbox("Rotation", &rotation);
			ImGui::Checkbox("Normal", &normal);
			ImGui::Checkbox("Indexed", &test);
			ImGui::SliderFloat("FPS CAP", &fps, 1.f, 144.0f);
		}
		ImGui::End();
	}
	/*ImGui::NewFrame();
	{
		static bool active = true;
		ImGui::Begin("My First Tool", &active, ImGuiWindowFlags_MenuBar);
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open..")) { active = true; }
				if (ImGui::MenuItem("Close")) { active = false; }
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		ImGui::End();
	}
		
	*/
	

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Render
(
	ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* &rtv, ID3D11DepthStencilView* dsView,
	D3D11_VIEWPORT& viewport, ID3D11VertexShader* vShader, ID3D11PixelShader* pShader, ID3D11InputLayout* inputLayout,
	ID3D11SamplerState* samplerState, Camera camera, CamData camData,
	ID3D11Buffer*& camBuffer, std::vector<SceneObject> &objects, ID3D11RenderTargetView* *rtvs, ID3D11ComputeShader* csShader, ID3D11UnorderedAccessView* uaView, ID3D11ShaderResourceView* * srvs, 
	bool &test, float clearColour[], ImGuiValues &imGuiStuff, ID3D11Buffer* imGuiBuffer, ParticleHandler& particles
)
{
	immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

	for (int i = 0; i < 5; i++)
	{
		immediateContext->ClearRenderTargetView(rtvs[i], clearColour);
	}
	immediateContext->OMSetRenderTargets(5, rtvs, dsView);

	//Layout and other stuff
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediateContext->RSSetViewports(1, &viewport);

	//Vertex Shader
	immediateContext->VSSetShader(vShader, nullptr, 0);

	D3D11_MAPPED_SUBRESOURCE subCam = {};
	camData.cameraPosition = camera.GetPositionFloat3();
	immediateContext->Map(camBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subCam);
	memcpy(subCam.pData, &camData, sizeof(CamData));
	immediateContext->Unmap(camBuffer, 0);

	D3D11_MAPPED_SUBRESOURCE values = {};
	immediateContext->Map(imGuiBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &values);
	memcpy(values.pData, &imGuiStuff, sizeof(ImGuiValues));
	immediateContext->Unmap(imGuiBuffer, 0);

	//Pixel Shader
	immediateContext->PSSetShader(pShader, nullptr, 0);
	immediateContext->PSSetSamplers(0, 1, &samplerState);
	camera.sendView(immediateContext);

	for (int i = 0; i < objects.size(); i++)
	{
		objects[i].draw(test);
	}
	immediateContext->VSSetShader(nullptr, nullptr, 0);
	immediateContext->PSSetShader(nullptr, nullptr, 0);


	ID3D11RenderTargetView* nullRtv = nullptr;
	immediateContext->OMSetRenderTargets(1, &nullRtv, dsView);
	immediateContext->CSSetShader(csShader, nullptr, 0);
	immediateContext->CSSetUnorderedAccessViews(0, 1, &uaView, nullptr);
	immediateContext->CSSetShaderResources(0, 5, srvs);
	immediateContext->CSSetConstantBuffers(0, 1, &camBuffer);
	immediateContext->CSSetConstantBuffers(1, 1, &imGuiBuffer);
	

	immediateContext->Dispatch(48, 32, 1);

	immediateContext->CSSetShaderResources(0, 0, nullptr);
	immediateContext->OMSetRenderTargets(1, &rtv, dsView);
	immediateContext->CSSetShaderResources(0, 0, nullptr);
	immediateContext->CSSetShader(nullptr, nullptr, 0);

	particles.drawParticles();
	particles.updateParticles();
	newImGui(clearColour, imGuiStuff, test);
}

void newImGui(float bgClr[], ImGuiValues& imGuiStuff, bool &noIndexing)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	{
		bool temps[3] = { imGuiStuff.imposition,imGuiStuff.imnormal,imGuiStuff.imcolour };
		bool begun = ImGui::Begin("Testing");
		if (begun)
		{
			ImGui::ColorEdit3("BG colour", bgClr);
			ImGui::Text("");
			ImGui::Text("Draw calls");
			ImGui::Checkbox("No indexing", &noIndexing);
			ImGui::Text("");
			ImGui::Text("Deferred Rendering");
			ImGui::Checkbox("Position", &temps[0]);
			ImGui::Checkbox("Normal", &temps[1]);
			ImGui::Checkbox("Colour", &temps[2]);
		}
		ImGui::End();
		imGuiStuff.imposition = temps[0];
		imGuiStuff.imnormal = temps[1];
		imGuiStuff.imcolour = temps[2];
	}
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

bool createTextures(ID3D11Device* device, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& srv, ID3D11RenderTargetView*& rtv, int width, int height)
{

	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.Width = (UINT)width;
	textureDesc.Height = (UINT)height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA subResource;
	ZeroMemory(&subResource, sizeof(D3D11_SUBRESOURCE_DATA));

	subResource.pSysMem = {};
	subResource.SysMemPitch = 0;

	ID3D11Texture2D* tempTexture;
	HRESULT hr = device->CreateTexture2D(&textureDesc, NULL, &texture);

	if (FAILED(hr)) return false;
	hr = device->CreateRenderTargetView(texture, NULL, &rtv);
	if (FAILED(hr)) return false;
	hr = device->CreateShaderResourceView(texture, nullptr, &srv);
	return !FAILED(hr);
}

bool CreateRenderTargetViews(ID3D11Device* device, IDXGISwapChain* swapChain, ID3D11Texture2D* buffer, ID3D11RenderTargetView*& rtv)
{
	HRESULT hr = device->CreateRenderTargetView(buffer, nullptr, &rtv);
	buffer->Release();
	return !FAILED(hr);
}