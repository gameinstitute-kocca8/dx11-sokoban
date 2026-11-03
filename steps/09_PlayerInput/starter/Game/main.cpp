// 09_PlayerInput - 키 입력, 카메라 기준 방향, 3인칭 카메라
//
// 확인 화면: levels/level02.txt. 카메라가 빨간 플레이어를 중심으로 50도 위에서 내려다본다.
// 실행해서 확인할 것: W A S D(또는 방향키)로 한 칸씩 이동, 상자 밀기, 마우스 왼쪽 드래그로 카메라 회전.
// 카메라를 90도 돌린 뒤에도 W는 '화면의 위쪽'으로 가야 한다.
#include "Engine/Application.h"
#include "Engine/Camera.h"
#include "Game/GridView.h"
#include "Game/PlayerController.h"
#include "Game/Sokoban.h"

#include <vector>

using render::Vector3;

class SokobanApp final : public engine::Application {
public:
    SokobanApp() : Application(L"09 Player Input") {}

private:
    sokoban::Level level_;
    std::vector<bool> inside_;
    engine::Camera camera_;
    render::MeshHandle cube_;
    float cameraYaw_ = DirectX::XM_PI;  // π: 지도 아래쪽(+Z)에서 바라봄 (08단계)

    bool OnStart() override {
        cube_ = GetRenderer().CreateCube();
        if (!cube_.IsValid()) return false;
        const std::wstring file = GetOptions().level.empty() ? L"levels/level02.txt" : GetOptions().level;
        if (!game::LoadLevelFile(file, level_)) return false;
        inside_ = game::ComputeInside(level_);
        return true;
    }

    void OnFrame(float deltaSeconds, float totalSeconds) override {
        (void)deltaSeconds;
        (void)totalSeconds;
        const engine::Input& input = GetInput();

        // TODO(09): 마우스 왼쪽 버튼을 누른 채 가로로 끌면 카메라가 돈다. 1픽셀당 0.01 라디안

        // TODO(09): 이동 키가 눌린 순간이면 00단계의 TryMove로 규칙을 적용한다.
        //   힌트: 이동이 성공했는지는 여기서 신경 쓰지 않는다. 막혔으면 TryMove가 상태를 바꾸지 않는다 (00단계에서 보장)

        // TODO(09): 3인칭 카메라: 플레이어 칸의 월드 위치를 중심으로, 거리 8, 높이 각 50도
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
