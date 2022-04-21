#pragma once

#include <vector>

class Model
{
public:

	struct Material
	{
		// マテリアル名
		std::string name;

		// テクスチャハンドル
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
	};

public:

	// 頂点数
	UINT m_indexCount = 0;

	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;

	// インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

	// マテリアル
	std::vector<std::unique_ptr<Material>> m_materials;

public:

	// コンストラクタ
	Model();

	// obj形式のファイルからモデル管理オブジェクトを作成する関数
	static std::unique_ptr<Model> CreateFromObj(ID3D11Device* device, const wchar_t* szFileName);
	
	// 描画関数
	void Draw(ID3D11DeviceContext* context);

	// マテリアルファイルからマテリアル情報を取得する関数
	static std::unique_ptr<Model::Material> GetMaterial(ID3D11Device* device, const char* szFileName);

};

