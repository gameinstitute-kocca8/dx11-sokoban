// GridView.h - 소코반 상태(Level)를 3D로 그리는 법
//
// 이 파일은 '상태를 보고 그리기'만 한다. Level을 바꾸지 않는다.
// 칸 하나 = 큐브 하나. 04단계에서 배운 "같은 메시를 World 행렬과 색만 바꿔 여러 번 그리기"가 전부다.
#pragma once

#include <string>
#include <vector>

#include "Game/Sokoban.h"
#include "Render/Renderer.h"

namespace game {

// 지도 좌표 → 월드 좌표.  칸 (x, y) → 월드 (x, 0, y)
//   지도의 x(오른쪽)는 월드 +X, 지도의 y(아래쪽)는 월드 +Z.
//   카메라를 +Z 쪽(지도의 아래쪽)에 두고 -Z를 바라보면 지도 텍스트와 화면의 위/아래/좌/우가 같아진다.
//   (05단계 예측 질문 2번: -Z 쪽에서 보면 +X가 화면 왼쪽이 되므로 카메라를 반대편에 둔다)
render::Vector3 CellToWorld(sokoban::Int2 cell);

// 레벨 한가운데의 월드 좌표 (카메라가 바라볼 곳)
render::Vector3 LevelCenter(const sokoban::Level& level);

// 플레이어가 걸어갈 수 있는 '벽 안쪽' 칸이면 true. 벽 바깥 빈칸에는 바닥을 그리지 않기 위해 쓴다.
std::vector<bool> ComputeInside(const sokoban::Level& level);

// 레벨 파일 읽기 (실행 파일 기준 경로). 실패하면 이유를 로그로 남기고 false.
bool LoadLevelFile(const std::wstring& relativePath, sokoban::Level& level);

// 레벨 전체를 그린다. inside는 ComputeInside의 결과.
void DrawLevel(render::Renderer& renderer, render::MeshHandle cube, const sokoban::Level& level,
               const std::vector<bool>& inside);

}  // namespace game
