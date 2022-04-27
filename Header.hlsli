cbuffer Transform : register(b0)
{
	matrix World;		// ワールド行列
	matrix View;		// ビュー行列
	matrix Projection;	// 射影行列
	float3 LightDir;	// 光の方向ベクトル
}

struct VSInput
{
	float3 Position	: SV_Position;	// 位置
	float3 Normal	: NORMAL;		// 法線
	float2 TexCoord	: TEXCOORD;		// テクスチャ座標
};

struct VSOutput
{
	float4 Position	: SV_POSITION;	// 位置
	float4 Color	: COLOR;		// 頂点カラー
	float2 TexCoord	: TEXCOORD;		// テクスチャ座標
};

// テクスチャ
Texture2D ColorMap : register(t0);

// サンプラー
SamplerState ColorSmp : register(s0);
