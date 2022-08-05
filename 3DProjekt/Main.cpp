#include <Windows.h>
#include <d3d11.h>
#include <iostream>
#include <DirectXMath.h>
#include <chrono>
#include <vector>
#include <time.h>

#include "Project.h"




using namespace DirectX;

void newImGui(float bgClr[], ImGuiValues& imGuiStuff, bool& noIndexing, CamData& camData, Camera& cam, bool currentCamera[], ShadowMappingClass& shadowMap);
bool createImGuiBuffer(ID3D11Device* device, ID3D11Buffer*& imGuiBuffer, struct ImGuiValues& imGuiStuff);
bool createCamBuffer(ID3D11Device* device, ID3D11Buffer*& camBuffer, struct CamData& camData);
void Render(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* &rtv, ID3D11DepthStencilView* dsView, D3D11_VIEWPORT& viewport, ID3D11SamplerState* samplerState, Camera& camera, CamData& camData, ID3D11Buffer*& camBuffer, std::vector<SceneObject> &objects,bool &test, float clearColour[], ImGuiValues &imGuiStuff, ID3D11Buffer* imGuiBuffer, ParticleHandler &particles, TesselatorClass& tesselator, DeferredRenderer& deferred, CubemapClass& cubemap, FrustumCuller& culler, Camera otherView[], ShadowMappingClass& shadowMap, bool currentCamera[]);
std::vector<SceneObject> setUpScene(ID3D11DeviceContext* immediateContext, ID3D11Device* device, std::vector<newObjThing> objData);

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
	bool x = true;
	bool rotation = false;
	bool normal = false;
	bool test = true;
	float bgColour[4] = { 0.0, 0.0, 0.0, 0.0 };
	bool currentCamera[4]{ false };

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
	ID3D11InputLayout		* inputLayout;
	ID3D11SamplerState		* samplerState;
	D3D11_VIEWPORT			  viewport;

	//Deferred rendering
	ID3D11Texture2D			* dsTexture;
	ID3D11ShaderResourceView* missingTexture;
	

	ID3D11Buffer			* values;

	//Other
	ID3D11Buffer* camBuf;
	ID3D11Buffer* imGuiBuffer;
	
	//Helpers
	ParticleHandler particles;
	TesselatorClass tesselator;
	DeferredRenderer deferred(WIDTH, HEIGHT);
	CubemapClass cubemap;
	FrustumCuller culler;
	ShadowMappingClass shadowMap;

	//Classes
	std::vector<newObjThing> newObj;
	materialChecker material;
	std::vector<SceneObject> objects;
	Camera camera;
	Camera otherView[4];
	CamData camData;
	ImGuiValues imGuiStuff;

	if (!SetupD3D11(WIDTH, HEIGHT, window, device, immediateContext, swapChain, rtv, dsTexture, dsView, viewport))
	{
		std::cerr << "Failed to setup d3d11!" << std::endl;
		return -1;
	}

	if (!SetupPipeline(device, samplerState))
	{
		std::cerr << "Failed to setup pipeline!" << std::endl;
		return -1;
	}

	camera.SetPosition(0, 0, -3);
	createCamBuffer(device, camBuf, camData);
	camera.CreateCBuffer(immediateContext, device);
	for (int i = 0; i < 4; i++) otherView[i].CreateCBuffer(immediateContext, device);
	/*otherView.SetPosition(0, 50, 0);
	otherView.SetRotation(XM_PI / 2, 0.0f, 0.0f, immediateContext);*/
	/*otherView[0].SetPosition(0, 50, 0);
	otherView[0].SetRotation(XM_PI / 2, 0, 0, immediateContext);
	otherView[1].SetPosition(0, 25, -50);
	otherView[1].SetRotation(XM_PI / 4, 0, 0, immediateContext);
	otherView[2].SetPosition(0, 50, 0);
	otherView[2].SetRotation(XM_PI / 4, XM_PI / 4, 0, immediateContext);
	otherView[3].SetPosition(0, 50, 0);
	otherView[3].SetRotation(XM_PI / 4, -XM_PI / 4, 0, immediateContext);*/

	createImGuiBuffer(device, imGuiBuffer, imGuiStuff);
	
	newerReadModels(device, missingTexture, material, newObj);
	cubemap.createCube(newObj[4]);

	//Initiates all the handlers
	particles.InitiateHandler(immediateContext, device, &camera);
	tesselator.setUpTesselator(immediateContext, device, &camera);
	deferred.initiateDeferredRenderer(immediateContext, device, swapChain, dsView, &camera, &imGuiStuff);
	cubemap.initiateCubemap(immediateContext, device, dsView);
	shadowMap.initiateShadowMapping(immediateContext, device);
	camera.createFrustum();

	shadowMap.setCameraPosAndRot(otherView);
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, immediateContext);



	MSG msg = {};

	for (int i = 0; i < 4; i++)
	{
		objects.push_back(SceneObject(newObj[i]));
		objects[i].initiateObject(immediateContext, device, &newObj[i].mesh, &newObj[i].indices);
	}


	objects[1].setWorldPos(testValues);
	objects[0].setWorldPos(testValues2);
	testValues[1] = -1;

	testValues[0] = 1.0f;
	testValues[1] = 1.0f;
	testValues[2] = 1.0f;

	objects[1].setScale(testValues);
	//objects = setUpScene(immediateContext, device, objects, newObj);
	objects.push_back(SceneObject(newObj[0]));
	objects[4].initiateObject(immediateContext, device, &newObj[0].mesh, &newObj[0].indices);

	testValues[0] = 2.0f;
	testValues[1] = 1.0f;
	testValues[2] = 2.0f;
	//objects[4].setScale(testValues);

	int amount = objects.size();
	int counter = 0;
	for (int i = objects.size(); i < 20 + amount; i++)
	{
		objects.push_back(SceneObject(newObj[0]));
		objects[i].initiateObject(immediateContext, device, &newObj[0].mesh, &newObj[0].indices);
		objects[i].setWorldPos(cubePositions[counter++]);
	}

	for (int i = 0; i < 1; i++)
	{
		objects.push_back(SceneObject(newObj[0]));
		objects[objects.size()-1].initiateObject(immediateContext, device, &newObj[0].mesh, &newObj[0].indices);
		if (i == 0) 
		{ 
			objects[objects.size() - 1].setWorldPos(DirectX::XMFLOAT3(0, 25, -50)); 
			objects[objects.size() - 1].setRot(DirectX::XMFLOAT3(XM_PI/4, 0, 0));
		}
	}

	for (auto& o : objects)
	{
		o.setBoundingBox();
		if (!o.setVertexBuffer(device))
		{
			return -6;
		}
	}

	int quadTreeSize = 64;
	culler.initiateCuller(&objects, quadTreeSize, quadTreeSize);

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
				samplerState, camera, camData, camBuf, objects, test, bgColour, imGuiStuff, imGuiBuffer, particles, tesselator, deferred, cubemap, culler, otherView, shadowMap, currentCamera);
			

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
	samplerState->Release();


	//Buffers
	camBuf->Release();
	imGuiBuffer->Release();

	dsTexture->Release();
	
	
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
	camData.tesselationConst = 20.0f;

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
	imGuiStuff.imwireframe = false;

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

void Render
(
	ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* &rtv, ID3D11DepthStencilView* dsView,
	D3D11_VIEWPORT& viewport, ID3D11SamplerState* samplerState, Camera& camera, CamData& camData,
	ID3D11Buffer*& camBuffer, std::vector<SceneObject> &objects, bool &test, float clearColour[], 
	ImGuiValues &imGuiStuff, ID3D11Buffer* imGuiBuffer, ParticleHandler& particles, TesselatorClass& tesselator, 
	DeferredRenderer& deferred, CubemapClass& cubemap, FrustumCuller& culler, Camera otherView[], ShadowMappingClass& shadowMap, 
	bool currentCamera[]
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
		if (&objects[4] == culledObjects[i]) break;
		culledObjects.push_back(&objects[4]);
	}
	
	objects[4].setWorldPos(camera.GetPositionFloat3());
	objects[4].setRot(camera.GetRotationFloat3());

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
		culledObjects[i]->draw(test);
	}
	/*for (int i = 0; i < objects.size(); i++)
	{
		objects[i].draw(test);
	}*/

	//cubemap.draw(objects, particles, dsView);
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
	//cubemap.drawCube();
	if (currentCamera[0]) particles.drawParticles(&otherView[0]);
	else if (currentCamera[1]) particles.drawParticles(&otherView[1]);
	else if (currentCamera[2]) particles.drawParticles(&otherView[2]);
	else if (currentCamera[3]) particles.drawParticles(&otherView[3]);
	else particles.drawParticles();

	particles.updateParticles();

	//ImGui
	newImGui(clearColour, imGuiStuff, test, camData, camera, currentCamera, shadowMap);
}

std::vector<SceneObject> setUpScene(ID3D11DeviceContext* immediateContext, ID3D11Device* device, std::vector<newObjThing> objData)
{
	std::vector<SceneObject> obj;
	float testValues[3] = { 0.0f,10.0f,0.0f };
	float testValues2[3] = { 10.0f,1.0f,5.0f };
	for (int i = 0; i < 4; i++)
	{
		obj.push_back(SceneObject(objData[i]));
		obj[i].initiateObject(immediateContext, device, &objData[i].mesh, &objData[i].indices);
		//objects[i].setBoundingBox();
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

void newImGui(float bgClr[], ImGuiValues& imGuiStuff, bool &noIndexing, CamData& camData, Camera& cam, bool currentCamera[], ShadowMappingClass& shadowMap)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	{
		bool temps[3] = { (int)imGuiStuff.imposition, (int)imGuiStuff.imnormal, (int)imGuiStuff.imcolour };
		bool wireframe = imGuiStuff.imwireframe;
		float tesselationConst = camData.tesselationConst;
		bool reflectionType = (tesselationConst>=25.f);
		float particleSize = cam.getParticleSize();
		std::string currCam = "Main camera";
		float lightDir[3];
		lightDir[0] = shadowMap.getDir(0).x;
		lightDir[1] = shadowMap.getDir(0).y;
		lightDir[2] = shadowMap.getDir(0).z;

		if (GetAsyncKeyState('1')) currentCamera[0] = currentCamera[1] = currentCamera[2] = currentCamera[3] = false;
		if (GetAsyncKeyState('2')) { currentCamera[0] = true; currentCamera[1] = currentCamera[2] = currentCamera[3] = false; }
		if (GetAsyncKeyState('3')) { currentCamera[1] = true; currentCamera[0] = currentCamera[2] = currentCamera[3] = false; }
		if (GetAsyncKeyState('4')) { currentCamera[2] = true; currentCamera[1] = currentCamera[0] = currentCamera[3] = false; }
		if (GetAsyncKeyState('5')) { currentCamera[3] = true; currentCamera[1] = currentCamera[2] = currentCamera[0] = false; }

		if (currentCamera[0] || currentCamera[1] || currentCamera[2] || currentCamera[3]) currCam = "Light camera";
		bool begun = ImGui::Begin("Testing");
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
			ImGui::SliderFloat3("Light Direction", lightDir, -1, 1);
			ImGui::Text("");
			ImGui::ColorEdit3("BG colour", bgClr);
			ImGui::Text("");
			ImGui::Text("Draw calls");
			ImGui::Checkbox("No indexing", &noIndexing);
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
		DirectX::XMFLOAT3 dir(lightDir[0], lightDir[1], lightDir[2]);
		shadowMap.setLightDirection(dir, 0);
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
