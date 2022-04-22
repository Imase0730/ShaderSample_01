#include "pch.h"
#include "Model.h"
#include <string>
#include <fstream>
#include <vector>

using namespace DirectX;
using namespace Microsoft::WRL;

Model::Model()
	: m_indexCount(0)
{
}

std::unique_ptr<Model> Model::CreateFromObj(ID3D11Device* device, const wchar_t* szFileName)
{
	char szBuffer[3][256];
	XMFLOAT3 f3;
	XMFLOAT2 f2;
	std::vector<XMFLOAT3> vertex;
	std::vector<XMFLOAT2> texCoord;
	std::vector<Index> index;
	std::string str;

	// モデル管理オブジェクトを作成
	auto model = std::make_unique<Model>();

	std::ifstream ifs(szFileName);

	// １行読み込み
	while (std::getline(ifs, str))
	{
		// マテリアルファイル名
		if (sscanf_s(str.data(), "mtllib %s", szBuffer[0], (unsigned int)sizeof(szBuffer[0])) == 1)
		{
			model->m_materials.push_back(GetMaterial(device, szBuffer[0]));
			continue;
		}

		// 頂点データ
		if (sscanf_s(str.data(), "v %f %f %f", &f3.x, &f3.y, &f3.z) == 3)
		{
			vertex.push_back(f3);
			continue;
		}

		// テクスチャ座標
		if (sscanf_s(str.data(), "vt %f %f", &f2.x, &f2.y) == 2)
		{
			// DirectXのUV座標系へ変換する（左下原点→左上原点）
			f2.y = 1.0f - f2.y;
			texCoord.push_back(f2);
			continue;
		}

		// 各頂点のインデックス番号（三角形のみが前提）
		if (sscanf_s(str.data(), "f %s %s %s", szBuffer[0], (unsigned int)sizeof(szBuffer[0]), szBuffer[1], (unsigned int)sizeof(szBuffer[1]), szBuffer[2], (unsigned int)sizeof(szBuffer[2])) == 3)
		{
			// 各頂点のインデックス番号を格納
			for (size_t i = 0; i < 3; i++)
			{
				Index idx;
				if (sscanf_s(szBuffer[i], "%hu/%hu", &idx.v_idx, &idx.t_idx) == 2)
				{
					// インデックスの番号を０スタートへ修正する
					idx.v_idx--;
					idx.t_idx--;
					index.push_back(idx);
				}
			}
		}
	}

	ifs.close();

	// モデルの頂点数
	model->m_indexCount = index.size();

	// 頂点バッファの作成
	{
		size_t size = index.size();

		VertexPositionTexture* p = new VertexPositionTexture[size];

		for (size_t i = 0; i < size; i++)
		{
			p[i].position = vertex[index[i].v_idx];
			p[i].textureCoordinate = texCoord[index[i].t_idx];
		}

		D3D11_BUFFER_DESC desc = {};

		desc.ByteWidth = (UINT)(sizeof(VertexPositionTexture) * size);
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA subData;
		subData.pSysMem = p;
		subData.SysMemPitch = 0;
		subData.SysMemSlicePitch = 0;

		DX::ThrowIfFailed(device->CreateBuffer(&desc, &subData, &model->m_vertexBuffer));

		delete[] p;
	}

	// インデックスバッファの作成
	{
		size_t size = index.size();
		uint16_t* p = new uint16_t[size];

		for (uint16_t i = 0; i < size; i++)
		{
			p[i] = i;
		}

		D3D11_BUFFER_DESC desc = {};

		desc.ByteWidth = (UINT)(sizeof(uint16_t) * index.size());
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA subData;
		subData.pSysMem = p;
		subData.SysMemPitch = 0;
		subData.SysMemSlicePitch = 0;

		DX::ThrowIfFailed(device->CreateBuffer(&desc, &subData, &model->m_indexBuffer));

		delete[] p;
	}

    return model;
}

void Model::Draw(ID3D11DeviceContext* context, DirectX::CommonStates& states)
{
	// テクスチャの設定
	ID3D11ShaderResourceView* textures[1] = { m_materials[0].get()->texture.Get() };
	context->PSSetShaderResources(0, 1, textures);

	// サンプラーの設定
	ID3D11SamplerState* samplerState = states.LinearWrap();
	context->PSSetSamplers(0, 1, &samplerState);

	// 頂点バッファを設定する
	{
		auto vertexBuffer = m_vertexBuffer.Get();
		UINT vertexStride = sizeof(VertexPositionTexture);
		UINT vertexOffset = 0;

		context->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexStride, &vertexOffset);
	}

	// インデックスバッファを設定する
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	// プリミティブの描画 
	context->DrawIndexed(m_indexCount, 0, 0);
}

std::unique_ptr<Model::Material> Model::GetMaterial(ID3D11Device* device, const char* szFileName)
{
	char szBuffer[MAX_PATH];
	std::string str;

	// モデル管理オブジェクトを作成
	auto material = std::make_unique<Model::Material>();

	std::ifstream ifs(szFileName);

	// １行読み込み
	while (std::getline(ifs, str))
	{
		// マテリアル名
		if (sscanf_s(str.data(), "newmtl %s", szBuffer, (unsigned int)sizeof(szBuffer)) == 1)
		{
			szBuffer[MAX_PATH - 1] = '\0';
			material->name = szBuffer;
			continue;
		}

		// テクスチャファイル名
		if (sscanf_s(str.data(), "map_Kd %s", szBuffer, (unsigned int)sizeof(szBuffer)) == 1)
		{
			szBuffer[MAX_PATH - 1] = '\0';
			str = szBuffer;
			size_t pos = str.rfind('\\');
			if (pos != std::string::npos) {
				// ファイル名を取り出す
				str = str.substr(pos + 1, str.size() - pos - 1);
				// ファイル名をワイド文字に変換する
				wchar_t buffer[256];
				MultiByteToWideChar(CP_OEMCP, 0, str.c_str(), -1, buffer, sizeof(buffer) / sizeof(wchar_t));
				// GPU側にテクスチャリソースを作成する
				CreateWICTextureFromFile(device, buffer, nullptr, material->texture.GetAddressOf());
			}
		}
	}

	ifs.close();

	return material;
}
