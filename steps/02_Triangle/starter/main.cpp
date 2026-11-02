// 02_Triangle - Vertex Buffer, Input Layout, Vertex/Pixel Shader, Viewport, Draw
//
// 확인 화면: 01의 남색 배경 가운데에 꼭짓점마다 빨강/초록/파랑인 삼각형 하나.
#include "StepApp.h"

#include <d3d11.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace {

// 정점 하나의 메모리 배치. Input Layout과 HLSL의 VSInput이 이 배치와 정확히 맞아야 한다.
struct Vertex {
    float position[3];  // 클립 공간 좌표 (x, y: -1 ~ 1, z: 0 ~ 1)
    float color[4];     // RGBA
};

}  // namespace

class TriangleApp final : public step::StepApp {
public:
    TriangleApp() : StepApp(L"02 Triangle") {}

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

        // ---- 02: 정점 버퍼 ----
        // TODO(02): 삼각형 정점 3개로 Vertex Buffer를 만든다.
        //   힌트: 위(0, 0.5) 빨강 → 오른쪽 아래(0.5, -0.5) 초록 → 왼쪽 아래(-0.5, -0.5) 파랑. z는 0.5.
        //   힌트: 이 순서는 화면에서 시계 방향이다. D3D11 기본 설정은 시계 방향만 앞면으로 그린다.
        //   힌트: D3D11_BUFFER_DESC: ByteWidth, Usage DEFAULT, BindFlags VERTEX_BUFFER
        //   힌트: D3D11_SUBRESOURCE_DATA.pSysMem에 정점 배열을 넣고 device_->CreateBuffer

        // ---- 02: 셰이더와 Input Layout ----
        // TODO(02): shaders/Triangle.hlsl의 VSMain(vs_5_0), PSMain(ps_5_0)을 컴파일해 셰이더 객체를 만들고,
        //   힌트: step::CompileShader(L"shaders/Triangle.hlsl", "VSMain", "vs_5_0", &blob)
        //   힌트: Input Layout은 '정점 셰이더의 바이트코드'로 만든다. 그래서 VS blob을 PS보다 오래 들고 있어야 한다.
        //   힌트: D3D11_INPUT_ELEMENT_DESC 두 개: "POSITION" R32G32B32_FLOAT 오프셋 0, "COLOR" R32G32B32A32_FLOAT 오프셋 12
        return true;
    }

    void OnRender() override {
        const float clearColor[4] = {0.10f, 0.20f, 0.35f, 1.0f};
        context_->ClearRenderTargetView(renderTargetView_.Get(), clearColor);

        // TODO(02): 파이프라인 단계를 순서대로 설정하고 Draw로 정점 3개를 그린다.
        //   힌트: OM(렌더 타깃) → RS(뷰포트: 창 전체, 깊이 0~1) → IA(Input Layout, 정점 버퍼, TRIANGLELIST) → VS → PS → Draw(3, 0)
        //   힌트: 정점 버퍼의 stride는 sizeof(Vertex), offset은 0
    }

    void OnPresent() override { swapChain_->Present(1, 0); }

    bool OnCapture(const std::wstring& path) override {
        return step::SaveTexturePng(context_.Get(), backBuffer_.Get(), path);
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
    TriangleApp app;
    return app.Run(instance);
}
