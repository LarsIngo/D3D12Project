// Particle.
struct Particle
{
    float4 position : SV_POSITION;
    float4 velocity : VELOCITY;
    float4 color : COLOR;
    float4 scale : SCALE;
};
//StructuredBuffer<Particle> g_Input : register(t2);

Particle main(uint vID : SV_VertexID)
{
    Particle p;
    p.position = float4(0.f, 0.f, 0.f, 0.f);
    p.velocity = float4(0.f, 0.f, 0.f, 0.f);
    p.color = float4(1.f, 0.f, 0.f, 0.f);//g_Input[vID].color;
    p.scale = float4(1.f, 1.f, 0.f, 0.f);
    return p;
}
