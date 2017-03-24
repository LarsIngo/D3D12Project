float4 main(uint id : SV_VertexID) : SV_POSITION
{
    return float4((id == 0 || id == 1) * 2 - 1, (id == 0 || id == 2) * 2 - 1, 0, 1);
}
