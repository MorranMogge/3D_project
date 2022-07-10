RWBuffer<float> particlePositions : register(u0);

cbuffer particleAnimation : register(b0)
{
    float time;
    float3 padding;
};

[numthreads(20, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    particlePositions[DTid.x * 3] = cos(2 * 3.141592 * time + DTid.x);
    particlePositions[DTid.x * 3 + 1] = sqrt(time + DTid.x);
    particlePositions[DTid.x * 3 + 2] = sin(2 * 3.141592 * time + DTid.x);
    //particlePositions[DTid.x * 3 + 2] = log(2 * 3.141592 * time + DTid.x);

}