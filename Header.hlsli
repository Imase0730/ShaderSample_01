cbuffer Transform : register(b0)
{
	matrix World;		// ワールド行列
	matrix View;		// ビュー行列
	matrix Projection;	// 射影行列
}

struct VSInput
{
	float3 Position	: SV_Position;	// 位置
	float2 TexCoord	: TEXCOORD;		// テクスチャ座標
};

struct VSOutput
{
	float4 Position	: SV_POSITION;	// 位置
	float2 TexCoord	: TEXCOORD;		// テクスチャ座標
};

// テクスチャ
Texture2D ColorMap : register(t0);

// サンプラー
SamplerState ColorSmp : register(s0);
