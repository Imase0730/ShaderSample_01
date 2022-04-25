#pragma once

#include <vector>

class Model
{

private:

	// �C���f�b�N�X���
	struct Index
	{
		uint16_t v_idx;	// ���_
		uint16_t t_idx;	// �e�N�X�`�����W
	};

public:

	// �}�e���A�����
	struct Material
	{
		// �}�e���A����
		std::string name;

		// �e�N�X�`���n���h��
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
	};

public:

	// ���_��
	UINT m_indexCount = 0;

	// ���_�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;

	// �C���f�b�N�X�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

	// �}�e���A��
	std::vector<std::unique_ptr<Material>> m_materials;

public:

	// �R���X�g���N�^
	Model();

	// obj�`���̃t�@�C�����烂�f���Ǘ��I�u�W�F�N�g���쐬����֐�
	static std::unique_ptr<Model> CreateFromObj(ID3D11Device* device, const wchar_t* szFileName);
	
	// �`��֐�
	void Draw(ID3D11DeviceContext* contextconst, DirectX::CommonStates& states);

	// �}�e���A���t�@�C������}�e���A�������擾����֐�
	static std::unique_ptr<Model::Material> GetMaterial(ID3D11Device* device, const char* szFileName);

};

