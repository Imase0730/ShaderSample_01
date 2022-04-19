//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */

    //-----------------------------------------------------------------------------//

    // �f�o�b�O�J�������쐬
    m_debugCamera = std::make_unique<DebugCamera>(width, height);

    // �f�o�C�X���擾����
    auto device = m_deviceResources->GetD3DDevice();

    // �f�o�C�X�R���e�L�X�g���擾����
    auto deviceContext = m_deviceResources->GetD3DDeviceContext();

    // �R�����X�e�[�g�̍쐬
    m_states = std::make_unique<CommonStates>(device);

    // ���_�V�F�[�_�[�̓ǂݍ���
    DX::ThrowIfFailed(D3DReadFileToBlob(L"VertexShader.cso", m_vsBlob.GetAddressOf()));

    // �s�N�Z���V�F�[�_�[�̓ǂݍ���
    DX::ThrowIfFailed(D3DReadFileToBlob(L"PixelShader.cso", m_psBlob.GetAddressOf()));

    // ���_�V�F�[�_�[�̍쐬
    device->CreateVertexShader(m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf());

    // �s�N�Z���V�F�[�_�[�̍쐬
    device->CreatePixelShader(m_psBlob->GetBufferPointer(), m_psBlob->GetBufferSize(), nullptr, m_pixelShader.GetAddressOf());

    // ���̓��C�A�E�g�̍쐬
    DX::ThrowIfFailed(
        device->CreateInputLayout(
            VertexPositionColorTexture::InputElements, VertexPositionColorTexture::InputElementCount,
            m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize(),
            m_inputLayout.GetAddressOf()
        )
    );

    // ���_�o�b�t�@�̍쐬
    {
        D3D11_BUFFER_DESC desc = {};

        desc.ByteWidth = sizeof(VertexPositionColorTexture) * 4;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        DX::ThrowIfFailed(device->CreateBuffer(&desc, nullptr, &m_vertexBuffer));
    }

    //--------------------------//
    // ���_�f�[�^��ݒ�         //
    //--------------------------//
    D3D11_MAPPED_SUBRESOURCE mappedVertices;
    // ���_����ݒ�
    DX::ThrowIfFailed(
        deviceContext->Map(m_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertices)
    );

    // ���_�f�[�^
    VertexPositionColorTexture vertices[4] =
    {
        { Vector3(-1.0f,  1.0f, 0.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector2(0.0f, 0.0f) },
        { Vector3( 1.0f,  1.0f, 0.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f) },
        { Vector3( 1.0f, -1.0f, 0.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f) },
        { Vector3(-1.0f, -1.0f, 0.0f), Vector4(1.0f, 0.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f) },
    };

    memcpy(mappedVertices.pData, vertices, sizeof(VertexPositionColorTexture) * 4);
    deviceContext->Unmap(m_vertexBuffer.Get(), 0);
    //----------------------------//
    // ���_�f�[�^��ݒ�i�I�j     //
    //----------------------------//

    // �C���f�b�N�X�o�b�t�@�̍쐬
    {
        D3D11_BUFFER_DESC desc = {};

        desc.ByteWidth = sizeof(uint16_t) * 6;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        DX::ThrowIfFailed(device->CreateBuffer(&desc, nullptr, &m_indexBuffer));
    }

    //----------------------------------//
    // �C���f�b�N�X�f�[�^��ݒ�         //
    //----------------------------------//
    D3D11_MAPPED_SUBRESOURCE mappedIndices;
    // �C���f�b�N�X��ݒ�
    DX::ThrowIfFailed(
        deviceContext->Map(m_indexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedIndices)
    );

    uint16_t indices[] =
    {
        0, 1, 2,
        0, 2, 3,
    };

    auto outputIndices = static_cast<uint16_t*>(mappedIndices.pData);

    for (size_t i = 0; i < 6; i++)
    {
        outputIndices[i] = static_cast<uint16_t>(indices[i]);
    }
    deviceContext->Unmap(m_indexBuffer.Get(), 0);
    //------------------------------------//
    // �C���f�b�N�X�f�[�^��ݒ�i�I�j     //
    //------------------------------------//

    // �萔�o�b�t�@�̍쐬
    {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(ConstantBuffer);
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        DX::ThrowIfFailed(device->CreateBuffer(&desc, nullptr, m_constantBuffer.GetAddressOf()));
    }

    // �e�N�X�`���̃��[�h
    DX::ThrowIfFailed(
        CreateDDSTextureFromFile(device, L"mario.dds", nullptr, m_texture.GetAddressOf())
    );

}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.
    elapsedTime;

    // �f�o�b�O�p�J�����̍X�V
    m_debugCamera->Update();
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");

    // �f�o�C�X�R���e�L�X�g���擾����
    auto deviceContext = m_deviceResources->GetD3DDeviceContext();

    // TODO: Add your rendering code here.

    // ���[���h�s��
    Matrix world = Matrix::Identity;

    // �J�����̐ݒ�
    Matrix view = m_debugCamera->GetCameraMatrix();

    // �ˉe�s��̐ݒ�
    int width, height;
    GetDefaultSize(width, height);
    Matrix projection = Matrix::CreatePerspectiveFieldOfView(
        XMConvertToRadians(45.0f),  // ��p
        width / (float)height,      // �A�X�y�N�g��
        0.1f,                       // Near�N���b�v�ʂ̈ʒu
        100.0f                      // Far�N���b�v�ʂ̈ʒu
    );

    //----------------------------------//
    // �萔�o�b�t�@��ݒ�               //
    //----------------------------------//
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        DX::ThrowIfFailed(
            deviceContext->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)
        );

        ConstantBuffer buffer = { world, view, projection };

        *static_cast<ConstantBuffer*>(mappedResource.pData) = buffer;
        deviceContext->Unmap(m_constantBuffer.Get(), 0);
    }
    //----------------------------------//
    // �萔�o�b�t�@��ݒ�i�I�j         //
    //----------------------------------//

    //---------------------------------------------------------------------------------//
    // Input Assembler (IA)
    //---------------------------------------------------------------------------------//
    // ���̓��C�A�E�g�̐ݒ�
    deviceContext->IASetInputLayout(m_inputLayout.Get());

    // �v���~�e�B�u�̎�ނ�ݒ肷��
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // ���_�o�b�t�@��ݒ肷��
    {
        auto vertexBuffer = m_vertexBuffer.Get();
        UINT vertexStride = sizeof(VertexPositionColorTexture);
        UINT vertexOffset = 0;

        deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexStride, &vertexOffset);
    }

    // �C���f�b�N�X�o�b�t�@��ݒ肷��
    deviceContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

    //---------------------------------------------------------------------------------//
    // Vertex Shader (VS)
    //---------------------------------------------------------------------------------//
    // ���_�V�F�[�_�[��ݒ肷��
    deviceContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);

    // ���_�V�F�[�_�[�֓n���萔�o�b�t�@��ݒ肷��
    deviceContext->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

    //---------------------------------------------------------------------------------//
    // Rasterizer (RS)
    //---------------------------------------------------------------------------------//
    // �J�����O���[�h�̐ݒ�
    deviceContext->RSSetState(m_states->CullNone());

    //---------------------------------------------------------------------------------//
    // Pixel Shader (PS)
    //---------------------------------------------------------------------------------//
    // �s�N�Z���V�F�[�_�[��ݒ肷��
    deviceContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);

    // �e�N�X�`���̐ݒ�
    ID3D11ShaderResourceView* textures[1] = { m_texture.Get() };
    deviceContext->PSSetShaderResources(0, 1, textures);

    // �T���v���[�̐ݒ�
    ID3D11SamplerState* samplerState = m_states->LinearWrap();
    deviceContext->PSSetSamplers(0, 1, &samplerState);

    //---------------------------------------------------------------------------------//
    // Output Merger (OM)
    //---------------------------------------------------------------------------------//
    // �u�����h�X�e�[�g�̐ݒ�
    deviceContext->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);

    // �[�x�o�b�t�@�̐ݒ�
    deviceContext->OMSetDepthStencilState(m_states->DepthNone(), 0);

    // �v���~�e�B�u�̕`�� 
    deviceContext->DrawIndexed(6, 0, 0);

    m_deviceResources->PIXEndEvent();

    // Show the new frame.
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
    auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 800;
    height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();

    // TODO: Initialize device dependent objects here (independent of window size).
    device;
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
