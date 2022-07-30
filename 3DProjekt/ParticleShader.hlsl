RWBuffer<float> particlePositions : register(u0);

cbuffer particleAnimation : register(b0)
{
    float time;
    float pariclesPerThread;
    float2 padding;
};

#define ThreadsPerGroup 10

[numthreads(ThreadsPerGroup, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint startingParticle = DTid.x * pariclesPerThread;
    for (int i = 0; i < (int) pariclesPerThread; i++)
    {
        particlePositions[(startingParticle + i) * 3]       = cos(2 * 3.141592 * time + startingParticle+i);
        particlePositions[(startingParticle + i) * 3 + 1]   = sqrt(time + startingParticle + i);
        particlePositions[(startingParticle + i) * 3 + 2]   = sin(2 * 3.141592 * time + startingParticle + i);
    }
}