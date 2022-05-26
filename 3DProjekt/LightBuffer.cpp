#include "LightBuffer.h"
#include <array>



LightBuffer::LightBuffer()
    :lightB
    (    
        lightBuffer
        (
        //Light part
        { 0.2f, 0.2f,  0.2f},     //ambient
        { 0.75f, 0.75f, 0.75f},   //diffuse
        { 1.0f, 1.0f, 1.0f},      //specular
        { 0.0f, 0.0f, -6.0f },    //Light pos

        //Camera
        { 0.0f, 0.0f, -3.0f }     //Camera pos
        )
    )
{
}



bool LightBuffer::setLightBuffer(ID3D11Device* device, ID3D11Buffer*& lightBuffer)
{
    D3D11_BUFFER_DESC constBuf;
    constBuf.ByteWidth = sizeof(lightB);
    constBuf.Usage = D3D11_USAGE_DYNAMIC;
    constBuf.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constBuf.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    constBuf.MiscFlags = 0;
    constBuf.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA bufData;
    bufData.pSysMem = &lightB;
    bufData.SysMemPitch = 0;
    bufData.SysMemSlicePitch = 0;

    HRESULT hr = device->CreateBuffer(&constBuf, &bufData, &lightBuffer);

    return !FAILED(hr);
}
