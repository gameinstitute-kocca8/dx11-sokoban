// 12_Optional - 선택 과제 예시: Undo (Z)
//
// 다른 선택 과제(부드러운 이동, 텍스처, 레벨 추가)는 TASK.md 참고. 이 정답은 Undo만 구현한다.
//
// 확인 화면 (CI는 ci/captures.txt의 두 장, levels/level01.txt)
//   start : HUD 안내에 "Z: Undo"가 추가됨
//   undo  : --moves ddz → Clear 직전으로 되돌아감 (Moves: 1, CLEAR 없음)
#include "Engine/Application.h"
#include "Engine/Camera.h"
#include "Game/GridView.h"
#include "Game/PlayerController.h"
#include "Game/SokobanGame.h"

#include <algorithm>
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
    SokobanApp() : Application(L"12 Undo") {}

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
        if (!GetRenderer().LoadFont(L"fonts/ui.spritefont")) return false;

        customLevel_ = !GetOptions().level.empty();
        if (!LoadLevel(customLevel_ ? GetOptions().level : kLevels[levelIndex_])) return false;

        for (char c : GetOptions().moves) {
            sokoban::Dir dir;
            if (c == 'z') game_.Undo();  // 12단계: 검증 스크립트에서 z = 되돌리기
            else if (game::ScriptCharToDir(c, dir)) game_.Move(dir);
        }
        return true;
    }

    void OnFrame(float deltaSeconds, float totalSeconds) override {
        (void)totalSeconds;
        const engine::Input& input = GetInput();

        if (input.IsMouseLeftDown()) cameraYaw_ += input.MouseDeltaX() * 0.01f;

        if (input.WasPressed(engine::Key::R)) game_.Restart();
        // TODO(12): Z: 되돌리기
        if (!customLevel_ && input.WasPressed(engine::Key::N)) {
            levelIndex_ = (levelIndex_ + 1) % kLevelCount;
            LoadLevel(kLevels[levelIndex_]);
        }
        if (!customLevel_ && input.WasPressed(engine::Key::P)) {
            levelIndex_ = (levelIndex_ + kLevelCount - 1) % kLevelCount;
            LoadLevel(kLevels[levelIndex_]);
        }

        sokoban::Dir dir;
        if (game::ReadMoveInput(input, cameraYaw_, dir)) game_.Move(dir);

        if (game_.State() == game::GameState::Cleared) cameraYaw_ += deltaSeconds * 0.5f;

        camera_.SetLens(DirectX::XM_PI / 3.0f, GetRenderer().AspectRatio(), 0.1f, 100.0f);
        // 맵 중심 카메라 (09단계): 레벨 한가운데를 중심으로 마우스 yaw만큼 돌린다
        const sokoban::Level& level = game_.GetLevel();
        const float size = static_cast<float>(std::max(level.width, level.height));
        camera_.Orbit(game::LevelCenter(level), size * 0.9f + 4.0f, cameraYaw_, DirectX::XMConvertToRadians(55.0f));
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

        const Vector4 white(1.0f, 1.0f, 1.0f, 1.0f);
        std::string status = "Moves: " + std::to_string(game_.Moves());
        if (!customLevel_) status = "Level " + std::to_string(levelIndex_ + 1) + "/" + std::to_string(kLevelCount) + "   " + status;
        Text(renderer, status, 20.0f, 16.0f, white, 1.0f);
        Text(renderer, "WASD: Move   Mouse drag: Camera   Z: Undo   R: Restart   N/P: Level", 20.0f, 680.0f,
             Vector4(0.85f, 0.85f, 0.85f, 1.0f), 0.7f);

        if (game_.State() == game::GameState::Cleared) {
            const std::string title = "CLEAR!";
            const std::string detail = "Moves: " + std::to_string(game_.Moves()) + "   N: Next level";
            Text(renderer, title, CenterX(title, 2.2f), 90.0f, Vector4(1.0f, 0.85f, 0.2f, 1.0f), 2.2f);
            Text(renderer, detail, CenterX(detail, 1.0f), 180.0f, white, 1.0f);
        }
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
    SokobanApp app;
    return app.Run(instance);
}
