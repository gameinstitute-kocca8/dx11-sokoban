// 08_GridMap - 00단계의 레벨 파일을 3D 맵으로
//
// 확인 화면: levels/level03.txt가 회색 벽, 바닥, 파란 목표, 나무색 상자, 빨간 플레이어로 보인다.
// 카메라는 지도의 아래쪽(+Z)에서 55도로 내려다본다. 텍스트 지도와 화면의 위/아래/좌/우가 같다.
//
// 아직 움직이지 않는다. 입력은 09단계.
#include "Engine/Application.h"
#include "Engine/Camera.h"
#include "Game/GridView.h"
#include "Game/Sokoban.h"

#include <algorithm>
#include <vector>

using render::Vector3;

class SokobanApp final : public engine::Application {
public:
    SokobanApp() : Application(L"08 Grid Map") {}

private:
    sokoban::Level level_;
    std::vector<bool> inside_;
    engine::Camera camera_;
    render::MeshHandle cube_;

    bool OnStart() override {
        cube_ = GetRenderer().CreateCube();
        if (!cube_.IsValid()) return false;
        // TODO(08): 레벨 파일을 읽고 inside_를 계산한다. 파일은 --level 옵션이 있으면 그것, 없으면 levels/level03.txt
        //   힌트: GetOptions().level 이 비어 있는지 확인한다. 읽기 실패면 false를 돌려 프로그램을 끝낸다.
        return true;
    }

    void OnFrame(float deltaSeconds, float totalSeconds) override {
        (void)deltaSeconds;
        (void)totalSeconds;
        // TODO(08): 레벨 한가운데를 지도 아래쪽(+Z)에서 55도로 내려다본다. 거리는 레벨 크기에 맞춘다.
        //   힌트: yaw = π 이면 카메라가 target의 +Z 쪽에 온다 (05단계 Orbit의 식을 다시 보자)
        //   힌트: 거리 = max(가로, 세로) * 0.9 + 4 정도면 레벨 전체가 들어온다
    }

    void OnDraw(render::Renderer& renderer) override {
        renderer.SetCamera(camera_.View(), camera_.Projection());
        renderer.SetLight(Vector3(0.45f, -1.0f, -0.6f), 0.3f);
        if (!inside_.empty()) game::DrawLevel(renderer, cube_, level_, inside_);
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
    SokobanApp app;
    return app.Run(instance);
}
