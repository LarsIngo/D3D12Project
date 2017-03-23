// Input.
struct PSInput
{
    float4 position : SV_POSITION;
    float3 worldPosition : WORLDPOSITION;
    float3 color : COLOR;
    float2 uv : UV;
};

// Output.
struct PSOutput
{
    float4 color : SV_TARGET0;
};

PSOutput main(PSInput input) : SV_TARGET
{
    PSOutput output;

    float x = input.uv.x - 0.5f;
    float y = input.uv.y - 0.5f;
    float r = sqrt(x * x + y * y);
    float factor = max(1.f - r * 2.f, 0.f); //[1,0]
    float sinFactor = 1.f - sin(3.14159265f / 2.f * (factor + 1.f));

    output.color = float4(input.color, sinFactor);

    return output;
}
