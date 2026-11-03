// PlayerController.cpp
#include "Game/PlayerController.h"

#include <cmath>

using engine::Key;
using sokoban::Dir;

namespace game {

Dir CameraRelativeDir(float cameraYaw, int forward, int right) {
    // TODO(09): 카메라 기준 입력을 월드 방향 벡터로 바꾸고, 가장 가까운 지도 방향으로 맞춘다.
    //   힌트: Orbit에서 눈 = target + (sin(yaw), *, -cos(yaw)) * r 이었다. 바라보는 방향은 그 반대 (-sin(yaw), 0, cos(yaw))
    //   힌트: 오른쪽 = (-cos(yaw), 0, -sin(yaw)). yaw = π를 넣어 앞이 (0,0,-1), 오른쪽이 (1,0,0)인지 확인하자
    //   힌트: 월드 +X = 지도 오른쪽, 월드 +Z = 지도 아래쪽 (08단계 CellToWorld). |x|와 |z| 중 큰 쪽이 이긴다
    (void)cameraYaw;
    (void)forward;
    (void)right;
    return Dir::Up;
}

bool ReadMoveInput(const engine::Input& input, float cameraYaw, Dir& out) {
    // TODO(09): W/↑ 앞, S/↓ 뒤, A/← 왼쪽, D/→ 오른쪽. '눌린 순간'(WasPressed)만 인정한다.
    (void)input;
    (void)cameraYaw;
    (void)out;
    return false;
}

}  // namespace game
