//struct PSinput
//{
//    float4 position : SV_Target0;
//    float4 colour : SV_Target1;
//    float4 normal : SV_Target2;
//};


struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float4 newPos : NEWPOSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    ///return input.normal;
    float4 yes = input.newPos;
    return yes;
}