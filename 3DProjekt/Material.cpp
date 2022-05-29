#include "Material.h"
#include "memoryLeakChecker.h"

Material::Material()
    :materialInfo(material
    (
        0.75f,  //ambient
        0.75f,  //diffuse
        0.95f,  //specular
        110.0f  //shininess
    ) 
    )
{
}

bool Material::setMaterial(ID3D11Device* device, ID3D11Buffer*& lightBuffer)
{
    D3D11_BUFFER_DESC constBuf;
    constBuf.ByteWidth = sizeof(materialInfo);
    constBuf.Usage = D3D11_USAGE_DYNAMIC;
    constBuf.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constBuf.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    constBuf.MiscFlags = 0;
    constBuf.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA bufData;
    bufData.pSysMem = &materialInfo;
    bufData.SysMemPitch = 0;
    bufData.SysMemSlicePitch = 0;

    HRESULT hr = device->CreateBuffer(&constBuf, &bufData, &lightBuffer);

    return !FAILED(hr);
}
