// Picking.cpp
#include "Game/Picking.h"

#include <cmath>

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;

namespace game {

bool PickCell(const engine::Camera& camera, float mouseX, float mouseY, float screenWidth, float screenHeight,
              sokoban::Int2& cell) {
    // 1) 픽셀 → 클립 공간. 화면은 y가 아래로, 클립 공간은 y가 위로 커진다.
    const float x = mouseX / screenWidth * 2.0f - 1.0f;
    const float y = 1.0f - mouseY / screenHeight * 2.0f;

    // 2) 클립 공간 → 월드. Transform은 w로 나누기까지 해 준다 (XMVector3TransformCoord).
    const Matrix inverse = (camera.View() * camera.Projection()).Invert();
    const Vector3 nearPoint = Vector3::Transform(Vector3(x, y, 0.0f), inverse);
    const Vector3 farPoint = Vector3::Transform(Vector3(x, y, 1.0f), inverse);
    const Vector3 direction = farPoint - nearPoint;

    // 3) 바닥 판 윗면(y = -0.5)과 만나는 점. 광선이 수평이거나 위를 향하면 만나지 않는다.
    const float floorY = -0.5f;
    if (std::fabs(direction.y) < 1e-6f) return false;
    const float t = (floorY - nearPoint.y) / direction.y;
    if (t < 0.0f) return false;
    const Vector3 hit = nearPoint + direction * t;

    // 08단계: 칸 (x, y) → 월드 (x, 0, y). 칸 중심이 정수이므로 반올림하면 그 칸이다.
    cell = sokoban::Int2{static_cast<int>(std::lround(hit.x)), static_cast<int>(std::lround(hit.z))};
    return true;
}

}  // namespace game
