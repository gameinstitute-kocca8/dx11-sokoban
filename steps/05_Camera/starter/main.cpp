// 05_Camera - Camera 클래스 (View, Projection), 궤도 카메라
//
// 확인 화면: 바닥 위에 3 x 3으로 놓인 큐브 9개를, 카메라가 위에서 비스듬히 내려다보며 천천히 한 바퀴 돈다.
// 04단계와 달라진 것은 '행렬을 누가 만드나'뿐이다. View와 Projection은 이제 Camera가 만든다.
#include "StepApp.h"

#include <d3d11.h>
#include <wrl/client.h>

#include <cstdint>

#include "Camera.h"
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

struct PerObjectConstants {
    Matrix worldViewProj;
    Vector4 tint;
};
static_assert(sizeof(PerObjectConstants) % 16 == 0, "Constant Buffer 크기는 16의 배수");

}  // namespace

class CameraApp final : public step::StepApp {
public:
    CameraApp() : StepApp(L"05 Camera") {}

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
    Camera camera_;
    float cameraYaw_ = 0.0f;

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
        D3D11_BUFFER_DESC cbDesc = {};
        cbDesc.ByteWidth = sizeof(PerObjectConstants);
        cbDesc.Usage = D3D11_USAGE_DEFAULT;
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        if (!step::Check(device_->CreateBuffer(&cbDesc, nullptr, &constantBuffer_), "CreateBuffer(constant)")) {
            return false;
        }

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
        // 카메라는 원점을 중심으로 9만큼 떨어져, 35도 위에서 내려다보며 초당 0.4 라디안씩 돈다.
        cameraYaw_ = totalSeconds * 0.4f;
        // TODO(05): Camera에 렌즈를 설정하고, 궤도 위치를 지정한다.
        //   힌트: 렌즈: 시야각 60도(XM_PI / 3), 화면비 Width / Height, 0.1 ~ 100  (04단계와 같은 값)
        //   힌트: 궤도: 중심 원점, 거리 9, yaw = cameraYaw_, pitch = 35도 (XMConvertToRadians)
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

        // 바닥: 큐브 하나를 납작하게 늘린 것
        DrawCube(Matrix::CreateScale(8.0f, 0.2f, 8.0f) * Matrix::CreateTranslation(0.0f, -0.6f, 0.0f),
                 Vector4(0.55f, 0.55f, 0.55f, 1.0f));

        // 3 x 3 큐브. 위치만 다르고 모두 같은 메시다. (08단계에서는 이 반복이 레벨 파일에서 나온다)
        for (int z = -1; z <= 1; ++z) {
            for (int x = -1; x <= 1; ++x) {
                DrawCube(Matrix::CreateTranslation(x * 2.0f, 0.0f, z * 2.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f));
            }
        }
    }

    void DrawCube(const Matrix& world, const Vector4& tint) {
        PerObjectConstants constants;
        // 04단계에서는 view_ * projection_ 이었다. 이제 카메라에게 묻는다.
        constants.worldViewProj = (world * camera_.View() * camera_.Projection()).Transpose();
        constants.tint = tint;
        context_->UpdateSubresource(constantBuffer_.Get(), 0, nullptr, &constants, 0, 0);
        context_->VSSetConstantBuffers(0, 1, constantBuffer_.GetAddressOf());
        context_->DrawIndexed(ARRAYSIZE(kCubeIndices), 0, 0);
    }

    void OnPresent() override { swapChain_->Present(1, 0); }

    bool OnCapture(const std::wstring& path) override {
        return step::SaveTexturePng(context_.Get(), backBuffer_.Get(), path);
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
    CameraApp app;
    return app.Run(instance);
}
