// PlayerController.cpp
#include "Game/PlayerController.h"

#include <cmath>

using engine::Key;
using sokoban::Dir;

namespace game {

Dir CameraRelativeDir(float cameraYaw, int forward, int right) {
    const float fx = -std::sin(cameraYaw);
    const float fz = std::cos(cameraYaw);
    const float rx = -std::cos(cameraYaw);
    const float rz = -std::sin(cameraYaw);
    const float mx = fx * forward + rx * right;
    const float mz = fz * forward + rz * right;
    if (std::fabs(mx) >= std::fabs(mz)) return mx > 0.0f ? Dir::Right : Dir::Left;
    return mz > 0.0f ? Dir::Down : Dir::Up;
}

bool ReadMoveInput(const engine::Input& input, float cameraYaw, Dir& out) {
    if (input.WasPressed(Key::W) || input.WasPressed(Key::Up)) {
        out = CameraRelativeDir(cameraYaw, 1, 0);
        return true;
    }
    if (input.WasPressed(Key::S) || input.WasPressed(Key::Down)) {
        out = CameraRelativeDir(cameraYaw, -1, 0);
        return true;
    }
    if (input.WasPressed(Key::D) || input.WasPressed(Key::Right)) {
        out = CameraRelativeDir(cameraYaw, 0, 1);
        return true;
    }
    if (input.WasPressed(Key::A) || input.WasPressed(Key::Left)) {
        out = CameraRelativeDir(cameraYaw, 0, -1);
        return true;
    }
    return false;
}

}  // namespace game
