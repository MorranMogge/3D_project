#include "ConstantBuffer.h"

ConstantBuffer::ConstantBuffer()
{
    world = XMMatrixIdentity();
    world *= XMMatrixTranslation(0.f, 0.f, -1.f);
    world *= XMMatrixRotationY(0);
    world = XMMatrixTranspose(world);
    XMStoreFloat4x4(&matrices.worldMatrix, world);

    XMVECTOR eyePos = XMVectorSet(0.0f, 0.0f, -3.0f, 0.0f);
    XMVECTOR focusPos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR upDir = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    viewAndProj = XMMatrixLookAtLH(eyePos, focusPos, upDir) * XMMatrixPerspectiveFovLH(0.25f * XM_2PI, 1024.f / 576.f, 0.1f, 100.0f);
    viewAndProj = XMMatrixTranspose(viewAndProj);
    XMStoreFloat4x4(&matrices.projectionMatrix, viewAndProj);
}

void ConstantBuffer::NewUpdate(float inrotation)
{
    static float rotation = inrotation * 0.25f / 360.f * XM_2PI;
    world = XMLoadFloat4x4(&matrices.worldMatrix);
    world = XMMatrixTranspose(world);
    world *= XMMatrixTranslation(0.f, 0.f, -1.f);
    world *= XMMatrixRotationY(rotation);
    world *= XMMatrixTranslation(0.f, 0.f, 1.f);
    world = XMMatrixTranspose(world);
    XMStoreFloat4x4(&matrices.worldMatrix, world); 

    XMStoreFloat4x4(&matrices.worldMatrix, world);
}

void ConstantBuffer::Update(ID3D11DeviceContext* immediateContext, ID3D11Buffer*& constantBuffer)
{
   /* static float rotation = 0.25f / 360.f * XM_2PI;
    world = XMLoadFloat4x4(&matrices.worldMatrix);
    world = XMMatrixTranspose(world);
    world *= XMMatrixTranslation(0.f, 0.f, -1.f);
    world *= XMMatrixRotationY(rotation);
    world *= XMMatrixTranslation(0.f, 0.f, 1.f);
    world = XMMatrixTranspose(world);
    XMStoreFloat4x4(&matrices.worldMatrix, world);*/

    //XMStoreFloat4x4(&matrices.worldMatrix, world);

    D3D11_MAPPED_SUBRESOURCE mapRes;
    ZeroMemory(&mapRes, sizeof(D3D11_MAPPED_SUBRESOURCE));
    HRESULT hr = immediateContext->Map(constantBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mapRes);
    memcpy(mapRes.pData, &matrices, sizeof(matrices));
    immediateContext->Unmap(constantBuffer, 0);
}

bool ConstantBuffer::setConstBuf(ID3D11Device* device, ID3D11Buffer*& constantBuffer)
{
    D3D11_BUFFER_DESC constBuf;
    constBuf.ByteWidth = sizeof(matrices);
    constBuf.Usage = D3D11_USAGE_DYNAMIC;
    constBuf.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constBuf.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    constBuf.MiscFlags = 0;
    constBuf.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA bufData;
    bufData.pSysMem = &matrices;
    bufData.SysMemPitch = 0;
    bufData.SysMemSlicePitch = 0;

    HRESULT hr = device->CreateBuffer(&constBuf, &bufData, &constantBuffer);

    return !FAILED(hr);
}

void ConstantBuffer::setXValue(float xPos)
{
    matrices.worldMatrix._14 = xPos;
}

void ConstantBuffer::setYValue(float yPos)
{
    matrices.worldMatrix._24 = yPos;
}

void ConstantBuffer::setZValue(float zPos)
{
    matrices.worldMatrix._34 = zPos;
}
