#include "CubemapClass.h"

void CubemapClass::setRot(int index)
{
    //Order Matters
    //0: +x     1:  -x
    //2: +y     3:  -y
    //4: +z     5:  -z

    if (index == 0) cam.SetRotation(0.0f, NINETYDEGREES, 0.0f, immediateContext);
    else if (index == 1) cam.SetRotation(0.0f, -NINETYDEGREES, 0.0f, immediateContext);
    else if (index == 2) cam.SetRotation(-NINETYDEGREES, 0.0f, 0.0f, immediateContext);
    else if (index == 3) cam.SetRotation(NINETYDEGREES, 0.0f, 0.0f, immediateContext);
    else if (index == 4) cam.SetRotation(0.0f, 0.0f, 0.0f, immediateContext);
    else if (index == 5) cam.SetRotation(0.0f, -NINETYDEGREES*2, 0.0f, immediateContext);
}

void CubemapClass::updateWrldMtx()
{
    worldMtx = DirectX::XMMatrixIdentity() * DirectX::XMMatrixTranslation(cam.GetPositionFloat3().x, cam.GetPositionFloat3().y, cam.GetPositionFloat3().z);// *DirectX::XMMatrixTranslation(particles[index].x, particles[index].y, particles[index].z);
    worldMtx = DirectX::XMMatrixTranspose(worldMtx);
    DirectX::XMStoreFloat4x4(&wrlMtx, worldMtx);

    D3D11_MAPPED_SUBRESOURCE mapRes;
    ZeroMemory(&mapRes, sizeof(D3D11_MAPPED_SUBRESOURCE));
    HRESULT hr = immediateContext->Map(constBuf, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mapRes);
    memcpy(mapRes.pData, &wrlMtx, sizeof(wrlMtx));
    immediateContext->Unmap(constBuf, 0);
}

void CubemapClass::updateCamBuffer()
{
    D3D11_MAPPED_SUBRESOURCE subCam = {};
    camData.cameraPosition = cam.GetPositionFloat3();
    immediateContext->Map(camBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subCam);
    memcpy(subCam.pData, &camData, sizeof(CamData));
    immediateContext->Unmap(camBuffer, 0);
}

bool CubemapClass::setUpSRVAndRTV(ID3D11Device* device)
{
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = CUBEMAPRESOLUTION;
    desc.Height = CUBEMAPRESOLUTION;
    desc.ArraySize = AMOUNTOFSIDESACUBEHAS;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.CPUAccessFlags = 0;
    desc.MipLevels = 1;

    ID3D11Texture2D* testTex;
    HRESULT hr = device->CreateTexture2D(&desc, NULL, &testTex);
    if (FAILED(hr)) { std::cout << "Failed to create cube map texture2D\n"; return false; }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = desc.Format;
    //srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    /*srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Texture2DArray.FirstArraySlice = 0;
    srvDesc.Texture2DArray.MostDetailedMip = 0;
    srvDesc.Texture2DArray.MipLevels = 1;
    srvDesc.Texture2DArray.ArraySize = AMOUNTOFSIDESACUBEHAS;*/


    hr = device->CreateShaderResourceView(testTex, NULL, &srv);
    if (FAILED(hr)) { std::cout << "Failed to create cube map srv\n"; return false; }

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
    rtvDesc.Texture2DArray.ArraySize = 1;
    rtvDesc.Texture2DArray.MipSlice = 0;

    for (int i = 0; i < AMOUNTOFSIDESACUBEHAS; i++)
    {
        rtvDesc.Texture2DArray.FirstArraySlice = (UINT)i;
        hr = device->CreateRenderTargetView(testTex, &rtvDesc, &rtv[i]);
        if (FAILED(hr)) { std::cout << "Failed to create cube map rtv\n"; return false; }
    }

    testTex->Release();
	return true;
}

bool CubemapClass::setUpShaders(ID3D11Device* device, std::string& vShaderByteCode)
{
    std::string shaderData;
    std::ifstream reader;
    reader.open("../x64/Debug/PixelShader.cso", std::ios::binary | std::ios::ate);
    if (!reader.is_open())
    {
        std::cout << "Could not open pixel shader file!" << std::endl;
        return false;
    }

    reader.seekg(0, std::ios::end);
    shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
    reader.seekg(0, std::ios::beg);

    shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

    if (FAILED(device->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pShader)))
    {
        std::cerr << "Failed to create pixel shader!" << std::endl;
        return false;
    }

    shaderData.clear();
    reader.close();

    reader.open("../x64/Debug/CubeMapVertex.cso", std::ios::binary | std::ios::ate);
    if (!reader.is_open())
    {
        std::cout << "Could not open vertex shader file!" << std::endl;
        return false;
    }

    reader.seekg(0, std::ios::end);
    shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
    reader.seekg(0, std::ios::beg);

    shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

    if (FAILED(device->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &cubeVshader)))
    {
        std::cerr << "Failed to create vertex shader!" << std::endl;
        return false;
    }

    vShaderByteCode = shaderData;
    shaderData.clear();
    reader.close();


    reader.open("../x64/Debug/CubeMapPixel.cso", std::ios::binary | std::ios::ate);
    if (!reader.is_open())
    {
        std::cout << "Could not open pixel shader file!" << std::endl;
        return false;
    }

    reader.seekg(0, std::ios::end);
    shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
    reader.seekg(0, std::ios::beg);

    shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

    if (FAILED(device->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &cubePshader)))
    {
        std::cerr << "Failed to create pixel shader!" << std::endl;
        return false;
    }
    return true;
}

bool CubemapClass::setUpCamBuffer(ID3D11Device* device)
{
    cam.SetPosition(0 + 5 * cos(timer * DirectX::XM_2PI), 5, 0 + 5 * std::sin(timer * DirectX::XM_2PI));

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

bool CubemapClass::setUpDepthStencil(ID3D11Device* device)
{
    D3D11_TEXTURE2D_DESC textureDesc;
    textureDesc.Width = CUBEMAPRESOLUTION;
    textureDesc.Height = CUBEMAPRESOLUTION;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    if (FAILED(device->CreateTexture2D(&textureDesc, nullptr, &dsTexture)))
    {
        std::cerr << "Failed to create depth stencil texture!" << std::endl;
        return false;
    }

    HRESULT hr = device->CreateDepthStencilView(dsTexture, nullptr, &dsView);
    return !FAILED(hr);
}

bool CubemapClass::setUpVertexBuffer(ID3D11Device* device)
{
    //Setup description
    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.ByteWidth = sizeof(cubeInfo) * this->cube.size();
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    stride = sizeof(cubeInfo);
    offset = 0;

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = cube.data();
    data.SysMemPitch = 0;
    data.SysMemSlicePitch = 0;

    if ((FAILED(device->CreateBuffer(&bufferDesc, &data, &vBuffer)))) return false;
    return true;
}

bool CubemapClass::setUpInputLayout(ID3D11Device* device, const std::string& vShaderByteCode)
{
    D3D11_INPUT_ELEMENT_DESC inputDesc[2] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    HRESULT hr = device->CreateInputLayout(inputDesc, std::size(inputDesc), vShaderByteCode.c_str(), vShaderByteCode.length(), &cubeInputLayout);

    return !FAILED(hr);
}

bool CubemapClass::setUpConstBuf(ID3D11Device* device)
{
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeof(wrlMtx);
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    HRESULT hr = device->CreateBuffer(&bufferDesc, 0, &constBuf);

    this->updateWrldMtx();
    return !FAILED(hr);
}

CubemapClass::CubemapClass()
    :camBuffer(nullptr), immediateContext(nullptr), pShader(nullptr), srv(nullptr), dsView(nullptr), timer(0.0f), increase(1)
{
    camData.cameraPosition = DirectX::XMFLOAT3(5, 5, 5);
    camData.tesselationConst = 0;
    for (int i = 0; i < std::size(rtv); i++)
    {
        rtv[i] = nullptr;
    }
}

CubemapClass::~CubemapClass()
{
    //Shaders
    pShader->Release();

    //Resources
    srv->Release();
    dsTexture->Release();
    camBuffer->Release();

    //Other
    dsView->Release();
    for (int i = 0; i < AMOUNTOFSIDESACUBEHAS; i++)
    {
        rtv[i]->Release();
    }

    cubeVshader->Release();
    cubePshader->Release();
    cubeInputLayout->Release();
    vBuffer->Release();
    constBuf->Release();
}

bool CubemapClass::initiateCubemap(ID3D11DeviceContext* immediateContext, ID3D11Device* device, ID3D11DepthStencilView* dsView)
{
    std::string vShaderByteCode;
    this->immediateContext = immediateContext;
    if ((this->immediateContext) == nullptr)		        return false;
    if (!this->setUpShaders(device, vShaderByteCode))       return false;
    if (!this->setUpInputLayout(device, vShaderByteCode))   return false;
    if (!this->setUpDepthStencil(device))                   return false;
    if (!this->setUpSRVAndRTV(device))                      return false;
    if (!cam.CreateCBuffer(immediateContext, device))       return false;
    if (!this->setUpVertexBuffer(device))                   return false;
    if (!this->setUpConstBuf(device))                       return false;
    if (!this->setUpCamBuffer(device))                      return false;
    
    cam.ChangeProjectionMatrix(XM_2PI/4, 1, 0.1, 100);
	return true;
}

void CubemapClass::createCube(newObjThing vertices)
{
    for (int i = 0; i < vertices.mesh.size(); i++)
    {
        cube.push_back(cubeInfo());
        cube[i].pos = DirectX::XMFLOAT3(vertices.mesh[i].pos[0], vertices.mesh[i].pos[1], vertices.mesh[i].pos[2]);
        cube[i].normal = DirectX::XMFLOAT3(vertices.mesh[i].n[0], vertices.mesh[i].n[1], vertices.mesh[i].n[2]);
    }
}

void CubemapClass::draw(std::vector<SceneObject>& o, ParticleHandler& pHandler, ID3D11DepthStencilView* &view)
{
    float clearColour[4]{ 0.0,0.0,1.0,0.0 }; //Blue sky

    for (int i = 0; i < AMOUNTOFSIDESACUBEHAS; i++)
    {
        immediateContext->ClearRenderTargetView(rtv[i], clearColour);
    }

    immediateContext->HSSetShader(nullptr, nullptr, 0);	//Since we use tesselation for LOD
    immediateContext->DSSetShader(nullptr, nullptr, 0); //-||-

    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    this->updateCamBuffer(); //Update Cam
    immediateContext->PSSetShader(pShader, nullptr, 0);

    immediateContext->PSSetConstantBuffers(0, 1, &camBuffer);

    //Draw objects from the cubes perspective
    for (int c = 0; c < AMOUNTOFSIDESACUBEHAS; c++)
    {
        immediateContext->ClearRenderTargetView(rtv[c], clearColour);
        immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
        immediateContext->OMSetRenderTargets(1, &rtv[c], dsView);
        this->setRot(c);
        for (int i = 0; i < o.size(); i++)
        {
            o[i].draw();
        }
    }
    
    immediateContext->VSSetShader(nullptr, nullptr, 0);
    immediateContext->PSSetShader(nullptr, nullptr, 0);

   //Draw particles from the cubes perspective
   for (int c = 0; c < AMOUNTOFSIDESACUBEHAS; c++)
   {
        immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
        immediateContext->OMSetRenderTargets(1, &rtv[c], dsView);
        this->setRot(c);
        cam.sendView(immediateContext);
        pHandler.drawParticles(&cam);
    }
    
}

void CubemapClass::drawCube()
{
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    immediateContext->VSSetShader(cubeVshader, nullptr, 0);
    immediateContext->PSSetShader(cubePshader, nullptr, 0);
    immediateContext->IASetInputLayout(cubeInputLayout);
    immediateContext->VSSetConstantBuffers(0, 1, &constBuf);
    immediateContext->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);
    immediateContext->PSSetShaderResources(0, 1, &srv);
    this->updateWrldMtx();

    immediateContext->Draw(cube.size(), 0);

    immediateContext->VSSetShader(nullptr, nullptr, 0);
    immediateContext->PSSetShader(nullptr, nullptr, 0);

    /*timer = timer + 0.2f *(1.f / 144.0f);
    cam.SetPosition(0 + 5*cos(timer * XM_2PI), 5 ,0 + 5*std::sin(timer * XM_2PI));
    if (timer >= 1.0f || timer <= 0.0f) timer = 0;*/
}
