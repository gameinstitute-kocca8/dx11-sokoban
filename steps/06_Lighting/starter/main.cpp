// 06_Lighting - 법선(normal), 방향광, Lambert 조명
//
// 확인 화면: 05단계와 같은 장면이지만, 면마다 칠한 색 대신 '재질 색 × 빛의 세기'로 그린다.
// 빛을 정면으로 받는 면은 밝고, 비스듬한 면은 어둡다. 큐브 색은 소코반의 색을 미리 써 본다.
//   회색 = 벽, 빨강 = 플레이어, 파랑 = 목표, 나무색 = 상자
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

// 05단계까지는 정점마다 색을 넣었다. 이제 색은 물체(재질)가 정하고, 정점은 '면이 향하는 방향'을 가진다.
struct Vertex {
    float position[3];
    float normal[3];  // 바깥을 향하는 단위 벡터
};

// 정육면체: 면마다 법선이 다르므로 면마다 정점 4개씩, 총 24개 (04단계와 같은 위치, 같은 감김 순서)
const Vertex kCubeVertices[] = {
    // +X
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
    // -X
    {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}},
    {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}},
    // +Y
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    // -Y
    {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}},
    // +Z
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    // -Z
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
};

const std::uint16_t kCubeIndices[] = {
    0,  1,  2,  0,  2,  3,   // +X
    4,  5,  6,  4,  6,  7,   // -X
    8,  9,  10, 8,  10, 11,  // +Y
    12, 13, 14, 12, 14, 15,  // -Y
    16, 17, 18, 16, 18, 19,  // +Z
    20, 21, 22, 20, 22, 23,  // -Z
};

// HLSL의 cbuffer PerObject와 같은 배치: worldViewProj(64) → world(64) → color(16) → lightDirection(16) = 160바이트
struct PerObjectConstants {
    Matrix worldViewProj;
    // TODO(06): 법선을 돌리기 위한 World 행렬을 추가한다.
    //   힌트: 법선은 World로 회전시켜야 하므로 WorldViewProj만으로는 부족하다.
    Vector4 color;  // 재질 색 (04~05단계의 tint)
    // TODO(06): 빛 방향(xyz)과 주변광 세기(w)를 담을 Vector4를 추가한다.
};
static_assert(sizeof(PerObjectConstants) % 16 == 0, "Constant Buffer 크기는 16의 배수");

}  // namespace

class LightingApp final : public step::StepApp {
public:
    LightingApp() : StepApp(L"06 Lighting") {}

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
    // 빛이 나아가는 방향: 위쪽의 -X, -Z 쪽에서 아래쪽 +X, +Z 쪽으로. 주변광은 0.25
    Vector3 lightDirection_ = Vector3(0.45f, -1.0f, 0.6f);
    float ambient_ = 0.25f;

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
        if (!step::CompileShader(L"shaders/Lit.hlsl", "VSMain", "vs_5_0", &vsBlob)) return false;
        if (!step::CompileShader(L"shaders/Lit.hlsl", "PSMain", "ps_5_0", &psBlob)) return false;
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
            // TODO(06): 두 번째 정점 요소를 COLOR(float4)에서 NORMAL(float3)로 바꾼다.
            //   힌트: 시맨틱 "NORMAL", 형식 R32G32B32_FLOAT, 오프셋 12. HLSL VSInput과 맞아야 한다.
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
        const float aspect = static_cast<float>(Width()) / static_cast<float>(Height());
        camera_.SetLens(DirectX::XM_PI / 3.0f, aspect, 0.1f, 100.0f);
        camera_.Orbit(Vector3::Zero, 9.0f, cameraYaw_, DirectX::XMConvertToRadians(35.0f));
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

        // 바닥
        DrawCube(Matrix::CreateScale(8.0f, 0.2f, 8.0f) * Matrix::CreateTranslation(0.0f, -0.6f, 0.0f),
                 Vector4(0.75f, 0.75f, 0.72f, 1.0f));

        // 3 x 3 큐브: 소코반 색 미리보기. 가운데 빨강(플레이어), 오른쪽 앞 파랑(목표), 왼쪽 앞 나무색(상자), 나머지 회색(벽)
        const Vector4 wall(0.45f, 0.45f, 0.50f, 1.0f);
        for (int z = -1; z <= 1; ++z) {
            for (int x = -1; x <= 1; ++x) {
                Vector4 color = wall;
                if (x == 0 && z == 0) color = Vector4(0.90f, 0.20f, 0.20f, 1.0f);
                if (x == -1 && z == -1) color = Vector4(0.20f, 0.40f, 0.95f, 1.0f);
                if (x == 1 && z == -1) color = Vector4(0.80f, 0.55f, 0.25f, 1.0f);
                DrawCube(Matrix::CreateTranslation(x * 2.0f, 0.0f, z * 2.0f), color);
            }
        }
    }

    void DrawCube(const Matrix& world, const Vector4& color) {
        PerObjectConstants constants;
        constants.worldViewProj = (world * camera_.View() * camera_.Projection()).Transpose();
        constants.color = color;
        // TODO(06): World 행렬과 빛 방향(정규화)·주변광을 상수에 채운다.
        //   힌트: world도 Transpose해서 넣는다 (worldViewProj와 같은 이유)
        //   힌트: 빛 방향은 길이 1이어야 dot 결과가 cos이 된다. Vector3::Normalize() 또는 Normalize(out)
        context_->UpdateSubresource(constantBuffer_.Get(), 0, nullptr, &constants, 0, 0);
        context_->VSSetConstantBuffers(0, 1, constantBuffer_.GetAddressOf());
        context_->PSSetConstantBuffers(0, 1, constantBuffer_.GetAddressOf());  // 픽셀 셰이더도 color, 빛을 읽는다
        context_->DrawIndexed(ARRAYSIZE(kCubeIndices), 0, 0);
    }

    void OnPresent() override { swapChain_->Present(1, 0); }

    bool OnCapture(const std::wstring& path) override {
        return step::SaveTexturePng(context_.Get(), backBuffer_.Get(), path);
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
    LightingApp app;
    return app.Run(instance);
}
