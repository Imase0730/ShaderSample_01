#include "Header.hlsli"

float4 main(float3 pos : SV_Position) : SV_POSITION
{
	float4 localPos = float4(pos, 1.0f);
	float4 worldPos = mul(World, localPos);
	float4 viewPos = mul(View, worldPos);
	float4 projPos = mul(Projection, viewPos);

	return projPos;
}
