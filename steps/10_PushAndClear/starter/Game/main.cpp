// 10_PushAndClear - 한 판의 흐름: 이동 횟수, Clear 상태, 검증용 이동 스크립트
//
// 확인 화면 (CI는 ci/captures.txt에 적힌 세 장을 찍는다, levels/level01.txt)
//   start : 처음 상태
//   push  : --moves d   → 상자가 한 칸 밀린 상태
//   clear : --moves dd  → 상자가 목표 위(초록), 창 제목에 CLEAR
// 창 제목에 이동 횟수가 나온다. 화면 글자(UI)는 11단계.
#include "Engine/Application.h"
#include "Engine/Camera.h"
#include "Game/GridView.h"
#include "Game/PlayerController.h"
#include "Game/SokobanGame.h"

#include <algorithm>
#include <string>
#include <vector>

using render::Vector3;

class SokobanApp final : public engine::Application {
public:
    SokobanApp() : Application(L"10 Push & Clear") {}

private:
    game::SokobanGame game_;
    std::vector<bool> inside_;
    engine::Camera camera_;
    render::MeshHandle cube_;
    float cameraYaw_ = DirectX::XM_PI;  // π: 지도 아래쪽(+Z)에서 바라봄 (08단계)
    int shownMoves_ = -1;               // 창 제목을 이동 횟수가 바뀔 때만 갱신하기 위해

    bool OnStart() override {
        cube_ = GetRenderer().CreateCube();
        if (!cube_.IsValid()) return false;
        const std::wstring file = GetOptions().level.empty() ? L"levels/level01.txt" : GetOptions().level;
        if (!game_.Load(file)) return false;
        inside_ = game::ComputeInside(game_.GetLevel());

        // TODO(10): --moves 옵션의 글자들을 순서대로 game_.Move에 적용한다 (CI가 '밀기 후' 화면을 찍을 때 쓴다)
        //   힌트: ScriptCharToDir로 글자를 방향으로 바꾼다. 모르는 글자는 건너뛴다.
        return true;
    }

    void OnFrame(float deltaSeconds, float totalSeconds) override {
        (void)totalSeconds;
        const engine::Input& input = GetInput();

        if (input.IsMouseLeftDown()) cameraYaw_ += input.MouseDeltaX() * 0.01f;

        // 09단계에서는 TryMove를 바로 불렀다. 이제 '한 판'의 규칙을 아는 game_에게 맡긴다.
        sokoban::Dir dir;
        if (game::ReadMoveInput(input, cameraYaw_, dir)) game_.Move(dir);

        // TODO(10): Clear가 되면 카메라를 천천히(초당 0.5 라디안) 돌려 축하한다. 이동 횟수나 상태가 바뀌면 창 제목을 바꾼다.
        //   힌트: 창 제목: SetWindowTextW(Window(), L"...").  숫자는 std::to_wstring(game_.Moves())
        //   힌트: 제목을 매 프레임 바꾸지 않는 이유를 설명할 수 있어야 한다 (shownMoves_)

        camera_.SetLens(DirectX::XM_PI / 3.0f, GetRenderer().AspectRatio(), 0.1f, 100.0f);
        // 맵 중심 카메라 (09단계): 레벨 한가운데를 중심으로 마우스 yaw만큼 돌린다
        const sokoban::Level& level = game_.GetLevel();
        const float size = static_cast<float>(std::max(level.width, level.height));
        camera_.Orbit(game::LevelCenter(level), size * 0.9f + 4.0f, cameraYaw_, DirectX::XMConvertToRadians(55.0f));
    }

    void OnDraw(render::Renderer& renderer) override {
        renderer.SetCamera(camera_.View(), camera_.Projection());
        renderer.SetLight(Vector3(0.45f, -1.0f, -0.6f), 0.3f);
        if (!inside_.empty()) game::DrawLevel(renderer, cube_, game_.GetLevel(), inside_);
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
    SokobanApp app;
    return app.Run(instance);
}
