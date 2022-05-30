#include <Windows.h>
#include <d3d11.h>
#include <iostream>
#include <DirectXMath.h>
#include <chrono>
#include <vector>

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

#include "imGUI\imconfig.h"
#include "imGUI\imgui.h"
#include "imGUI\imgui_impl_dx11.h"
#include "imGUI\imgui_internal.h"
#include "imGUI\imstb_rectpack.h"
#include "imGUI\imstb_textedit.h"
#include "imGUI\imstb_truetype.h"
#include "imGUI\imgui_impl_win32.h"



using namespace DirectX;

struct CamData
{
	XMFLOAT3 cameraPosition;
	float padding;
};

bool createCamBuffer(ID3D11Device* device, ID3D11Buffer*& camBuffer, struct CamData& camData);
void handleImGui(float xyz[], float rot[], float scale[], float rotSpeed[], bool &rotation, bool &normal);
void Render(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsView, D3D11_VIEWPORT& viewport, ID3D11VertexShader* vShader, ID3D11PixelShader* pShader, ID3D11InputLayout* inputLayout, ID3D11SamplerState* samplerState, ID3D11Buffer** lightBuffer, Camera camera, CamData camData, ID3D11Buffer*& camBuffer, std::vector<SceneObject> &objects);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstace, _In_ LPWSTR lpCmdLine, _In_ int nCmdShhow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	std::vector<objThing> obj;
	readModels(obj);
	std::vector<ID3D11ShaderResourceView*> textureSrvs;

	//First we set some values
	float xyzPos[3] = { 0.f,0.f,4.5f,};
	float xyzRot[3] = { 0.f,0.f,0.f };
	float xyzRotSpeed[3] = { 1.f,1.f,1.f };
	float xyzScale[3] = { 1.f,1.f,1.f };
	bool x = true;
	bool rotation = false;
	bool normal = false;

	//Window size
	const UINT WIDTH = 1520;
	const UINT HEIGHT = 876;

	int verticeCounter = 0;

	//Framerate
	float fps = 144.f;

	HWND window;

	std::chrono::time_point<std::chrono::system_clock> start;
	start = std::chrono::system_clock::now();

	CBuffer cb2;
	LightBuffer lb;
	Material mat;
	std::vector<SceneObject> objects;
	Camera cam;
	CamData camData;
	ID3D11Buffer* camBuf;

	if (!SetupWindow(hInstance, WIDTH, HEIGHT, nCmdShhow, window))
	{
		std::cerr << "Failed to setup window!" << std::endl;
		return -1;
	}


	ID3D11Device			* device;
	ID3D11DeviceContext		* immediateContext;
	IDXGISwapChain			* swapChain;
	ID3D11RenderTargetView	* rtv;
	ID3D11Texture2D			* dsTexture;
	ID3D11DepthStencilView	* dsView;
	ID3D11VertexShader		* vShader;
	ID3D11PixelShader		* pShader;
	ID3D11InputLayout		* inputLayout;
	ID3D11SamplerState		* samplerState;
	ID3D11Buffer			* lightBuffer[2];
	ID3D11Buffer			* values;
	D3D11_VIEWPORT			  viewport;

	if (!SetupD3D11(WIDTH, HEIGHT, window, device, immediateContext, swapChain, rtv, dsTexture, dsView, viewport))
	{
		std::cerr << "Failed to setup d3d11!" << std::endl;
		return -1;
	}

	if (!SetupPipeline(device, vShader, pShader, inputLayout, samplerState))
	{
		std::cerr << "Failed to setup pipeline!" << std::endl;
		return -1;
	}

	if (!lb.setLightBuffer(device, lightBuffer[0]))
	{
		std::cerr << "Failed to setup light buffer!" << std::endl;
		return -1;
	}

	if (!mat.setMaterial(device, lightBuffer[1]))
	{
		std::cerr << "Failed to setup material info!" << std::endl;
		return -1;
	}

	

	cam.SetPosition(0, 0, -3);
	createCamBuffer(device, camBuf, camData);
	cam.CreateCBuffer(immediateContext, device);

	LoadTexutres(device, textureSrvs);

	

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, immediateContext);

	MSG msg = {};
	
	objects.push_back(SceneObject(&obj[3].mesh));
	objects[0].setImmediateContext(immediateContext);
	objects[0].setTextureSrv(textureSrvs[3]);
	objects[0].createConstBuf(device);

	objects.push_back(SceneObject(&obj[0].mesh));
	objects[1].setImmediateContext(immediateContext);
	objects[1].setTextureSrv(textureSrvs[textureSrvs.size() - 1]);
	objects[1].createConstBuf(device);

	objects.push_back(SceneObject(&obj[2].mesh));
	objects[2].setImmediateContext(immediateContext);
	objects[2].setTextureSrv(textureSrvs[1]);
	objects[2].createConstBuf(device);
	float tempArr[]{ 0,-5,0 };
	//objects[2].setScale(tempArr);
	tempArr[1] = -5;
	objects[2].setWorldPos(tempArr);

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
			cam.moveCamera(immediateContext, cam, 1.f / 144.f);
			verticeCounter = 0;
			float clearColour[4] = { 0, 0, 0, 0 };
			immediateContext->ClearRenderTargetView(rtv, clearColour);
			immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
			
			Render(immediateContext, rtv, dsView, viewport, vShader, pShader, inputLayout, 
				samplerState, lightBuffer, cam, camData, camBuf, objects);

			handleImGui(xyzPos, xyzRot,xyzScale, xyzRotSpeed, rotation, normal);
			lb.setNormal(immediateContext, lightBuffer[0], normal);
			if (rotation) 
			{ 
				xyzRot[0] += xyzRotSpeed[0]*0.01;
				if (xyzRot[0] >= XM_2PI) xyzRot[0] = 0;
				xyzRot[1] += xyzRotSpeed[1] * 0.01;
				if (xyzRot[1] >= XM_2PI) xyzRot[1] = 0;
				xyzRot[2] += xyzRotSpeed[2] * 0.01;
				if (xyzRot[2] >= XM_2PI) xyzRot[2] = 0;
			}
			objects[0].setScale(xyzScale);
			objects[0].setRot(xyzRot);
			objects[0].setWorldPos(xyzPos);

			swapChain->Present(0, 0);

			start = std::chrono::system_clock::now();
		}
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	device->Release();
	immediateContext->Release();
	swapChain->Release();
	rtv->Release();
	dsTexture->Release();
	dsView->Release();
	vShader->Release();
	pShader->Release();
	inputLayout->Release();
	lightBuffer[0]->Release();
	lightBuffer[1]->Release();
	samplerState->Release();
	camBuf->Release();
	cam.noMoreMemoryLeaks();

	//Releases all textures
	for (int i = 0; i < textureSrvs.size(); i++)
	{
		textureSrvs[i]->Release();
	}
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i].releaseCom();
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

void handleImGui(float xyz[], float rot[], float scale[], float rotSpeed[], bool &rotation, bool &normal)
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
	ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsView,
	D3D11_VIEWPORT& viewport, ID3D11VertexShader* vShader, ID3D11PixelShader* pShader, ID3D11InputLayout* inputLayout,
	ID3D11SamplerState* samplerState, ID3D11Buffer** lightBuffer, Camera camera, CamData camData,
	ID3D11Buffer*& camBuffer, std::vector<SceneObject> &objects
)
{
	//Layout and other stuff
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediateContext->RSSetViewports(1, &viewport);
	immediateContext->OMSetRenderTargets(1, &rtv, dsView);

	//Vertex Shader
	immediateContext->VSSetShader(vShader, nullptr, 0);

	//Pixel Shader
	immediateContext->PSSetShader(pShader, nullptr, 0);
	immediateContext->PSSetConstantBuffers(0, 2, lightBuffer);

	//We only need to change sampler if we want to sample texture in a different way
	immediateContext->PSSetSamplers(0, 1, &samplerState);

	//This makes it possible to see from the camera's perspective
	D3D11_MAPPED_SUBRESOURCE subCam = {};
	camData.cameraPosition = camera.GetPositionFloat3();
	immediateContext->Map(camBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subCam);
	memcpy(subCam.pData, &camData, sizeof(CamData));
	immediateContext->Unmap(camBuffer, 0);
	immediateContext->PSSetConstantBuffers(2, 1, &camBuffer);

	//Send it to the vertex shader, index = 1 if no index is added to argument
	camera.sendView(immediateContext);

	//Draw every single object in the scene
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i].draw();
	}
}
