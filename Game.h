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

    // コモンステート
    std::unique_ptr<DirectX::CommonStates> m_states;

    // 頂点シェーダーのデータ
    Microsoft::WRL::ComPtr<ID3DBlob> m_vsBlob;

    // ピクセルシェーダーのデータ
    Microsoft::WRL::ComPtr<ID3DBlob> m_psBlob;

    // 頂点シェーダー
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;

    // ピクセルシェーダー
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;

    // 入力レイアウト
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

    // 頂点バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;

    // インデックスバッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

    // 定数バッファの構造体の定義
    struct ConstantBuffer
    {
        DirectX::SimpleMath::Matrix World;       // ワールド行列
        DirectX::SimpleMath::Matrix View;        // ビュー行列
        DirectX::SimpleMath::Matrix Projection;  // 射影行列
    };

    // 定数バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;

};
