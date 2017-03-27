// Particle.
struct Particle
{
    float4 position;
    float4 velocity;
    float4 color;
    float4 scale;
};
// Input particles.
StructuredBuffer<Particle> g_InputParticles : register(t0);

// Output particles.
RWStructuredBuffer<Particle> g_OutputParticles : register(u0);

// Meta data.
struct MetaData
{
    float dt;
    uint particleCount;
    float pad[6];
};
// Meta buffer.
StructuredBuffer<MetaData> g_MetaBuffer : register(t1);

[numthreads(256, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    MetaData metaData = g_MetaBuffer[0];
    float dt = metaData.dt;
    uint particleCount = metaData.particleCount;
    uint tID = threadID.x;

    if (tID < particleCount)
    {
        Particle self = g_InputParticles[tID];
        self.position.xyz = self.position.xyz + self.velocity.xyz * dt;
        
        uint intersectCount = 0;
        for (uint pID = 0; pID < particleCount; ++pID)
        {
            if (tID != pID)
            {
                Particle other = g_InputParticles[pID];
                if (length(other.position - self.position) < 1.f)
                {
                    ++intersectCount;
                }                
            }
        }
        self.color = float4(intersectCount / 10.f, 1.f, 0.f, 0.f);
        
        g_OutputParticles[tID] = self;
    }
}
