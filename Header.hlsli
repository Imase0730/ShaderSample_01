cbuffer Transform : register(b0)
{
	matrix World;		// ワールド行列
	matrix View;		// ビュー行列
	matrix Projection;	// 射影行列
}

struct VSInput
{
	float3 Position	: SV_Position;	// 位置
};

struct VSOutput
{
	float4 Position	: SV_POSITION;	// 位置
};

