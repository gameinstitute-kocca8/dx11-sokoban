// 14_AStarGrid - 13단계의 A*로 '클릭한 칸까지 걸어가기'
//
// 오른쪽 클릭한 칸까지 A*로 길을 찾고, 플레이어가 한 칸씩 걸어간다. 상자는 밀지 않고 피해 간다.
// 노란 칸 = 찾은 경로, 청록 점 = A*가 탐색한(closed) 칸. V로 탐색 표시를 켜고 끈다.
//
// 확인 화면 (CI는 ci/captures.txt의 두 장, levels/level03.txt)
//   start : 처음 상태
//   path  : --target 1,2 → 플레이어에서 왼쪽 위 (1, 2)까지의 경로와 탐색한 칸이 표시됨 (캡처는 시간이 흐르지 않아 출발 전 모습)
#include "Engine/Application.h"
#include "Engine/Camera.h"
#include "Game/AStar.h"
#include "Game/GridView.h"
#include "Game/PathFinder.h"
#include "Game/Picking.h"
#include "Game/PlayerController.h"
#include "Game/SokobanGame.h"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

using render::Matrix;
using render::Vector3;
using render::Vector4;

namespace {

const wchar_t* const kLevels[] = {L"levels/level01.txt", L"levels/level02.txt", L"levels/level03.txt"};
constexpr int kLevelCount = static_cast<int>(sizeof(kLevels) / sizeof(kLevels[0]));

constexpr float kCharWidth = 18.0f;
constexpr float kScreenWidth = 1280.0f;
constexpr float kStepSeconds = 0.12f;  // 자동 이동에서 한 칸에 걸리는 시간

float CenterX(const std::string& text, float scale) {
    return (kScreenWidth - static_cast<float>(text.size()) * kCharWidth * scale) * 0.5f;
}

// 칸 위에 얇은 표시판을 올린다 (바닥 판 윗면 y = -0.5 위)
Matrix MarkerMatrix(astar::Int2 cell, float size, float height) {
    const Vector3 p = game::CellToWorld(sokoban::Int2{cell.x, cell.y});
    return Matrix::CreateScale(size, height, size) * Matrix::CreateTranslation(p.x, -0.5f + height * 0.5f, p.z);
}

}  // namespace

class SokobanApp final : public engine::Application {
public:
    SokobanApp() : Application(L"14 A* Grid") {}

private:
    game::SokobanGame game_;
    std::vector<bool> inside_;
    engine::Camera camera_;
    render::MeshHandle cube_;
    float cameraYaw_ = DirectX::XM_PI;
    int levelIndex_ = 2;  // 가장 넓은 level03에서 시작
    bool customLevel_ = false;

    // 자동 이동 계획
    astar::Result search_;               // 마지막 A* 결과 (경로 + 탐색한 칸)
    std::vector<sokoban::Dir> plan_;     // 경로를 방향으로 바꾼 것
    size_t nextStep_ = 0;                // plan_에서 다음에 실행할 위치
    float stepTimer_ = 0.0f;
    bool showSearch_ = true;
    bool noPath_ = false;                // 마지막 클릭이 갈 수 없는 칸이었는지 (HUD 표시용)

    void ClearPlan() {
        search_ = astar::Result{};
        plan_.clear();
        nextStep_ = 0;
        stepTimer_ = 0.0f;
        noPath_ = false;
    }

    bool LoadLevel(const std::wstring& file) {
        if (!game_.Load(file)) return false;
        inside_ = game::ComputeInside(game_.GetLevel());
        cameraYaw_ = DirectX::XM_PI;
        ClearPlan();
        return true;
    }

    // 지금 위치에서 target 칸까지 길을 찾아 자동 이동 계획을 세운다
    void PlanTo(sokoban::Int2 target) {
        ClearPlan();
        const sokoban::Level& level = game_.GetLevel();
        if (!level.InBounds(target) || !inside_[level.Index(target)]) {
            noPath_ = true;  // 맵 밖이나 벽 바깥을 눌렀다
            return;
        }
        // TODO(14): 지금 상태로 걷기용 격자를 만들고 A*로 길을 찾아, 찾았으면 plan_을 채운다.
        //   힌트: MakeWalkGrid(level) → astar::FindPath(grid, ToAStar(level.player), ToAStar(target))
        //   힌트: 못 찾았으면 noPath_ = true. 찾았으면 PathToDirs(search_.path, plan_)
        //   힌트: search_에 결과를 남겨 두면 OnDraw가 경로와 탐색한 칸을 그린다
    }

    bool OnStart() override {
        cube_ = GetRenderer().CreateCube();
        if (!cube_.IsValid()) return false;
        if (!GetRenderer().LoadFont(L"fonts/ui.spritefont")) return false;

        customLevel_ = !GetOptions().level.empty();
        if (!LoadLevel(customLevel_ ? GetOptions().level : kLevels[levelIndex_])) return false;

        for (char c : GetOptions().moves) {
            sokoban::Dir dir;
            if (c == 'z') game_.Undo();
            else if (game::ScriptCharToDir(c, dir)) game_.Move(dir);
        }
        // 검증용: --target x,y 는 그 칸을 오른쪽 클릭한 것과 같다
        int tx = 0;
        int ty = 0;
        if (std::sscanf(GetOptions().target.c_str(), "%d,%d", &tx, &ty) == 2) PlanTo(sokoban::Int2{tx, ty});
        return true;
    }

    void OnFrame(float deltaSeconds, float totalSeconds) override {
        (void)totalSeconds;
        const engine::Input& input = GetInput();

        if (input.IsMouseLeftDown()) cameraYaw_ += input.MouseDeltaX() * 0.01f;
        if (input.WasPressed(engine::Key::V)) showSearch_ = !showSearch_;

        if (input.WasPressed(engine::Key::R)) {
            game_.Restart();
            ClearPlan();
        }
        if (input.WasPressed(engine::Key::Z)) {
            game_.Undo();
            ClearPlan();
        }
        if (!customLevel_ && input.WasPressed(engine::Key::N)) {
            levelIndex_ = (levelIndex_ + 1) % kLevelCount;
            LoadLevel(kLevels[levelIndex_]);
        }
        if (!customLevel_ && input.WasPressed(engine::Key::P)) {
            levelIndex_ = (levelIndex_ + kLevelCount - 1) % kLevelCount;
            LoadLevel(kLevels[levelIndex_]);
        }

        // 키보드로 움직이면 자동 이동을 멈춘다 (사람의 입력이 우선)
        sokoban::Dir dir;
        if (game::ReadMoveInput(input, cameraYaw_, dir)) {
            ClearPlan();
            game_.Move(dir);
        }

        // 오른쪽 클릭: 가리킨 칸까지 길 찾기. 카메라는 지난 프레임 것을 쓴다 (화면에 보이는 그대로)
        sokoban::Int2 cell;
        if (input.WasRightClicked() &&
            game::PickCell(camera_, static_cast<float>(input.MouseX()), static_cast<float>(input.MouseY()),
                           static_cast<float>(Width()), static_cast<float>(Height()), cell)) {
            PlanTo(cell);
        }

        // TODO(14): 계획이 남아 있으면 kStepSeconds마다 한 칸씩 game_.Move로 실행한다. 막히면 계획을 버린다.
        //   힌트: 키보드 이동과 같은 game_.Move를 쓰므로 이동 횟수, Clear 판정, Undo가 그대로 동작한다
        //   힌트: 경로를 다 걸었으면 plan_은 비우되, 표시(search_)는 남겨 두어도 된다

        if (game_.State() == game::GameState::Cleared) cameraYaw_ += deltaSeconds * 0.5f;

        camera_.SetLens(DirectX::XM_PI / 3.0f, GetRenderer().AspectRatio(), 0.1f, 100.0f);
        const sokoban::Level& level = game_.GetLevel();
        const float size = static_cast<float>(std::max(level.width, level.height));
        camera_.Orbit(game::LevelCenter(level), size * 0.9f + 4.0f, cameraYaw_, DirectX::XMConvertToRadians(55.0f));
    }

    static void Text(render::Renderer& r, const std::string& s, float x, float y, const Vector4& color, float scale) {
        r.DrawScreenText(s, x + 2.0f, y + 2.0f, Vector4(0.0f, 0.0f, 0.0f, 0.8f), scale);
        r.DrawScreenText(s, x, y, color, scale);
    }

    void DrawSearch(render::Renderer& renderer) {
        const Vector4 searched(0.15f, 0.65f, 0.70f, 1.0f);  // 청록: 탐색한 칸
        const Vector4 path(0.95f, 0.85f, 0.20f, 1.0f);      // 노랑: 경로
        if (showSearch_) {
            for (const astar::Int2& c : search_.expanded) renderer.DrawMesh(cube_, MarkerMatrix(c, 0.22f, 0.04f), searched);
        }
        // 아직 걷지 않은 경로만 그린다. path[0]은 출발 칸이므로 nextStep_ + 1부터
        for (size_t i = nextStep_ + 1; i < search_.path.size(); ++i) {
            const float s = (i + 1 == search_.path.size()) ? 0.7f : 0.45f;  // 목표 칸은 크게
            renderer.DrawMesh(cube_, MarkerMatrix(search_.path[i], s, 0.06f), path);
        }
    }

    void OnDraw(render::Renderer& renderer) override {
        renderer.SetCamera(camera_.View(), camera_.Projection());
        renderer.SetLight(Vector3(0.45f, -1.0f, -0.6f), 0.3f);
        if (!inside_.empty()) game::DrawLevel(renderer, cube_, game_.GetLevel(), inside_);
        DrawSearch(renderer);

        const Vector4 white(1.0f, 1.0f, 1.0f, 1.0f);
        std::string status = "Moves: " + std::to_string(game_.Moves());
        if (!customLevel_) status = "Level " + std::to_string(levelIndex_ + 1) + "/" + std::to_string(kLevelCount) + "   " + status;
        if (search_.found) {
            status += "   Path: " + std::to_string(search_.path.size() - 1) + "   Searched: " + std::to_string(search_.expanded.size());
        }
        if (noPath_) status += "   No path";
        Text(renderer, status, 20.0f, 16.0f, white, 1.0f);
        Text(renderer, "WASD: Move   Right click: Go   V: Search view   Left drag: Camera   Z: Undo   R: Restart   N/P: Level",
             20.0f, 680.0f, Vector4(0.85f, 0.85f, 0.85f, 1.0f), 0.6f);

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
