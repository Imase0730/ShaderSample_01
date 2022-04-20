#pragma once

class Model
{
public:

	// ���_��
	UINT m_indexCount = 0;

	// ���_�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;

	// �C���f�b�N�X�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

public:

	// �R���X�g���N�^
	Model();

	// obj�`���̃t�@�C�����烂�f���Ǘ��I�u�W�F�N�g���쐬����֐�
	static std::unique_ptr<Model> CreateFromObj(ID3D11Device* device, const wchar_t* szFileName);
	
	// �`��֐�
	void Draw(ID3D11DeviceContext* context);
};

