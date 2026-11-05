// Picking.h - 마우스로 가리킨 화면 점이 맵의 어느 칸인지 (제공 코드)
//
// 이 단계의 과제는 A*를 게임에 연결하는 것이다. 피킹은 읽고 이해만 하면 된다.
//   1) 마우스 픽셀 → 클립 공간 (-1 ~ 1)
//   2) (View × Projection)의 역행렬로 가까운 면(z = 0)과 먼 면(z = 1)의 월드 점을 구한다 → 광선
//   3) 광선이 바닥 높이(y = -0.5) 평면과 만나는 점 → 가장 가까운 칸 (08단계 CellToWorld의 반대)
#pragma once

#include "Engine/Camera.h"
#include "Game/Sokoban.h"

namespace game {

// 성공하면 true와 칸 좌표. 광선이 바닥과 만나지 않으면(하늘을 가리킴) false.
//   맵 밖 칸일 수도 있다. 범위 검사는 부르는 쪽이 한다.
bool PickCell(const engine::Camera& camera, float mouseX, float mouseY, float screenWidth, float screenHeight,
              sokoban::Int2& cell);

}  // namespace game
