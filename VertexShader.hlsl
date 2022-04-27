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

	// ローカル座標系の法線ベクトルをワールド空間へ
	float3 normal = mul(World, float4(input.Normal, 1.0f)).xyz;
	// ベクトルを正規化
	normal = normalize(normal);
	// 光の方向から色の強度を求める
	float col = saturate(dot(normal, LightDir));
	output.Color = float4(col, col, col, 1.0f);

	return output;
}
