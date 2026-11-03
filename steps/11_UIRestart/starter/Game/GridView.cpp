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
    return Vector3(static_cast<float>(cell.x), 0.0f, static_cast<float>(cell.y));
}

Vector3 LevelCenter(const Level& level) {
    return Vector3((level.width - 1) * 0.5f, 0.0f, (level.height - 1) * 0.5f);
}

std::vector<bool> ComputeInside(const Level& level) {
    std::vector<bool> inside(level.terrain.size(), false);
    std::vector<Int2> todo{level.player};
    inside[level.Index(level.player)] = true;
    while (!todo.empty()) {
        const Int2 cell = todo.back();
        todo.pop_back();
        const Int2 neighbors[] = {{cell.x + 1, cell.y}, {cell.x - 1, cell.y}, {cell.x, cell.y + 1}, {cell.x, cell.y - 1}};
        for (const Int2& n : neighbors) {
            if (!level.InBounds(n) || level.TerrainAt(n) == Terrain::Wall || inside[level.Index(n)]) continue;
            inside[level.Index(n)] = true;
            todo.push_back(n);
        }
    }
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
    for (int y = 0; y < level.height; ++y) {
        for (int x = 0; x < level.width; ++x) {
            const Int2 cell{x, y};
            const Terrain terrain = level.TerrainAt(cell);
            if (terrain == Terrain::Wall) {
                renderer.DrawMesh(cube, CellMatrix(cell, Vector3(1.0f, 1.0f, 1.0f), -0.5f), kWallColor);
                continue;
            }
            if (!inside[level.Index(cell)]) continue;

            const Vector4& tile = terrain == Terrain::Goal ? kGoalColor : kFloorColor;
            renderer.DrawMesh(cube, CellMatrix(cell, Vector3(0.96f, 0.1f, 0.96f), -0.6f), tile);

            switch (level.OccupantAt(cell)) {
                case Occupant::Crate:
                    renderer.DrawMesh(cube, CellMatrix(cell, Vector3(0.8f, 0.8f, 0.8f), -0.5f),
                                      terrain == Terrain::Goal ? kCrateOnGoalColor : kCrateColor);
                    break;
                case Occupant::Player:
                    renderer.DrawMesh(cube, CellMatrix(cell, Vector3(0.6f, 0.9f, 0.6f), -0.5f), kPlayerColor);
                    break;
                case Occupant::Empty:
                    break;
            }
        }
    }
}

}  // namespace game
