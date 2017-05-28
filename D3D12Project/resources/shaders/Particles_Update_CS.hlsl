#define ITER 2000000.f

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

[numthreads(1, 1, 1)]
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
        
        self.color = float4(0.0, 0.0, 0.0, 0.0);
        for (int i = 0; i < ITER; ++i)
        {
            float sinFactorX = (sin(self.position.x * dt) + 1.f) / 2.f;
            float sinFactorY = (sin(self.position.y * dt) + 1.f) / 2.f;
            float sinFactorZ = (sin(self.position.z * dt) + 1.f) / 2.f;

            self.color += float4(sinFactorX, sinFactorY, sinFactorZ, 1.f) / ITER;
        }
        g_OutputParticles[tID] = self;
    }
}
