#include "Header.hlsli"

float4 main(VSOutput input) : SV_TARGET
{
	return ColorMap.Sample(ColorSmp, input.TexCoord) * input.Color;
}
