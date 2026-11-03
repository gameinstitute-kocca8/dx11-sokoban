// GridView.cpp
#include "Game/GridView.h"

#include <filesystem>
#include <fstream>
#include <sstream>

#include "StepApp.h"  // step::ExePath, step::Log

using render::Matrix;
using render::Vector3;
using render::Vector4;
using sokoban::Int2;
using sokoban::Level;
using sokoban::Occupant;
using sokoban::Terrain;

namespace game {

namespace {

// 색 팔레트 (06단계에서 미리 써 본 색)
const Vector4 kFloorColor(0.75f, 0.75f, 0.72f, 1.0f);
const Vector4 kGoalColor(0.20f, 0.40f, 0.95f, 1.0f);       // 파랑: 목표
const Vector4 kWallColor(0.45f, 0.45f, 0.50f, 1.0f);       // 회색: 벽
const Vector4 kCrateColor(0.80f, 0.55f, 0.25f, 1.0f);      // 나무색: 상자
const Vector4 kCrateOnGoalColor(0.30f, 0.75f, 0.40f, 1.0f);  // 초록: 목표 위의 상자
const Vector4 kPlayerColor(0.90f, 0.20f, 0.20f, 1.0f);     // 빨강: 플레이어

// 큐브 하나(한 변 1, 원점 중심)를 크기·높이만 바꿔 칸 위에 놓는 World 행렬
Matrix CellMatrix(Int2 cell, const Vector3& size, float bottomY) {
    const Vector3 p = CellToWorld(cell);
    return Matrix::CreateScale(size) * Matrix::CreateTranslation(p.x, bottomY + size.y * 0.5f, p.z);
}

}  // namespace

Vector3 CellToWorld(Int2 cell) {
    // TODO(08): 지도 칸 (x, y)를 월드 (x, 0, y)로 바꾼다. 위 헤더의 좌표 약속을 따른다.
    (void)cell;
    return Vector3::Zero;
}

Vector3 LevelCenter(const Level& level) {
    return Vector3((level.width - 1) * 0.5f, 0.0f, (level.height - 1) * 0.5f);
}

std::vector<bool> ComputeInside(const Level& level) {
    std::vector<bool> inside(level.terrain.size(), false);
    // TODO(08): 플레이어 위치에서 시작해 벽이 아닌 칸을 상하좌우로 넓혀 가며(flood fill) inside를 true로 채운다.
    //   힌트: 할 일 목록(std::vector<Int2>)에 플레이어 칸을 넣고, 하나씩 꺼내 이웃 4칸을 검사한다.
    //   힌트: 맵 밖(InBounds가 false), 벽, 이미 true인 칸은 넣지 않는다. 재귀를 쓰면 큰 맵에서 스택이 넘칠 수 있다.
    return inside;
}

bool LoadLevelFile(const std::wstring& relativePath, Level& level) {
    const std::filesystem::path path(step::ExePath(relativePath.c_str()));
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        step::Log("[실패] 레벨 파일을 열 수 없음: %s", path.u8string().c_str());
        return false;
    }
    std::ostringstream text;
    text << file.rdbuf();
    std::string error;
    if (!sokoban::LoadLevelFromText(text.str(), level, error)) {
        step::Log("[실패] 레벨 형식 오류: %s", error.c_str());
        return false;
    }
    return true;
}

void DrawLevel(render::Renderer& renderer, render::MeshHandle cube, const Level& level,
               const std::vector<bool>& inside) {
    // TODO(08): 모든 칸을 돌며 Terrain과 Occupant에 맞는 큐브를 그린다.
    //   힌트: 벽: 1 x 1 x 1, 바닥(y = -0.5)에서 시작. 바닥/목표: 0.96 x 0.1 x 0.96 판, 윗면이 y = -0.5 (inside인 칸만)
    //   힌트: 상자: 0.8 x 0.8 x 0.8 (목표 위면 초록), 플레이어: 0.6 x 0.9 x 0.6. 둘 다 바닥(y = -0.5)에서 시작
    //   힌트: CellMatrix(칸, 크기, 아랫면 높이)가 크기와 위치를 함께 만들어 준다.
    (void)renderer;
    (void)cube;
    (void)level;
    (void)inside;
}

}  // namespace game
