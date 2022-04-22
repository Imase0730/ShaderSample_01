#include "Header.hlsli"

VSOutput main(VSInput input)
{
	VSOutput output;

	float4 localPos = float4(input.Position, 1.0f);
	float4 worldPos = mul(World, localPos);
	float4 viewPos = mul(View, worldPos);
	float4 projPos = mul(Projection, viewPos);

	output.Position = projPos;
	output.TexCoord = input.TexCoord;

	return output;
}
