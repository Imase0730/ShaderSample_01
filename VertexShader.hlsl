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

	// ���[�J�����W�n�̖@���x�N�g�������[���h��Ԃ�
	float3 normal = mul(World, float4(input.Normal, 1.0f)).xyz;
	// �x�N�g���𐳋K��
	normal = normalize(normal);
	// ���̕�������F�̋��x�����߂�
	float col = saturate(dot(normal, LightDir));
	output.Color = float4(col, col, col, 1.0f);

	return output;
}
