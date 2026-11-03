// 11_UIRestart - 화면 글자(HUD), R 재시작, N/P 레벨 바꾸기
//
// 확인 화면 (CI는 ci/captures.txt의 두 장, levels/level01.txt)
//   start : 왼쪽 위 "Level 1/3  Moves: 0", 아래쪽 조작 안내
//   clear : --moves dd → 가운데 큰 "CLEAR!"와 다음 레벨 안내
#include "Engine/Application.h"
#include "Engine/Camera.h"
#include "Game/GridView.h"
#include "Game/PlayerController.h"
#include "Game/SokobanGame.h"

#include <string>
#include <vector>

using render::Vector3;
using render::Vector4;

namespace {

const wchar_t* const kLevels[] = {L"levels/level01.txt", L"levels/level02.txt", L"levels/level03.txt"};
constexpr int kLevelCount = static_cast<int>(sizeof(kLevels) / sizeof(kLevels[0]));

// 글꼴(ui.spritefont)은 고정폭이라 글자 하나의 폭이 같다. 가운데 정렬에만 쓴다.
constexpr float kCharWidth = 18.0f;
constexpr float kScreenWidth = 1280.0f;

float CenterX(const std::string& text, float scale) {
    return (kScreenWidth - static_cast<float>(text.size()) * kCharWidth * scale) * 0.5f;
}

}  // namespace

class SokobanApp final : public engine::Application {
public:
    SokobanApp() : Application(L"11 UI & Restart") {}

private:
    game::SokobanGame game_;
    std::vector<bool> inside_;
    engine::Camera camera_;
    render::MeshHandle cube_;
    float cameraYaw_ = DirectX::XM_PI;
    int levelIndex_ = 0;
    bool customLevel_ = false;  // --level로 파일을 직접 지정했으면 N/P를 쓰지 않는다

    bool LoadLevel(const std::wstring& file) {
        if (!game_.Load(file)) return false;
        inside_ = game::ComputeInside(game_.GetLevel());
        cameraYaw_ = DirectX::XM_PI;
        return true;
    }

    bool OnStart() override {
        cube_ = GetRenderer().CreateCube();
        if (!cube_.IsValid()) return false;
        // TODO(11): 글꼴을 읽는다. 실패하면 false (글자 없이 게임을 계속하지 않는다).

        customLevel_ = !GetOptions().level.empty();
        if (!LoadLevel(customLevel_ ? GetOptions().level : kLevels[levelIndex_])) return false;

        for (char c : GetOptions().moves) {
            sokoban::Dir dir;
            if (game::ScriptCharToDir(c, dir)) game_.Move(dir);
        }
        return true;
    }

    void OnFrame(float deltaSeconds, float totalSeconds) override {
        (void)totalSeconds;
        const engine::Input& input = GetInput();

        if (input.IsMouseLeftDown()) cameraYaw_ += input.MouseDeltaX() * 0.01f;

        // TODO(11): R: 재시작, N/P: 다음/이전 레벨 (--level로 직접 지정한 경우는 제외). '눌린 순간'만.
        //   힌트: 레벨 번호는 0 ~ kLevelCount-1 안에서 돈다: (i + 1) % n, (i + n - 1) % n

        sokoban::Dir dir;
        if (game::ReadMoveInput(input, cameraYaw_, dir)) game_.Move(dir);

        if (game_.State() == game::GameState::Cleared) cameraYaw_ += deltaSeconds * 0.5f;

        camera_.SetLens(DirectX::XM_PI / 3.0f, GetRenderer().AspectRatio(), 0.1f, 100.0f);
        camera_.Orbit(game::CellToWorld(game_.GetLevel().player), 8.0f, cameraYaw_,
                      DirectX::XMConvertToRadians(50.0f));
    }

    // 그림자를 깔아 밝은 바닥 위에서도 읽히게 한다
    static void Text(render::Renderer& r, const std::string& s, float x, float y, const Vector4& color, float scale) {
        r.DrawScreenText(s, x + 2.0f, y + 2.0f, Vector4(0.0f, 0.0f, 0.0f, 0.8f), scale);
        r.DrawScreenText(s, x, y, color, scale);
    }

    void OnDraw(render::Renderer& renderer) override {
        renderer.SetCamera(camera_.View(), camera_.Projection());
        renderer.SetLight(Vector3(0.45f, -1.0f, -0.6f), 0.3f);
        if (!inside_.empty()) game::DrawLevel(renderer, cube_, game_.GetLevel(), inside_);

        // TODO(11): HUD를 그린다. 이 함수는 상태를 '읽기만' 한다 (Moves, State, levelIndex_).
        //   힌트: 왼쪽 위 (20, 16): "Level 1/3   Moves: 0"   아래 (20, 680): 조작 안내, 크기 0.7
        //   힌트: Clear면 가운데 "CLEAR!" (크기 2.2, y = 250)와 "Moves: N   N: Next level" (y = 340)
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
    SokobanApp app;
    return app.Run(instance);
}
