cbuffer Transform : register(b0)
{
	matrix World;		// ���[���h�s��
	matrix View;		// �r���[�s��
	matrix Projection;	// �ˉe�s��
}

struct VSInput
{
	float3 Position : SV_Position;	// �ʒu
	float4 Color	: COLOR;		// �F
};

struct VSOutput
{
	float4 Position : SV_POSITION;	// �ʒu
	float4 Color	: COLOR;		// �F
};