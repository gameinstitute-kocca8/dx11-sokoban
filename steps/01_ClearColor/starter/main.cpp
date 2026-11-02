// 01_ClearColor - Device, SwapChain, Render Target View, Clear, Present
//
// 확인 화면: 창 전체가 한 가지 색(짙은 남색)으로 채워진다.
#include "StepApp.h"

#include <d3d11.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class ClearColorApp final : public step::StepApp {
public:
    ClearColorApp() : StepApp(L"01 Clear Color") {}

private:
    ComPtr<ID3D11Device> device_;                    // 리소스를 '만드는' 객체
    ComPtr<ID3D11DeviceContext> context_;            // 그리기 명령을 '보내는' 객체
    ComPtr<IDXGISwapChain> swapChain_;               // 백 버퍼와 화면을 바꿔 주는 객체
    ComPtr<ID3D11Texture2D> backBuffer_;             // 우리가 그릴 텍스처
    ComPtr<ID3D11RenderTargetView> renderTargetView_;  // 백 버퍼를 '그리기 대상'으로 보는 뷰

    bool OnInit() override {
        // TODO(01): D3D11CreateDeviceAndSwapChain으로 Device, DeviceContext, SwapChain을 만든다.
        //   힌트: DXGI_SWAP_CHAIN_DESC: BufferCount 1, 크기 Width() x Height(), 형식 R8G8B8A8_UNORM,
        //   힌트: BufferUsage RENDER_TARGET_OUTPUT, OutputWindow Window(), SampleDesc.Count 1, Windowed TRUE
        //   힌트: 드라이버 종류는 UseWarp()가 true면 D3D_DRIVER_TYPE_WARP, 아니면 HARDWARE (CI 캡처용)
        //   힌트: 디버그 빌드에서는 D3D11_CREATE_DEVICE_DEBUG 플래그를 켜서 경고를 출력 창에서 본다.

        // TODO(01): SwapChain에서 백 버퍼를 꺼내고, 그 백 버퍼의 Render Target View를 만든다.
        //   힌트: swapChain_->GetBuffer(0, IID_PPV_ARGS(&backBuffer_))
        //   힌트: device_->CreateRenderTargetView(백 버퍼, nullptr, &renderTargetView_)
        return true;
    }

    void OnRender() override {
        // TODO(01): Render Target View를 한 가지 색으로 지운다. 색: R 0.10, G 0.20, B 0.35, A 1.0
    }

    void OnPresent() override {
        // TODO(01): 다 그린 백 버퍼를 화면에 내보낸다. 수직 동기화를 켠다(SyncInterval 1).
    }

    bool OnCapture(const std::wstring& path) override {
        return step::SaveTexturePng(context_.Get(), backBuffer_.Get(), path);
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
    ClearColorApp app;
    return app.Run(instance);
}
