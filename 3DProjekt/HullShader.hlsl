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

cbuffer cameraPos : register(b0)
{
    float3 cameraPosition;
    float tesselationConst;
};

HS_CONSTANT_DATA_OUTPUT ConstantPatchFunction(InputPatch<VertexShaderOutput, 3> inputPatch, uint patchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT output;
    float tessFactor;
    float3 middlePoint;// = (inputPatch[0].newPos + inputPatch[0].newPos + inputPatch[0].newPos) / 3;
    float3 distToTriangle;// = cameraPosition - middlePoint;
    float dist;// = distToTriangle.x * distToTriangle.x + distToTriangle.y * distToTriangle.y + distToTriangle.z * distToTriangle.z;
    float sizefactor;
    float3 distToEdge;
    float maxdistance = tesselationConst*tesselationConst;
    float maxTesselation = 10.0f;
   
    
    middlePoint = (inputPatch[0].newPos + inputPatch[1].newPos + inputPatch[2].newPos) / 3;
    distToEdge = inputPatch[0].newPos.xyz - middlePoint;
    sizefactor = (distToEdge.x * distToEdge.x + distToEdge.y * distToEdge.y + distToEdge.z * distToEdge.z);
    distToTriangle = cameraPosition - middlePoint;
    dist = distToTriangle.x * distToTriangle.x + distToTriangle.y * distToTriangle.y + distToTriangle.z * distToTriangle.z;
    //if (sizefactor > dist)
    //    dist = maxdistance;
    //dist /= tesselationConst;
    //sizefactor /= dist;
    if (dist >= maxdistance)
        tessFactor = 1.0f;
    else
        tessFactor = (maxTesselation * (maxdistance - dist) / maxdistance);
    for (int i = 0; i < 3; ++i) //SET DYNAMICALLY
    {
        output.edgeTessFactor[i] = tessFactor;
    }
    
    output.insideTessFactor = (output.edgeTessFactor[0] + output.edgeTessFactor[1] + output.edgeTessFactor[2])/3;
    
    return output;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantPatchFunction")]

VertexShaderOutput PerPatchFunction(InputPatch<VertexShaderOutput, 3> inputPatch, 
                                    uint i : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
    return inputPatch[i];
}