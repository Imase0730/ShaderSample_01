//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"


// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);
    ~Game() = default;

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    void OnDeviceLost() override;
    void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const noexcept;

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;
    //--------------------------------------------------------------------------------//

private:

    // �R�����X�e�[�g
    std::unique_ptr<DirectX::CommonStates> m_states;

    // ���_�V�F�[�_�[�̃f�[�^
    Microsoft::WRL::ComPtr<ID3DBlob> m_vsBlob;

    // �s�N�Z���V�F�[�_�[�̃f�[�^
    Microsoft::WRL::ComPtr<ID3DBlob> m_psBlob;

    // ���_�V�F�[�_�[
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;

    // �s�N�Z���V�F�[�_�[
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;

    // ���̓��C�A�E�g
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

    // ���_�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;

    // �C���f�b�N�X�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

    // �萔�o�b�t�@�̍\���̂̒�`
    struct ConstantBuffer
    {
        DirectX::SimpleMath::Matrix World;       // ���[���h�s��
        DirectX::SimpleMath::Matrix View;        // �r���[�s��
        DirectX::SimpleMath::Matrix Projection;  // �ˉe�s��
    };

    // �萔�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;

    // �e�N�X�`���n���h��
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;

};
