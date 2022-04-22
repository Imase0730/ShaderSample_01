cbuffer Transform : register(b0)
{
	matrix World;		// ���[���h�s��
	matrix View;		// �r���[�s��
	matrix Projection;	// �ˉe�s��
}

struct VSInput
{
	float3 Position	: SV_Position;	// �ʒu
	float2 TexCoord	: TEXCOORD;		// �e�N�X�`�����W
};

struct VSOutput
{
	float4 Position	: SV_POSITION;	// �ʒu
	float2 TexCoord	: TEXCOORD;		// �e�N�X�`�����W
};

// �e�N�X�`��
Texture2D ColorMap : register(t0);

// �T���v���[
SamplerState ColorSmp : register(s0);
