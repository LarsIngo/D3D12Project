// Particle.
struct Particle
{
    float4 position : SV_POSITION;
    float4 velocity : VELOCITY;
    float4 color : COLOR;
    float4 scale : SCALE;
};
StructuredBuffer<Particle> g_Input : register(t0);

Particle main(uint vID : SV_VertexID)
{
    return g_Input[vID];
}
