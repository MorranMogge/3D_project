cbuffer Matrices : register(b0)
{
    float4x4 viewProj;
}

cbuffer lightMtx1 : register(b1)
{
    float4x4 lightViewProj1;
}

cbuffer lightMtx2 : register(b2)
{
    float4x4 lightViewProj2;
}

cbuffer lightMtx3 : register(b3)
{
    float4x4 lightViewProj3;
}

cbuffer lightMtx4 : register(b4)
{
    float4x4 lightViewProj4;
}

struct DomainShaderOutput
{
    float4 position : SV_POSITION;
    float4 worldPos : WORLDPOSITION;
    float4 normal : NORMAL;
    float2 uv : UV;
    float4 lightPos1 : LIGHTPOS1;
    float4 lightPos2 : LIGHTPOS2;
    float4 lightPos3 : LIGHTPOS3;
    float4 lightPos4 : LIGHTPOS4;
};

struct HS_CONSTANT_DATA_OUTPUT
{
    float edgeTessFactor[3] : SV_TessFactor;
    float insideTessFactor : SV_InsideTessFactor;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float4 worldPos : WORLDPOSITION;
    float4 normal : NORMAL;
    float2 uv : UV;
    float4 lightPos1 : LIGHTPOS1;
    float4 lightPos2 : LIGHTPOS2;
    float4 lightPos3 : LIGHTPOS3;
    float4 lightPos4 : LIGHTPOS4;
};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
DomainShaderOutput main(HS_CONSTANT_DATA_OUTPUT input, float3 uvw : SV_DomainLocation,
                        const OutputPatch<VertexShaderOutput, 3> patch)
{
    DomainShaderOutput output;

    output.worldPos = patch[0].worldPos * uvw.x + patch[1].worldPos * uvw.y + patch[2].worldPos * uvw.z;
    output.uv = patch[0].uv * uvw.x + patch[1].uv * uvw.y + patch[2].uv * uvw.z;
    output.normal = patch[0].normal * uvw.x + patch[1].normal * uvw.y + patch[2].normal * uvw.z;
    
    output.position = mul(output.worldPos, viewProj);
    output.lightPos1 = mul(output.worldPos, lightViewProj1);
    output.lightPos2 = mul(output.worldPos, lightViewProj2);
    output.lightPos3 = mul(output.worldPos, lightViewProj3);
    output.lightPos4 = mul(output.worldPos, lightViewProj4);
    
    return output;
}
