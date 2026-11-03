// Camera.cpp
#include "Camera.h"

#include <algorithm>
#include <cmath>

void Camera::SetLens(float fovY, float aspect, float nearZ, float farZ) {
    projection_ = Matrix::CreatePerspectiveFieldOfView(fovY, aspect, nearZ, farZ);
}

void Camera::LookAt(const Vector3& eye, const Vector3& target, const Vector3& up) {
    position_ = eye;
    view_ = Matrix::CreateLookAt(eye, target, up);
}

void Camera::Orbit(const Vector3& target, float distance, float yaw, float pitch) {
    const float limit = DirectX::XMConvertToRadians(89.0f);
    pitch = std::clamp(pitch, -limit, limit);

    const float horizontal = distance * std::cos(pitch);
    const Vector3 offset(horizontal * std::sin(yaw), distance * std::sin(pitch), -horizontal * std::cos(yaw));
    LookAt(target + offset, target);
}
