// 03_IndexDepth - Index Buffer, Depth Buffer, DrawIndexed
//
// 확인 화면: 겹쳐진 사각형 두 개. 왼쪽 위 빨간 사각형(z = 0.3)이 오른쪽 아래 파란 사각형(z = 0.6)보다
// '앞'에 보인다. 그리는 순서는 빨강 → 파랑이므로, 깊이 버퍼가 없으면 겹친 부분이 파랗게 덮인다.
//
// 왜 아직 정육면체가 아닌가?
//   원근감 있는 정육면체에는 투영 행렬이 필요하고, 행렬을 셰이더에 넘기려면 Constant Buffer가 필요하다.
//   그건 04단계의 개념이다. 이 단계는 '정점 재사용(인덱스)'과 '앞뒤 판정(깊이)'만 확인한다.
#include "StepApp.h"

#include <d3d11.h>
#include <wrl/client.h>

#include <cstdint>

using Microsoft::WRL::ComPtr;

namespace {

struct Vertex {
    float position[3];
    float color[4];
};

}  // namespace

class IndexDepthApp final : public step::StepApp {
public:
    IndexDepthApp() : StepApp(L"03 Index Buffer & Depth") {}

private:
    ComPtr<ID3D11Device> device_;
    ComPtr<ID3D11DeviceContext> context_;
    ComPtr<IDXGISwapChain> swapChain_;
    ComPtr<ID3D11Texture2D> backBuffer_;
    ComPtr<ID3D11RenderTargetView> renderTargetView_;

    ComPtr<ID3D11Buffer> vertexBuffer_;
    ComPtr<ID3D11VertexShader> vertexShader_;
    ComPtr<ID3D11PixelShader> pixelShader_;
    ComPtr<ID3D11InputLayout> inputLayout_;

    ComPtr<ID3D11Buffer> indexBuffer_;
    ComPtr<ID3D11Texture2D> depthTexture_;
    ComPtr<ID3D11DepthStencilView> depthStencilView_;

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

        // ---- 02 → 03: 정점 버퍼 (사각형 두 개 = 정점 8개) ----
        // 각 사각형은 정점 4개만 가진다. 삼각형 2개를 만들 때 필요한 6개 중 2개는 인덱스로 재사용한다.
        const Vertex vertices[] = {
            // 0~3: 빨간 사각형, 왼쪽 위, z = 0.3 (가까움)
            {{-0.6f, 0.6f, 0.3f}, {0.9f, 0.2f, 0.2f, 1.0f}},
            {{0.2f, 0.6f, 0.3f}, {0.9f, 0.2f, 0.2f, 1.0f}},
            {{0.2f, -0.2f, 0.3f}, {0.9f, 0.2f, 0.2f, 1.0f}},
            {{-0.6f, -0.2f, 0.3f}, {0.9f, 0.2f, 0.2f, 1.0f}},
            // 4~7: 파란 사각형, 오른쪽 아래, z = 0.6 (멂)
            {{-0.2f, 0.2f, 0.6f}, {0.2f, 0.4f, 0.95f, 1.0f}},
            {{0.6f, 0.2f, 0.6f}, {0.2f, 0.4f, 0.95f, 1.0f}},
            {{0.6f, -0.6f, 0.6f}, {0.2f, 0.4f, 0.95f, 1.0f}},
            {{-0.2f, -0.6f, 0.6f}, {0.2f, 0.4f, 0.95f, 1.0f}},
        };
        D3D11_BUFFER_DESC vbDesc = {};
        vbDesc.ByteWidth = sizeof(vertices);
        vbDesc.Usage = D3D11_USAGE_DEFAULT;
        vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        D3D11_SUBRESOURCE_DATA vbData = {};
        vbData.pSysMem = vertices;
        if (!step::Check(device_->CreateBuffer(&vbDesc, &vbData, &vertexBuffer_), "CreateBuffer(vertex)")) {
            return false;
        }

        // ---- 03: 인덱스 버퍼 ----
        // TODO(03): 사각형 두 개(삼각형 4개)를 그리는 인덱스 12개로 Index Buffer를 만든다.
        //   힌트: 사각형 하나 = 삼각형 (0,1,2) + (0,2,3). 정점이 왼쪽 위부터 시계 방향으로 놓여 있다.
        //   힌트: 빨간 사각형 인덱스를 먼저, 파란 사각형 인덱스를 나중에 둔다. (그리는 순서 = 인덱스 순서)
        //   힌트: 형식은 std::uint16_t, BindFlags는 D3D11_BIND_INDEX_BUFFER

        // ---- 03: 깊이 버퍼 ----
        // TODO(03): 백 버퍼와 같은 크기의 깊이 텍스처와 Depth Stencil View를 만든다.
        //   힌트: D3D11_TEXTURE2D_DESC: Width/Height = 창 크기, MipLevels 1, ArraySize 1,
        //   힌트: Format D24_UNORM_S8_UINT, SampleDesc.Count 1, BindFlags DEPTH_STENCIL
        //   힌트: device_->CreateDepthStencilView(depthTexture_.Get(), nullptr, &depthStencilView_)

        // ---- 02: 셰이더와 Input Layout ----
        ComPtr<ID3DBlob> vsBlob;
        ComPtr<ID3DBlob> psBlob;
        if (!step::CompileShader(L"shaders/Color.hlsl", "VSMain", "vs_5_0", &vsBlob)) return false;
        if (!step::CompileShader(L"shaders/Color.hlsl", "PSMain", "ps_5_0", &psBlob)) return false;
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

    void OnRender() override {
        const float clearColor[4] = {0.10f, 0.20f, 0.35f, 1.0f};
        context_->ClearRenderTargetView(renderTargetView_.Get(), clearColor);

        ID3D11DepthStencilView* depthView = nullptr;
        // TODO(03): 깊이 버퍼를 1.0(가장 먼 값)으로 지우고, 아래 OMSetRenderTargets에 함께 넘긴다.
        //   힌트: ClearDepthStencilView(view, D3D11_CLEAR_DEPTH, 1.0f, 0)
        //   힌트: 매 프레임 지우지 않으면 무슨 일이 생길지 먼저 예측해 보자.
        context_->OMSetRenderTargets(1, renderTargetView_.GetAddressOf(), depthView);

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
        context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        context_->VSSetShader(vertexShader_.Get(), nullptr, 0);
        context_->PSSetShader(pixelShader_.Get(), nullptr, 0);

        // TODO(03): Index Buffer를 바인딩하고 DrawIndexed로 인덱스 12개를 그린다.
        //   힌트: IASetIndexBuffer(버퍼, DXGI_FORMAT_R16_UINT, 0) → DrawIndexed(12, 0, 0)
    }

    void OnPresent() override { swapChain_->Present(1, 0); }

    bool OnCapture(const std::wstring& path) override {
        return step::SaveTexturePng(context_.Get(), backBuffer_.Get(), path);
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
    IndexDepthApp app;
    return app.Run(instance);
}
