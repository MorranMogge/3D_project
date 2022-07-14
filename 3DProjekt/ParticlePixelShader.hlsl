struct PSin
{
    float4 position : SV_POSITION;
    float4 newPos : Position;
};

float4 main(PSin input) : SV_TARGET
{
    return float4(input.newPos.x, input.newPos.y, input.newPos.z, 1.0);
}