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

	// ���f���Ǘ��I�u�W�F�N�g���쐬
	auto model = std::make_unique<Model>();

	std::ifstream ifs(szFileName);

	// �P�s�ǂݍ���
	while (std::getline(ifs, str))
	{
		// ���_�f�[�^
		if (sscanf_s(str.data(), "v  %f %f %f", &f3.x, &f3.y, &f3.z) > 0)
		{
			vertex.push_back(f3);
			continue;
		}

		// �e���_�̃C���f�b�N�X�ԍ��i�O�p�`�݂̂��O��j
		if (sscanf_s(str.data(), "f %s %s %s", szBuffer[0], (unsigned int)sizeof(szBuffer[0]), szBuffer[1], (unsigned int)sizeof(szBuffer[1]), szBuffer[2], (unsigned int)sizeof(szBuffer[2])) == 3)
		{
			// �e���_�̃C���f�b�N�X�ԍ����i�[
			for (size_t i = 0; i < 3; i++)
			{
				uint16_t v_idx;
				if (sscanf_s(szBuffer[i], "%hu", &v_idx) == 1)
				{
					// �C���f�b�N�X�̔ԍ����O�X�^�[�g�֏C������
					v_idx--;
					index.push_back(v_idx);
				}
			}
		}
	}

	ifs.close();

	// ���f���̒��_��
	model->m_indexCount = index.size();

	// ���_�o�b�t�@�̍쐬
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

	// �C���f�b�N�X�o�b�t�@�̍쐬
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
	// ���_�o�b�t�@��ݒ肷��
	{
		auto vertexBuffer = m_vertexBuffer.Get();
		UINT vertexStride = sizeof(VertexPosition);
		UINT vertexOffset = 0;

		context->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexStride, &vertexOffset);
	}

	// �C���f�b�N�X�o�b�t�@��ݒ肷��
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	// �v���~�e�B�u�̕`�� 
	context->DrawIndexed(m_indexCount, 0, 0);
}
