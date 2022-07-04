RWBuffer<float> particlePositions : register(u0);

[numthreads(16, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    particlePositions[DTid.x * 3] = cos(1.f / 144.f * 0.01f + DTid.x);
    particlePositions[DTid.x * 3 + 2] = sin(1.f / 144.f * 0.01f + DTid.x);
}