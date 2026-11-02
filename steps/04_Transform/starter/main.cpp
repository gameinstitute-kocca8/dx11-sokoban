// 04_Transform - World/View/Projection 행렬, Constant Buffer, SimpleMath
//
// 확인 화면: 면마다 색이 다른 정육면체가 회전하고, 그 아래에 같은 메시를 납작하게 늘린 회색 바닥이 있다.
// 하나의 메시를 World 행렬과 Tint만 바꿔 두 번 그린다. 07단계의 '그리드 맵 = 큐브 여러 번 그리기'의 출발점이다.
#include "StepApp.h"

#include <d3d11.h>
#include <wrl/client.h>

#include <cstdint>

#include "SimpleMath.h"  // DirectXTK

using Microsoft::WRL::ComPtr;
using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector4;

namespace {

struct Vertex {
    float position[3];
    float color[4];
};

// 정육면체: 면마다 색이 달라야 하므로 면마다 정점 4개씩, 총 24개.
// 모든 면은 '바깥에서 볼 때 시계 방향'(왼쪽 위 → 오른쪽 위 → 오른쪽 아래 → 왼쪽 아래)으로 놓여 있다.
const Vertex kCubeVertices[] = {
    // +X
    {{0.5f, 0.5f, 0.5f}, {0.95f, 0.35f, 0.30f, 1.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.95f, 0.35f, 0.30f, 1.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.95f, 0.35f, 0.30f, 1.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.95f, 0.35f, 0.30f, 1.0f}},
    // -X
    {{-0.5f, 0.5f, -0.5f}, {0.30f, 0.80f, 0.85f, 1.0f}},
    {{-0.5f, 0.5f, 0.5f}, {0.30f, 0.80f, 0.85f, 1.0f}},
    {{-0.5f, -0.5f, 0.5f}, {0.30f, 0.80f, 0.85f, 1.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.30f, 0.80f, 0.85f, 1.0f}},
    // +Y
    {{-0.5f, 0.5f, -0.5f}, {0.40f, 0.80f, 0.35f, 1.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.40f, 0.80f, 0.35f, 1.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.40f, 0.80f, 0.35f, 1.0f}},
    {{-0.5f, 0.5f, 0.5f}, {0.40f, 0.80f, 0.35f, 1.0f}},
    // -Y
    {{-0.5f, -0.5f, 0.5f}, {0.75f, 0.40f, 0.85f, 1.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.75f, 0.40f, 0.85f, 1.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.75f, 0.40f, 0.85f, 1.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.75f, 0.40f, 0.85f, 1.0f}},
    // +Z
    {{-0.5f, 0.5f, 0.5f}, {0.30f, 0.45f, 0.95f, 1.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.30f, 0.45f, 0.95f, 1.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.30f, 0.45f, 0.95f, 1.0f}},
    {{-0.5f, -0.5f, 0.5f}, {0.30f, 0.45f, 0.95f, 1.0f}},
    // -Z
    {{0.5f, 0.5f, -0.5f}, {0.95f, 0.80f, 0.30f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {0.95f, 0.80f, 0.30f, 1.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.95f, 0.80f, 0.30f, 1.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.95f, 0.80f, 0.30f, 1.0f}},
};

const std::uint16_t kCubeIndices[] = {
    0,  1,  2,  0,  2,  3,   // +X
    4,  5,  6,  4,  6,  7,   // -X
    8,  9,  10, 8,  10, 11,  // +Y
    12, 13, 14, 12, 14, 15,  // -Y
    16, 17, 18, 16, 18, 19,  // +Z
    20, 21, 22, 20, 22, 23,  // -Z
};

// TODO(04): HLSL의 cbuffer PerObject와 같은 배치의 구조체를 만든다.
//   힌트: Constant Buffer 크기는 16바이트의 배수여야 한다. Matrix(64) + Vector4(16) = 80

}  // namespace

class TransformApp final : public step::StepApp {
public:
    TransformApp() : StepApp(L"04 Transform & Constant Buffer") {}

private:
    ComPtr<ID3D11Device> device_;
    ComPtr<ID3D11DeviceContext> context_;
    ComPtr<IDXGISwapChain> swapChain_;
    ComPtr<ID3D11Texture2D> backBuffer_;
    ComPtr<ID3D11RenderTargetView> renderTargetView_;

    ComPtr<ID3D11Buffer> vertexBuffer_;
    ComPtr<ID3D11Buffer> indexBuffer_;
    ComPtr<ID3D11VertexShader> vertexShader_;
    ComPtr<ID3D11PixelShader> pixelShader_;
    ComPtr<ID3D11InputLayout> inputLayout_;
    ComPtr<ID3D11Texture2D> depthTexture_;
    ComPtr<ID3D11DepthStencilView> depthStencilView_;

    ComPtr<ID3D11Buffer> constantBuffer_;
    Matrix cubeWorld_;
    Matrix floorWorld_;
    Matrix view_;
    Matrix projection_;

    bool OnInit() override {
        // ---- 01: Device, SwapChain, Render Target View ----
        DXGI_SWAP_CHAIN_DESC desc = {};
        desc.BufferCount = 1;
        desc.BufferDesc.Width = Width();
        desc.BufferDesc.Height = Height();
        desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.OutputWindow = Window();
        desc.SampleDesc.Count = 1;
        desc.Windowed = TRUE;
        desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        UINT flags = 0;
#ifdef _DEBUG
        flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
        const D3D_DRIVER_TYPE driver = UseWarp() ? D3D_DRIVER_TYPE_WARP : D3D_DRIVER_TYPE_HARDWARE;
        HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, driver, nullptr, flags, nullptr, 0, D3D11_SDK_VERSION,
                                                   &desc, &swapChain_, &device_, nullptr, &context_);
        if (FAILED(hr) && (flags & D3D11_CREATE_DEVICE_DEBUG)) {
            step::Log("디버그 레이어 없이 다시 시도합니다. 경고 메시지를 보려면 '그래픽 도구'를 설치하세요.");
            flags &= ~D3D11_CREATE_DEVICE_DEBUG;
            hr = D3D11CreateDeviceAndSwapChain(nullptr, driver, nullptr, flags, nullptr, 0, D3D11_SDK_VERSION, &desc,
                                               &swapChain_, &device_, nullptr, &context_);
        }
        if (!step::Check(hr, "D3D11CreateDeviceAndSwapChain")) return false;
        if (!step::Check(swapChain_->GetBuffer(0, IID_PPV_ARGS(&backBuffer_)), "GetBuffer")) return false;
        if (!step::Check(device_->CreateRenderTargetView(backBuffer_.Get(), nullptr, &renderTargetView_),
                         "CreateRenderTargetView")) {
            return false;
        }

        // ---- 02, 03: 정점/인덱스 버퍼 (정육면체) ----
        D3D11_BUFFER_DESC vbDesc = {};
        vbDesc.ByteWidth = sizeof(kCubeVertices);
        vbDesc.Usage = D3D11_USAGE_DEFAULT;
        vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        D3D11_SUBRESOURCE_DATA vbData = {};
        vbData.pSysMem = kCubeVertices;
        if (!step::Check(device_->CreateBuffer(&vbDesc, &vbData, &vertexBuffer_), "CreateBuffer(vertex)")) {
            return false;
        }
        D3D11_BUFFER_DESC ibDesc = {};
        ibDesc.ByteWidth = sizeof(kCubeIndices);
        ibDesc.Usage = D3D11_USAGE_DEFAULT;
        ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        D3D11_SUBRESOURCE_DATA ibData = {};
        ibData.pSysMem = kCubeIndices;
        if (!step::Check(device_->CreateBuffer(&ibDesc, &ibData, &indexBuffer_), "CreateBuffer(index)")) {
            return false;
        }

        // ---- 03: 깊이 버퍼 ----
        D3D11_TEXTURE2D_DESC depthDesc = {};
        depthDesc.Width = Width();
        depthDesc.Height = Height();
        depthDesc.MipLevels = 1;
        depthDesc.ArraySize = 1;
        depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthDesc.SampleDesc.Count = 1;
        depthDesc.Usage = D3D11_USAGE_DEFAULT;
        depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        if (!step::Check(device_->CreateTexture2D(&depthDesc, nullptr, &depthTexture_), "CreateTexture2D(depth)")) {
            return false;
        }
        if (!step::Check(device_->CreateDepthStencilView(depthTexture_.Get(), nullptr, &depthStencilView_),
                         "CreateDepthStencilView")) {
            return false;
        }

        // ---- 04: Constant Buffer ----
        // TODO(04): PerObjectConstants 크기의 Constant Buffer를 만든다. 초기 데이터는 없다(nullptr).
        //   힌트: BindFlags D3D11_BIND_CONSTANT_BUFFER, Usage DEFAULT → 매 그리기마다 UpdateSubresource로 채운다.

        // ---- 02: 셰이더와 Input Layout ----
        ComPtr<ID3DBlob> vsBlob;
        ComPtr<ID3DBlob> psBlob;
        if (!step::CompileShader(L"shaders/Transform.hlsl", "VSMain", "vs_5_0", &vsBlob)) return false;
        if (!step::CompileShader(L"shaders/Transform.hlsl", "PSMain", "ps_5_0", &psBlob)) return false;
        if (!step::Check(device_->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr,
                                                     &vertexShader_),
                         "CreateVertexShader")) {
            return false;
        }
        if (!step::Check(device_->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr,
                                                    &pixelShader_),
                         "CreatePixelShader")) {
            return false;
        }
        const D3D11_INPUT_ELEMENT_DESC layout[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        };
        if (!step::Check(device_->CreateInputLayout(layout, ARRAYSIZE(layout), vsBlob->GetBufferPointer(),
                                                    vsBlob->GetBufferSize(), &inputLayout_),
                         "CreateInputLayout")) {
            return false;
        }
        return true;
    }

    void OnUpdate(float deltaSeconds, float totalSeconds) override {
        (void)deltaSeconds;
        (void)totalSeconds;
        // TODO(04): 세 가지 행렬을 만든다.
        //   힌트: 큐브 World: Y축으로 totalSeconds 라디안, X축으로 그 절반만큼 회전
        //   힌트: 바닥 World: 크기 (6, 0.2, 6)로 늘리고 y = -1.2로 이동. 곱셈 순서 = 크기 * 회전 * 이동
        //   힌트: View: 눈 (0, 2.5, -5)에서 원점을 바라봄, 위쪽 +Y. Projection: 시야각 60도, 화면비 Width/Height, 0.1 ~ 100
    }

    void OnRender() override {
        const float clearColor[4] = {0.10f, 0.20f, 0.35f, 1.0f};
        context_->ClearRenderTargetView(renderTargetView_.Get(), clearColor);
        context_->ClearDepthStencilView(depthStencilView_.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
        context_->OMSetRenderTargets(1, renderTargetView_.GetAddressOf(), depthStencilView_.Get());

        D3D11_VIEWPORT viewport = {};
        viewport.Width = static_cast<float>(Width());
        viewport.Height = static_cast<float>(Height());
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        context_->RSSetViewports(1, &viewport);

        const UINT stride = sizeof(Vertex);
        const UINT offset = 0;
        context_->IASetInputLayout(inputLayout_.Get());
        context_->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), &stride, &offset);
        context_->IASetIndexBuffer(indexBuffer_.Get(), DXGI_FORMAT_R16_UINT, 0);
        context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context_->VSSetShader(vertexShader_.Get(), nullptr, 0);
        context_->PSSetShader(pixelShader_.Get(), nullptr, 0);

        // 같은 메시를 두 번 그린다. 바뀌는 것은 World 행렬과 색뿐이다.
        DrawCube(cubeWorld_, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
        DrawCube(floorWorld_, Vector4(0.55f, 0.55f, 0.55f, 1.0f));
    }

    void DrawCube(const Matrix& world, const Vector4& tint) {
        // TODO(04): 이 물체의 상수를 채워 Constant Buffer에 올리고, 정점 셰이더 0번 슬롯에 바인딩한다.
        //   힌트: worldViewProj = (world * view_ * projection_).Transpose()  ← 왜 Transpose인지 설명할 수 있어야 한다
        //   힌트: context_->UpdateSubresource(constantBuffer_.Get(), 0, nullptr, &constants, 0, 0)
        //   힌트: context_->VSSetConstantBuffers(0, 1, constantBuffer_.GetAddressOf())
        (void)world;
        (void)tint;
        context_->DrawIndexed(ARRAYSIZE(kCubeIndices), 0, 0);
    }

    void OnPresent() override { swapChain_->Present(1, 0); }

    bool OnCapture(const std::wstring& path) override {
        return step::SaveTexturePng(context_.Get(), backBuffer_.Get(), path);
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
    TransformApp app;
    return app.Run(instance);
}
