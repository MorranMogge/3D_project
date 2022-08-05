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
    //FIXA MED PROJECTION
    output.position = patch[0].position * uvw.x + patch[1].position * uvw.y + patch[2].position * uvw.z;
    output.worldPos = patch[0].worldPos * uvw.x + patch[1].worldPos * uvw.y + patch[2].worldPos * uvw.z;
    output.uv = patch[0].uv * uvw.x + patch[1].uv * uvw.y + patch[2].uv * uvw.z;
    output.normal = patch[0].normal * uvw.x + patch[1].normal * uvw.y + patch[2].normal * uvw.z;
    output.lightPos1 = patch[0].lightPos1 * uvw.x + patch[1].lightPos1 * uvw.y + patch[2].lightPos1 * uvw.z;
    output.lightPos2 = patch[0].lightPos2 * uvw.x + patch[1].lightPos2 * uvw.y + patch[2].lightPos2 * uvw.z;
    output.lightPos3 = patch[0].lightPos3 * uvw.x + patch[1].lightPos3 * uvw.y + patch[2].lightPos3 * uvw.z;
    output.lightPos4 = patch[0].lightPos4 * uvw.x + patch[1].lightPos4 * uvw.y + patch[2].lightPos4 * uvw.z;
    return output;
}
