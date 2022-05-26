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

#include "imGUI\imconfig.h"
#include "imGUI\imgui.h"
#include "imGUI\imgui_impl_dx11.h"
#include "imGUI\imgui_internal.h"
#include "imGUI\imstb_rectpack.h"
#include "imGUI\imstb_textedit.h"
#include "imGUI\imstb_truetype.h"
#include "imGUI\imgui_impl_win32.h"



using namespace DirectX;

void Render(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsView, D3D11_VIEWPORT& viewport,
	ID3D11VertexShader* vShader, ID3D11PixelShader* pShader, ID3D11InputLayout* inputLayout, ID3D11Buffer* vertexBuffer, ID3D11Buffer* constantBuffer,
	ID3D11ShaderResourceView* srv, ID3D11SamplerState* samplerState, ID3D11Buffer* *lightBuffer, int nrOfVertices, int startVertice)
{
	UINT stride	= sizeof(Vertex);
	UINT offset	= 0;

	//Layout and other stuff
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->RSSetViewports(1, &viewport);
	immediateContext->OMSetRenderTargets(1, &rtv, dsView);

	//Triangle stuff
	immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Vertex Shader
	immediateContext->VSSetShader(vShader, nullptr, 0);
	immediateContext->VSSetConstantBuffers(0, 1, &constantBuffer);

	//Pixel Shader
	immediateContext->PSSetSamplers(0, 1, &samplerState);
	immediateContext->PSSetShaderResources(0, 1, &srv);
	immediateContext->PSSetShader(pShader, nullptr, 0);
	immediateContext->PSSetConstantBuffers(0, 2, lightBuffer);

	immediateContext->Draw(nrOfVertices, startVertice);
}

void handleImGui(float xyz[], bool &rotation);
void handleConstantBuffer(ConstantBuffer &cb, float xyz[]);
void updateVertexBuffer(ID3D11Device* device, ID3D11Buffer*& vertexBuffer);
SceneObject loadObject(Vertex* inFile, int nrOfVertices);
SceneObject loadObject();


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstace, _In_ LPWSTR lpCmdLine, _In_ int nCmdShhow)
{
	//First we set some values
	float xyzPos[4] = { 0.f,0.f,0.f, 0.f };
	bool x = true;
	bool rotation = false;

	//Window size
	const UINT WIDTH = 1520;
	const UINT HEIGHT = 876;

	int verticeCounter = 0;

	//Framerate
	float fps = 144.f;

	HWND window;

	std::chrono::time_point<std::chrono::system_clock> start;
	start = std::chrono::system_clock::now();

	//ConstantBuffer cb;

	CBuffer cb2;
	LightBuffer lb;
	Material mat;
	std::vector<SceneObject> objects;

	Vertex quad[3] =
	{
		//First triangle
		{ {-1.5f, 0.5f, 0.0f}, {0, 0, -1}, {0, 0} },
		{ {0.0f, -0.5f, 0.0f}, {0, 0, -1}, {1, 1} },
		{ {0.0f, -0.5f, 0.0f}, {0, 0, -1}, {0, 1} }

	};
	/*std::vector<Vertex> temp;
	temp.push_back({ {-1.0f, 0.5f, 0.0f}, {0, 0, -1}, {0, 0} });
	temp.push_back({ {0.0f, -0.5f, 0.0f}, {0, 0, -1}, {1, 1} });
	temp.push_back({ {0.0f, -0.5f, 0.0f}, {0, 0, -1}, {0, 1} });*/

	objects.push_back(loadObject());
	objects.push_back(loadObject(quad, 3));


	if (!SetupWindow(hInstance, WIDTH, HEIGHT, nCmdShhow, window))
	{
		std::cerr << "Failed to setup window!" << std::endl;
		return -1;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ID3D11Device			* device;
	ID3D11DeviceContext		* immediateContext;
	IDXGISwapChain			* swapChain;
	ID3D11RenderTargetView	* rtv;
	ID3D11Texture2D			* dsTexture;
	ID3D11DepthStencilView	* dsView;
	ID3D11VertexShader		* vShader;
	ID3D11PixelShader		* pShader;
	ID3D11InputLayout		* inputLayout;
	ID3D11Buffer			* vertexBuffer;
	ID3D11Buffer			* constantBuffer;
	ID3D11Texture2D			* texture;
	ID3D11ShaderResourceView* srv;
	ID3D11SamplerState		* samplerState;
	ID3D11Buffer			* lightBuffer[2];
	D3D11_VIEWPORT			  viewport;

	if (!SetupD3D11(WIDTH, HEIGHT, window, device, immediateContext, swapChain, rtv, dsTexture, dsView, viewport))
	{
		std::cerr << "Failed to setup d3d11!" << std::endl;
		return -1;
	}

	if (!SetupPipeline(device, vertexBuffer, vShader, pShader, inputLayout, texture, srv, samplerState))
	{
		std::cerr << "Failed to setup pipeline!" << std::endl;
		return -1;
	}

	if (!cb2.setConstBuf(device, constantBuffer))
	{
		std::cerr << "Failed to setup constant buffer!" << std::endl;
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

	for (auto& o : objects)
	{
		if (!o.setVertexBuffer(device))
		{
			return -6;
		}
	}

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, immediateContext);

	MSG msg = {};
	

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}


		//This allows us to update depending on time since last frame
		if (((std::chrono::duration<float>)(std::chrono::system_clock::now() - start)).count() > 1.0f / fps)
		{
			verticeCounter = 0;
			float clearColour[4] = { 0, 0, 0, 0 };
			immediateContext->ClearRenderTargetView(rtv, clearColour);
			immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
			//cb2.setNewBuffer(immediateContext, constantBuffer, objects[0].getWorldMatrix());
			//Render(immediateContext, rtv, dsView, viewport, vShader, pShader, inputLayout, objects[0].getVertexBuffer(), constantBuffer, srv, samplerState, lightBuffer, objects[0].getVerticeAmount(), 3);
			
			for (auto& o : objects)
			{

				cb2.setNewBuffer(immediateContext, constantBuffer, o.getWorldMatrix());		
				Render(immediateContext, rtv, dsView, viewport, vShader, pShader, inputLayout, o.getVertexBuffer(), constantBuffer, srv, samplerState, lightBuffer, o.getVerticeAmount(), verticeCounter);
				verticeCounter += o.getVerticeAmount();
			}
			
			
			
			/*handleConstantBuffer(cb, xyzPos);
			if (rotation)
			{
				cb.NewUpdate(xyzPos[3]);
			}
			cb.Update(immediateContext, constantBuffer);*/

			//handleImGui(xyzPos, rotation);


			swapChain->Present(0, 0);

			start = std::chrono::system_clock::now();
		}
	}

	device->Release();
	immediateContext->Release();
	swapChain->Release();
	rtv->Release();
	dsTexture->Release();
	dsView->Release();
	vShader->Release();
	pShader->Release();
	inputLayout->Release();
	//vertexBuffer->Release();
	lightBuffer[0]->Release();
	lightBuffer[1]->Release();
	constantBuffer->Release();
	texture->Release();
	srv->Release();
	samplerState->Release();

	return 0;
}

void handleImGui(float xyz[], bool &rotation)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	{
		bool begun = ImGui::Begin("Modifiers");
		if (begun)
		{
			ImGui::SliderFloat("X pos", &xyz[0], -0.5f, 0.5f);
			ImGui::SliderFloat("Y pos", &xyz[1], -0.5f, 0.5f);
			ImGui::SliderFloat("Z pos", &xyz[2], -1.5f, 0.5f);
			ImGui::SliderFloat("Rotation speed", &xyz[3], -5.0f, 5.0f);
			ImGui::Checkbox("Rotation", &rotation);
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

void handleConstantBuffer(ConstantBuffer& cb, float xyz[])
{
	cb.setXValue(xyz[0]);
	cb.setYValue(xyz[1]);
	cb.setZValue(xyz[2]);
}

void updateVertexBuffer(ID3D11Device* device, ID3D11Buffer*& vertexBuffer)
{
}

SceneObject loadObject(Vertex* inFile, int nrOfVertices)
{
	return SceneObject(inFile, nrOfVertices);
}

SceneObject loadObject()
{
	return SceneObject();
}