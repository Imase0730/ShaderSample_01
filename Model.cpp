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

	// ���f���Ǘ��I�u�W�F�N�g���쐬
	auto model = std::make_unique<Model>();

	std::ifstream ifs(szFileName);

	// �P�s�ǂݍ���
	while (std::getline(ifs, str))
	{
		// �}�e���A���t�@�C����
		if (sscanf_s(str.data(), "mtllib %s", szBuffer[0], (unsigned int)sizeof(szBuffer[0])) == 1)
		{
			model->m_materials.push_back(GetMaterial(device, szBuffer[0]));
			continue;
		}

		// ���_�f�[�^
		if (sscanf_s(str.data(), "v %f %f %f", &f3.x, &f3.y, &f3.z) == 3)
		{
			vertex.push_back(f3);
			continue;
		}

		// �e�N�X�`�����W
		if (sscanf_s(str.data(), "vt %f %f", &f2.x, &f2.y) == 2)
		{
			// DirectX��UV���W�n�֕ϊ�����i�������_�����㌴�_�j
			f2.y = 1.0f - f2.y;
			texCoord.push_back(f2);
			continue;
		}

		// �e���_�̃C���f�b�N�X�ԍ��i�O�p�`�݂̂��O��j
		if (sscanf_s(str.data(), "f %s %s %s", szBuffer[0], (unsigned int)sizeof(szBuffer[0]), szBuffer[1], (unsigned int)sizeof(szBuffer[1]), szBuffer[2], (unsigned int)sizeof(szBuffer[2])) == 3)
		{
			// �e���_�̃C���f�b�N�X�ԍ����i�[
			for (size_t i = 0; i < 3; i++)
			{
				Index idx;
				if (sscanf_s(szBuffer[i], "%hu/%hu", &idx.v_idx, &idx.t_idx) == 2)
				{
					// �C���f�b�N�X�̔ԍ����O�X�^�[�g�֏C������
					idx.v_idx--;
					idx.t_idx--;
					index.push_back(idx);
				}
			}
		}
	}

	ifs.close();

	// ���f���̒��_��
	model->m_indexCount = index.size();

	// ���_�o�b�t�@�̍쐬
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

	// �C���f�b�N�X�o�b�t�@�̍쐬
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
	// �e�N�X�`���̐ݒ�
	ID3D11ShaderResourceView* textures[1] = { m_materials[0].get()->texture.Get() };
	context->PSSetShaderResources(0, 1, textures);

	// �T���v���[�̐ݒ�
	ID3D11SamplerState* samplerState = states.LinearWrap();
	context->PSSetSamplers(0, 1, &samplerState);

	// ���_�o�b�t�@��ݒ肷��
	{
		auto vertexBuffer = m_vertexBuffer.Get();
		UINT vertexStride = sizeof(VertexPositionTexture);
		UINT vertexOffset = 0;

		context->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexStride, &vertexOffset);
	}

	// �C���f�b�N�X�o�b�t�@��ݒ肷��
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	// �v���~�e�B�u�̕`�� 
	context->DrawIndexed(m_indexCount, 0, 0);
}

std::unique_ptr<Model::Material> Model::GetMaterial(ID3D11Device* device, const char* szFileName)
{
	char szBuffer[MAX_PATH];
	std::string str;

	// ���f���Ǘ��I�u�W�F�N�g���쐬
	auto material = std::make_unique<Model::Material>();

	std::ifstream ifs(szFileName);

	// �P�s�ǂݍ���
	while (std::getline(ifs, str))
	{
		// �}�e���A����
		if (sscanf_s(str.data(), "newmtl %s", szBuffer, (unsigned int)sizeof(szBuffer)) == 1)
		{
			szBuffer[MAX_PATH - 1] = '\0';
			material->name = szBuffer;
			continue;
		}

		// �e�N�X�`���t�@�C����
		if (sscanf_s(str.data(), "map_Kd %s", szBuffer, (unsigned int)sizeof(szBuffer)) == 1)
		{
			szBuffer[MAX_PATH - 1] = '\0';
			str = szBuffer;
			size_t pos = str.rfind('\\');
			if (pos != std::string::npos) {
				// �t�@�C���������o��
				str = str.substr(pos + 1, str.size() - pos - 1);
				// �t�@�C���������C�h�����ɕϊ�����
				wchar_t buffer[256];
				MultiByteToWideChar(CP_OEMCP, 0, str.c_str(), -1, buffer, sizeof(buffer) / sizeof(wchar_t));
				// GPU���Ƀe�N�X�`�����\�[�X���쐬����
				CreateWICTextureFromFile(device, buffer, nullptr, material->texture.GetAddressOf());
			}
		}
	}

	ifs.close();

	return material;
}
