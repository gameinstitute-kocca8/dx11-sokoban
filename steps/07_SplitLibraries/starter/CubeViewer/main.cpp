// CubeViewer - Engine.lib + Render.lib만 링크한 두 번째 프로그램
//
// 확인 화면: 06단계와 같은 장면(바닥 + 3 x 3 큐브, 궤도 카메라, 방향광).
// 이 프로그램이 빌드되고 06과 같은 화면이 나오면, 엔진이 '재사용 가능하게' 나뉜 것이다.
//
// 이 파일에는 d3d11.h도, 셰이더도, 버퍼도 없다. 그것이 07단계의 수락 조건이다.
#include "Engine/Application.h"
#include "Engine/Camera.h"

using render::Matrix;
using render::Vector3;
using render::Vector4;

class CubeViewer final : public engine::Application {
public:
    CubeViewer() : Application(L"07 CubeViewer") {}

private:
    engine::Camera camera_;
    render::MeshHandle cube_;

    bool OnStart() override {
        // TODO(07): 큐브 메시를 하나 만든다. 만들지 못하면 false.
        return true;
    }

    void OnFrame(float deltaSeconds, float totalSeconds) override {
        (void)deltaSeconds;
        // TODO(07): 05~06단계와 같은 렌즈와 궤도를 설정한다. 화면비는 GetRenderer().AspectRatio().
        (void)totalSeconds;
    }

    void OnDraw(render::Renderer& renderer) override {
        // TODO(07): 카메라와 빛을 넘기고, 06단계와 같은 장면을 DrawMesh로 그린다.
        //   힌트: 빛: 방향 (0.45, -1, 0.6), 주변광 0.25. 색과 위치는 06단계 OnRender와 같다.
        (void)renderer;
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
    CubeViewer app;
    return app.Run(instance);
}
