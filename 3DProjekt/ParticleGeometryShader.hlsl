struct PSin
{
	float4 position : SV_POSITION;
    float4 newPos : Position;
};

cbuffer Matrices : register(b0)
{
    float4x4 view;
    float4x4 proj;
}

cbuffer Vectors : register(b1)
{
    float3 upVector;
    float padding1;
    float3 forwardVector;
    float padding2;
}

struct GSin
{
    float4 position : Position;
};

[maxvertexcount(4)]
void main(
	point GSin input[1],
	inout TriangleStream< PSin > output
)
{
    PSin element;
    float4x4 viewProj = mul(view, proj);
    float SIZE = 0.05;
    float3 upVec = upVector;
    float3 particleRightVector = cross(forwardVector, upVector);

    float4 corners[4] =
    {
        input[0].position - float4(particleRightVector * SIZE + upVec * SIZE, 0.0f), //Bottom Left
        input[0].position + float4(particleRightVector * SIZE - upVec * SIZE, 0.0f), //Bottom Right
        input[0].position - float4(particleRightVector * SIZE - upVec * SIZE, 0.0f), //Top Left
        input[0].position + float4(particleRightVector * SIZE + upVec * SIZE, 0.0f)  //Top Right

    };
	
    for (int i = 0; i < 4; i++)
    {
        element.newPos = corners[i];
        element.position = (mul(corners[i], viewProj));
        output.Append(element);
    }
}