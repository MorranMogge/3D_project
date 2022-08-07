#include <Windows.h>
#include <d3d11.h>
#include <iostream>
#include <DirectXMath.h>
#include <chrono>
#include <vector>
#include <time.h>

#include "Project.h"




using namespace DirectX;

void handleImGui(float bgClr[], ImGuiValues& imGuiStuff, int& submeshAmount, CamData& camData, Camera& cam, bool currentCamera[], ShadowMappingClass& shadowMap, bool& cubeMap);
bool createImGuiBuffer(ID3D11Device* device, ID3D11Buffer*& imGuiBuffer, struct ImGuiValues& imGuiStuff);
bool createCamBuffer(ID3D11Device* device, ID3D11Buffer*& camBuffer, struct CamData& camData);
void Render(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* &rtv, ID3D11DepthStencilView* dsView, D3D11_VIEWPORT& viewport, ID3D11SamplerState* samplerState, Camera& camera, CamData& camData, ID3D11Buffer*& camBuffer, std::vector<SceneObject*> &objects, int& submeshAmount, float clearColour[], ImGuiValues &imGuiStuff, ID3D11Buffer* imGuiBuffer, ParticleHandler &particles, TesselatorClass& tesselator, DeferredRenderer& deferred, CubemapClass& cubemap, FrustumCuller& culler, Camera otherView[], ShadowMappingClass& shadowMap, bool currentCamera[], bool& cubeMap);
std::vector<SceneObject> setUpScene(ID3D11DeviceContext* immediateContext, ID3D11Device* device, std::vector<objectInfo> objData);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstace, _In_ LPWSTR lpCmdLine, _In_ int nCmdShhow)
{
	srand((unsigned)time(0));
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	DirectX::XMFLOAT3 cubePositions[20] =
	{
		DirectX::XMFLOAT3(2*5.0f,5.0f,2*5.0f),
		DirectX::XMFLOAT3(2*10.0f,5.0f,2*10.0f),
		DirectX::XMFLOAT3(2*15.0f,5.0f,2*15.0f),
		DirectX::XMFLOAT3(2*20.0f,5.0f,2*20.0f),
		DirectX::XMFLOAT3(2* 22.5f,5.0f,2* 22.5f),
		DirectX::XMFLOAT3(2*-5.0f,5.0f,2*-5.0f),
		DirectX::XMFLOAT3(2*-10.0f,5.0f,-2*10.0f),
		DirectX::XMFLOAT3(2*-15.0f,5.0f,-2*15.0f),
		DirectX::XMFLOAT3(2*-20.0f,5.0f,-2*20.0f),
		DirectX::XMFLOAT3(2*-22.5f,5.0f,-2* 22.5f),
		DirectX::XMFLOAT3(2*5.0f,5.0f,-2 * 5.0f),
		DirectX::XMFLOAT3(2*10.0f,5.0f,-2*10.0f),
		DirectX::XMFLOAT3(2*15.0f,5.0f,-2*15.0f),
		DirectX::XMFLOAT3(2*20.0f,5.0f,-2*20.0f),
		DirectX::XMFLOAT3(2* 22.5f,5.0f,-2*25.0f),
		DirectX::XMFLOAT3(2*-5.0f,5.0f,2 * 5.0f),
		DirectX::XMFLOAT3(2*-10.0f,5.0f,2*10.0f),
		DirectX::XMFLOAT3(2*-15.0f,5.0f,2*15.0f),
		DirectX::XMFLOAT3(2*-20.0f,5.0f,2*20.0f),
		DirectX::XMFLOAT3(2*-22.5f,5.0f,2*22.5f)
	};

	//First we set some values
	float xyzPos[3] = { 0.f,0.f,0.0f,};
	float xyzRot[3] = { 0.f,0.f,0.f };
	float xyzRotSpeed[3] = { 1.f,1.f,1.f };
	float xyzScale[3] = { 1.f,1.f,1.f };
	float testValues[3] = { -8.5f,10.0f,-8.5f };
	float testValues2[3] = { 10.0f,1.0f,5.0f };
	bool rotation = false;
	bool normal = false;
	bool test = true;
	float bgColour[4] = { 0.0, 0.0, 0.0, 0.0 };
	bool currentCamera[4]{ false };
	bool cubeMap = true;
	int submeshAmount = 0;

	//Window size
	const UINT WIDTH = 32 * 32;
	const UINT HEIGHT = 32 * 32;

	//Framerate
	float fps = 144.f;

	HWND window;

	if (!SetupWindow(hInstance, WIDTH, HEIGHT, nCmdShhow, window))
	{
		std::cerr << "Failed to setup window!" << std::endl;
		return -1;
	}

	std::chrono::time_point<std::chrono::system_clock> start;
	start = std::chrono::system_clock::now();

	ID3D11Device			* device;
	ID3D11DeviceContext		* immediateContext;
	IDXGISwapChain			* swapChain;
	ID3D11RenderTargetView	* rtv; 
	ID3D11DepthStencilView	* dsView;
	ID3D11SamplerState		* samplerState;
	D3D11_VIEWPORT			  viewport;

	ID3D11Texture2D			* dsTexture;
	ID3D11ShaderResourceView* missingTexture;

	//Other
	ID3D11Buffer* camBuf = nullptr;
	ID3D11Buffer* imGuiBuffer = nullptr;
	
	//Helpers
	ParticleHandler particles;
	TesselatorClass tesselator;
	DeferredRenderer deferred(WIDTH, HEIGHT);
	CubemapClass cubemap;
	FrustumCuller culler;
	ShadowMappingClass shadowMap;

	//Classes
	std::vector<objectInfo> newObj;
	materialChecker material;
	std::vector<SceneObject *> objects;
	Camera camera;
	Camera otherView[4];
	CamData camData;
	ImGuiValues imGuiStuff;

	
	if (!SetupD3D11(WIDTH, HEIGHT, window, device, immediateContext, swapChain, rtv, dsTexture, dsView, viewport))
	{
		std::cerr << "Failed to setup d3d11!" << std::endl;
		return -1;
	}

	if (!SetupPipeline(device, samplerState, camBuf, camData, imGuiBuffer, imGuiStuff))
	{
		std::cerr << "Failed to setup components!" << std::endl;
		return -1;
	}

	for (int i = 0; i < 4; i++) { if (!otherView[i].initiateBuffers(immediateContext, device)) return 1; }
	if (!camera.initiateBuffers(immediateContext, device)) return 1;

	readModels(device, missingTexture, material, newObj);
	cubemap.createCube(newObj[4]);

	//Initiates all the handlers
	if (!shadowMap.initiateShadowMapping(immediateContext, device))		return 1;
	if (!deferred.initiateDeferredRenderer(immediateContext, device, swapChain, dsView, &camera, &imGuiStuff)) return 1;
	if (!particles.InitiateHandler(immediateContext, device, &camera))	return 1;
	if (!cubemap.initiateCubemap(immediateContext, device, dsView))		return 1;
	if (!tesselator.setUpTesselator(immediateContext, device, &camera)) return 1;
	
	camera.createFrustum();
	shadowMap.setCameraPosAndRot(otherView);

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, immediateContext);

	MSG msg = {};

	for (int i = 0; i < 4; i++)
	{
		objects.push_back(new SceneObject(newObj[i]));
		if (!objects[i]->initiateObject(immediateContext, device, &newObj[i].mesh, &newObj[i].indices));
	}


	objects[1]->setWorldPos(testValues);
	objects[0]->setWorldPos(testValues2);
	testValues[1] = -1;
	objects[2]->setScale(2,2,2);

	testValues[0] = 1.0f;
	testValues[1] = 1.0f;
	testValues[2] = 1.0f;

	objects[1]->setScale(testValues);
	objects.push_back(new SceneObject(newObj[0]));
	if (!objects[4]->initiateObject(immediateContext, device, &newObj[0].mesh, &newObj[0].indices)) return false;

	int amount = objects.size();
	int counter = 0;
	for (int i = objects.size(); i < 20 + amount; i++)
	{
		objects.push_back(new SceneObject(newObj[0]));
		if (!objects[i]->initiateObject(immediateContext, device, &newObj[0].mesh, &newObj[0].indices)) return 1;
		objects[i]->setWorldPos(cubePositions[counter++]);
	}

	for (int i = 0; i < 4; i++)
	{
		objects.push_back(new SceneObject(newObj[0]));
		if (!objects[objects.size()-1]->initiateObject(immediateContext, device, &newObj[0].mesh, &newObj[0].indices)) return 1;
		objects[objects.size() - 1]->setWorldPos(otherView[i].GetPositionFloat3());
		objects[objects.size() - 1]->setRot(otherView[i].GetRotationFloat3());
	
	}
	objects.push_back(new SceneObject(newObj[5]));
	if (!objects[objects.size() - 1]->initiateObject(immediateContext, device, &newObj[5].mesh, &newObj[5].indices)) return false;
	objects[objects.size() - 1]->setWorldPos(DirectX::XMFLOAT3(-30, 35, 0));
	objects[objects.size() - 1]->setScale(3,3,3);

	for (auto& o : objects)
	{
		o->setBoundingBox();
		if (!o->setVertexBuffer(device))
		{
			return -6;
		}
	}

	int quadTreeSize = 64;
	if (!culler.initiateCuller(&objects, quadTreeSize, quadTreeSize)) return 1;

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
			camera.moveCamera(immediateContext, camera, 1.f / fps);
			
			Render(immediateContext, rtv, dsView, viewport, 
				samplerState, camera, camData, camBuf, objects, 
				submeshAmount, bgColour, imGuiStuff, imGuiBuffer, 
				particles, tesselator, deferred, cubemap, culler, 
				otherView, shadowMap, currentCamera, cubeMap);
			

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
	dsView->Release();
	samplerState->Release();
	dsTexture->Release();
	missingTexture->Release();

	//Buffers
	camBuf->Release();
	imGuiBuffer->Release();
	  
	for (int i = 0; i < objects.size(); i++)
	{
		delete objects[i];
	}
	for (int i = 0; i < material.textureSrvs.size(); i++)
	{
		material.textureSrvs[i]->Release();
	}

	return 0;
}

void Render
(
	ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* &rtv, ID3D11DepthStencilView* dsView,
	D3D11_VIEWPORT& viewport, ID3D11SamplerState* samplerState, Camera& camera, CamData& camData,
	ID3D11Buffer*& camBuffer, std::vector<SceneObject*> &objects, int& submeshAmount, float clearColour[],
	ImGuiValues &imGuiStuff, ID3D11Buffer* imGuiBuffer, ParticleHandler& particles, TesselatorClass& tesselator, 
	DeferredRenderer& deferred, CubemapClass& cubemap, FrustumCuller& culler, Camera otherView[], 
	ShadowMappingClass& shadowMap, bool currentCamera[], bool& cubeMap
)
{
	D3D11_MAPPED_SUBRESOURCE subCam = {};
	camData.cameraPosition = camera.GetPositionFloat3();
	immediateContext->Map(camBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subCam);
	memcpy(subCam.pData, &camData, sizeof(CamData));
	immediateContext->Unmap(camBuffer, 0);

	D3D11_MAPPED_SUBRESOURCE values = {};
	immediateContext->Map(imGuiBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &values);
	memcpy(values.pData, &imGuiStuff, sizeof(ImGuiValues));
	immediateContext->Unmap(imGuiBuffer, 0);
	immediateContext->ClearRenderTargetView(rtv, clearColour);

	camera.updateFrustum();
	std::vector<SceneObject*> culledObjects = culler.cullObjects(camera.getFrustumBB());
	for (int i = 0; i < culledObjects.size(); i++) //This ensures we always see the camera
	{
		if (objects[4] == culledObjects[i]) break;
		culledObjects.push_back(objects[4]);
	}
	
	objects[4]->setWorldPos(camera.GetPositionFloat3());
	objects[4]->setRot(camera.GetRotationFloat3());

	immediateContext->PSSetSamplers(0, 1, &samplerState);
	immediateContext->RSSetViewports(1, &viewport);
	shadowMap.firstPass(culledObjects);

	deferred.firstPass();
	immediateContext->PSSetSamplers(0, 1, &samplerState);
	shadowMap.secondPass();

	tesselator.setWireFrame(imGuiStuff.imwireframe);
	tesselator.setRasterizerState();
	immediateContext->HSSetConstantBuffers(0, 1, &camBuffer);


	if (currentCamera[0]) otherView[0].sendView(immediateContext, 0, true);
	else if (currentCamera[1]) otherView[1].sendView(immediateContext, 0, true);
	else if (currentCamera[2]) otherView[2].sendView(immediateContext, 0, true);
	else if (currentCamera[3]) otherView[3].sendView(immediateContext, 0, true);
	else camera.sendView(immediateContext, 0, true);

	for (int i = 0; i < culledObjects.size(); i++)
	{
		culledObjects[i]->draw(submeshAmount);
	}

	shadowMap.preCubeDraw();
	if (cubeMap) cubemap.draw(objects, particles, dsView);
	shadowMap.clearSecondPass();

	immediateContext->PSSetSamplers(0, 1, &samplerState);

	immediateContext->CSSetConstantBuffers(0, 1, &camBuffer);
	immediateContext->CSSetConstantBuffers(1, 1, &imGuiBuffer);
	
	shadowMap.preDispatch();
	deferred.secondPass();

	immediateContext->CSSetShaderResources(0, 0, nullptr);
	
	immediateContext->CSSetShaderResources(0, 0, nullptr);
	immediateContext->CSSetShader(nullptr, nullptr, 0);

	//Particle Draw call
	immediateContext->OMSetRenderTargets(1, &rtv, dsView);

	if (currentCamera[0]) otherView[0].sendView(immediateContext);
	else if (currentCamera[1]) otherView[1].sendView(immediateContext);
	else if (currentCamera[2]) otherView[2].sendView(immediateContext);
	else if (currentCamera[3]) otherView[3].sendView(immediateContext);
	else camera.sendView(immediateContext);

	immediateContext->PSSetConstantBuffers(0, 1, &camBuffer);
	if (cubeMap) cubemap.drawCube();
	if (currentCamera[0]) particles.drawParticles(&otherView[0]);
	else if (currentCamera[1]) particles.drawParticles(&otherView[1]);
	else if (currentCamera[2]) particles.drawParticles(&otherView[2]);
	else if (currentCamera[3]) particles.drawParticles(&otherView[3]);
	else particles.drawParticles();

	particles.updateParticles();

	//ImGui
	handleImGui(clearColour, imGuiStuff, submeshAmount, camData, camera, currentCamera, shadowMap, cubeMap);
}

std::vector<SceneObject> setUpScene(ID3D11DeviceContext* immediateContext, ID3D11Device* device, std::vector<objectInfo> objData)
{
	std::vector<SceneObject> obj;
	float testValues[3] = { 0.0f,10.0f,0.0f };
	float testValues2[3] = { 10.0f,1.0f,5.0f };
	for (int i = 0; i < 4; i++)
	{
		obj.push_back(SceneObject(objData[i]));
		obj[i].initiateObject(immediateContext, device, &objData[i].mesh, &objData[i].indices);
	}


	obj.push_back(SceneObject(objData[5]));
	obj[4].initiateObject(immediateContext, device, &objData[5].mesh, &objData[5].indices);
	obj[1].setWorldPos(testValues);
	obj[0].setWorldPos(testValues2);
	testValues[1] = -1;
	obj[3].setWorldPos(testValues);
	testValues[0] = 0.25f;
	testValues[1] = 0.25f;
	testValues[2] = 0.25f;

	obj[1].setScale(testValues);
	return obj;
}

void handleImGui(float bgClr[], ImGuiValues& imGuiStuff, int& submeshAmount, CamData& camData, Camera& cam, bool currentCamera[], ShadowMappingClass& shadowMap, bool& cubeMap)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	{
		bool temps[3] = { (int)imGuiStuff.imposition, (int)imGuiStuff.imnormal, (int)imGuiStuff.imcolour };
		bool wireframe = imGuiStuff.imwireframe;
		float tesselationConst = camData.tesselationConst;
		bool reflectionType = (tesselationConst >= 25.f);
		float particleSize = cam.getParticleSize();
		std::string currCam = "Main camera";
		float lightColour[3];
		lightColour[0] = shadowMap.getColour(0).x;
		lightColour[1] = shadowMap.getColour(0).y;
		lightColour[2] = shadowMap.getColour(0).z;
		float lightColour2[3];
		lightColour2[0] = shadowMap.getColour(1).x;
		lightColour2[1] = shadowMap.getColour(1).y;
		lightColour2[2] = shadowMap.getColour(1).z;
		float lightColour3[3];
		lightColour3[0] = shadowMap.getColour(2).x;
		lightColour3[1] = shadowMap.getColour(2).y;
		lightColour3[2] = shadowMap.getColour(2).z;
		float lightColour4[3];
		lightColour4[0] = shadowMap.getColour(3).x;
		lightColour4[1] = shadowMap.getColour(3).y;
		lightColour4[2] = shadowMap.getColour(3).z;
		bool isolation[4]{ shadowMap.getIsolation(0), shadowMap.getIsolation(1) , shadowMap.getIsolation(2) , shadowMap.getIsolation(3) };
		float cone[3]{ shadowMap.getCone(0), shadowMap.getCone(1), shadowMap.getCone(2) };
		float reach[3]{ shadowMap.getReach(0), shadowMap.getReach(1), shadowMap.getReach(2) };


		if (GetAsyncKeyState('1')) currentCamera[0] = currentCamera[1] = currentCamera[2] = currentCamera[3] = false;
		if (GetAsyncKeyState('2')) { currentCamera[0] = true; currentCamera[1] = currentCamera[2] = currentCamera[3] = false; }
		if (GetAsyncKeyState('3')) { currentCamera[1] = true; currentCamera[0] = currentCamera[2] = currentCamera[3] = false; }
		if (GetAsyncKeyState('4')) { currentCamera[2] = true; currentCamera[1] = currentCamera[0] = currentCamera[3] = false; }
		if (GetAsyncKeyState('5')) { currentCamera[3] = true; currentCamera[1] = currentCamera[2] = currentCamera[0] = false; }

		if (currentCamera[0] || currentCamera[1] || currentCamera[2] || currentCamera[3]) currCam = "Light camera";
		bool begun = ImGui::Begin("Project settings");
		if (begun)
		{
			ImGui::Text("Information");
			ImGui::Text("Camera position: X: %d Y: %d Z: %d", (int)cam.GetPositionFloat3().x, (int)cam.GetPositionFloat3().y, (int)cam.GetPositionFloat3().z);
			ImGui::Checkbox("Light 1: ", &currentCamera[0]);
			ImGui::Checkbox("Light 2: ", &currentCamera[1]);
			ImGui::Checkbox("Light 3: ", &currentCamera[2]);
			ImGui::Checkbox("Light 4: ", &currentCamera[3]);
			ImGui::Text("Current Camera: %s", currCam.c_str());
			ImGui::Text("");
			ImGui::Text("Light Colour");
			ImGui::ColorEdit3("Directional", lightColour);
			ImGui::ColorEdit3("Spotlight 1", lightColour2);
			ImGui::ColorEdit3("Spotlight 2", lightColour3);
			ImGui::ColorEdit3("Spotlight 3", lightColour4);
			ImGui::Text("Isolate Lights");
			ImGui::Checkbox("Directional", &isolation[0]);
			ImGui::Checkbox("Spotlight 1", &isolation[1]);
			ImGui::Checkbox("Spotlight 2", &isolation[2]);
			ImGui::Checkbox("Spotlight 3", &isolation[3]);
			ImGui::SliderFloat("Cone 1", &cone[0], 0, 50);
			ImGui::SliderFloat("Cone 2", &cone[1], 0, 50);
			ImGui::SliderFloat("Cone 3", &cone[2], 0, 50);
			ImGui::SliderFloat("Reach 1", &reach[0], 0, 50);
			ImGui::SliderFloat("Reach 2", &reach[1], 0, 50);
			ImGui::SliderFloat("Reach 3", &reach[2], 0, 50);
			ImGui::Text("");
			ImGui::Checkbox("Cube Map", &cubeMap);
			ImGui::Text("");
			ImGui::Text("Draw calls");
			ImGui::SliderInt("Submeshes", &submeshAmount, 0, 10);
			ImGui::Text("");
			ImGui::Text("Deferred Rendering");
			ImGui::Checkbox("Position", &temps[0]);
			ImGui::Checkbox("Normal", &temps[1]);
			ImGui::Checkbox("Colour", &temps[2]);

			ImGui::Text("");
			ImGui::Text("Tesselation");
			ImGui::Checkbox("Wireframe", &wireframe);
			ImGui::SliderFloat("Min Distance", &tesselationConst, 0.1f, 50.f);

			ImGui::Text("");
			ImGui::Text("Particles");
			ImGui::SliderFloat("Particle size", &particleSize, 0.01f, 0.5f);

			ImGui::Text("");
			ImGui::Text("Cubemap");
			ImGui::Checkbox("Reflection", &reflectionType);
		}
		ImGui::End();
		DirectX::XMFLOAT3 clr(lightColour[0], lightColour[1], lightColour[2]);
		DirectX::XMFLOAT3 clr2(lightColour2[0], lightColour2[1], lightColour2[2]);
		DirectX::XMFLOAT3 clr3(lightColour3[0], lightColour3[1], lightColour3[2]);
		DirectX::XMFLOAT3 clr4(lightColour4[0], lightColour4[1], lightColour4[2]);
		shadowMap.setLightColour(clr, 0);
		shadowMap.setLightColour(clr2, 1);
		shadowMap.setLightColour(clr3, 2);
		shadowMap.setLightColour(clr4, 3);
		for (int i = 0; i < 4; i++) shadowMap.setIsolation(isolation[i], i);
		for (int i = 0; i < 3; i++) { shadowMap.setCone(cone[i], i); shadowMap.setReach(reach[i], i); }
		imGuiStuff.imposition = temps[0];
		imGuiStuff.imnormal = temps[1];
		imGuiStuff.imcolour = temps[2];
		imGuiStuff.imwireframe = wireframe;
		camData.tesselationConst = tesselationConst;
		if (reflectionType && tesselationConst < 25) camData.tesselationConst = 25.f;
		else if (!reflectionType && tesselationConst >= 25) camData.tesselationConst = 20.f;

		cam.changeParticleSize(particleSize);
	}
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
