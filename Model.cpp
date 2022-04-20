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
	char szBuffer[3][32];
	XMFLOAT3 f3;
	std::vector<XMFLOAT3> vertex;
	std::vector<uint16_t> index;
	std::string str;

	// モデル管理オブジェクトを作成
	auto model = std::make_unique<Model>();

	std::ifstream ifs(szFileName);

	// １行読み込み
	while (std::getline(ifs, str))
	{
		// 頂点データ
		if (sscanf_s(str.data(), "v  %f %f %f", &f3.x, &f3.y, &f3.z) > 0)
		{
			vertex.push_back(f3);
			continue;
		}

		// 各頂点のインデックス番号（三角形のみが前提）
		if (sscanf_s(str.data(), "f %s %s %s", szBuffer[0], (unsigned int)sizeof(szBuffer[0]), szBuffer[1], (unsigned int)sizeof(szBuffer[1]), szBuffer[2], (unsigned int)sizeof(szBuffer[2])) == 3)
		{
			// 各頂点のインデックス番号を格納
			for (size_t i = 0; i < 3; i++)
			{
				uint16_t v_idx;
				if (sscanf_s(szBuffer[i], "%hu", &v_idx) == 1)
				{
					// インデックスの番号を０スタートへ修正する
					v_idx--;
					index.push_back(v_idx);
				}
			}
		}
	}

	ifs.close();

	// モデルの頂点数
	model->m_indexCount = index.size();

	// 頂点バッファの作成
	{
		size_t size = vertex.size();

		VertexPosition* p = new VertexPosition[size];

		for (size_t i = 0; i < size; i++)
		{
			p[i].position = vertex[i];
		}

		D3D11_BUFFER_DESC desc = {};

		desc.ByteWidth = (UINT)(sizeof(VertexPosition) * size);
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

		for (size_t i = 0; i < size; i++)
		{
			p[i] = index[i];
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

void Model::Draw(ID3D11DeviceContext* context)
{
	// 頂点バッファを設定する
	{
		auto vertexBuffer = m_vertexBuffer.Get();
		UINT vertexStride = sizeof(VertexPosition);
		UINT vertexOffset = 0;

		context->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexStride, &vertexOffset);
	}

	// インデックスバッファを設定する
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	// プリミティブの描画 
	context->DrawIndexed(m_indexCount, 0, 0);
}
