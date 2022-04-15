#include "Header.hlsli"

float4 main(VSOutput input) : SV_TARGET
{
	float4 color;
	int width, height;

	ColorMap.GetDimensions(width, height);
	float3 texCoord3 = float3(input.TexCoord.x * width, input.TexCoord.y * height, 0);
	color = ColorMap.Load(texCoord3);

	return color;
}
