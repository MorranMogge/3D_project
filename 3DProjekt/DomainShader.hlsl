struct DomainShaderOutput
{
    float4 position : SV_POSITION;
    float4 newPos : NEWPOSITION;
    float4 normal : NORMAL;
    float2 uv : UV;
};

struct HS_CONSTANT_DATA_OUTPUT
{
    float edgeTessFactor[3] : SV_TessFactor;
    float insideTessFactor : SV_InsideTessFactor;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float4 newPos : NEWPOSITION;
    float4 normal : NORMAL;
    float2 uv : UV;
};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
DomainShaderOutput main(HS_CONSTANT_DATA_OUTPUT input, float3 uvw : SV_DomainLocation,
                        const OutputPatch<VertexShaderOutput, 3> patch)
{
    DomainShaderOutput output;
    
    output.position = patch[0].position * uvw.x + patch[1].position * uvw.y + patch[2].position * uvw.z;
    output.newPos = patch[0].newPos * uvw.x + patch[1].newPos * uvw.y + patch[2].newPos * uvw.z;
    output.uv = patch[0].uv * uvw.x + patch[1].uv * uvw.y + patch[2].uv * uvw.z;
    output.normal = patch[0].normal * uvw.x + patch[1].normal * uvw.y + patch[2].normal * uvw.z;
    return output;
}
