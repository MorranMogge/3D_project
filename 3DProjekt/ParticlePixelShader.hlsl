struct PSin
{
    float4 position : SV_POSITION;
    float4 newPos : Position;
};

float4 main(PSin input) : SV_TARGET
{
	//float4 clr = float4(input.newPos.x + 1, input.newPos.y + 1, input.newPos.z, 1.0)/2; //Returns colour depending on the particles position
	//return clr;
    return float4(input.newPos.x, input.newPos.y, input.newPos.z, 1.0);
}