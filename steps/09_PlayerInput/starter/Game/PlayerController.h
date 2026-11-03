// PlayerController.h - 키 입력을 소코반의 '지도 방향'으로 바꾼다
//
// 카메라를 마우스로 돌릴 수 있으므로 "W = 지도의 위쪽"이 아니다.
// "W = 지금 카메라가 바라보는 쪽에 가장 가까운 지도 방향"이다. 대각선은 없으므로 네 방향 중 하나로 맞춘다(스냅).
#pragma once

#include "Engine/Input.h"
#include "Game/Sokoban.h"

namespace game {

// 카메라 yaw(05단계 Orbit의 yaw)를 기준으로, 앞(+1)/뒤(-1)와 오른쪽(+1)/왼쪽(-1) 입력을 지도 방향으로 바꾼다.
//   forward와 right 중 하나만 0이 아니어야 한다.
sokoban::Dir CameraRelativeDir(float cameraYaw, int forward, int right);

// 이번 프레임에 이동 키(W A S D, 방향키)가 '눌린 순간'이면 방향을 out에 넣고 true
bool ReadMoveInput(const engine::Input& input, float cameraYaw, sokoban::Dir& out);

}  // namespace game
