cbuffer Transform : register(b0)
{
	matrix World;		// ワールド行列
	matrix View;		// ビュー行列
	matrix Projection;	// 射影行列
}

struct VSInput
{
	float3 Position : SV_Position;	// 位置
	float4 Color	: COLOR;		// 色
};

struct VSOutput
{
	float4 Position : SV_POSITION;	// 位置
	float4 Color	: COLOR;		// 色
};