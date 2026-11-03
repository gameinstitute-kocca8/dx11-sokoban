// Camera.cpp
#include "Camera.h"

#include <algorithm>
#include <cmath>

void Camera::SetLens(float fovY, float aspect, float nearZ, float farZ) {
    // TODO(05): 원근 투영 행렬을 만든다.
    //   힌트: Matrix::CreatePerspectiveFieldOfView(fovY, aspect, nearZ, farZ)  (오른손 좌표계)
    //   힌트: nearZ는 0보다 커야 한다. 0을 넣으면 어떻게 되는지 TASK.md의 예측 질문을 보자.
    (void)fovY;
    (void)aspect;
    (void)nearZ;
    (void)farZ;
}

void Camera::LookAt(const Vector3& eye, const Vector3& target, const Vector3& up) {
    // TODO(05): 눈 위치를 기억하고, View 행렬을 만든다.
    //   힌트: Matrix::CreateLookAt(eye, target, up)
    (void)eye;
    (void)target;
    (void)up;
}

void Camera::Orbit(const Vector3& target, float distance, float yaw, float pitch) {
    // TODO(05): 구면 좌표(distance, yaw, pitch)를 눈 위치로 바꾸고 LookAt을 호출한다.
    //   힌트: pitch가 ±90도에 가까우면 시선과 위쪽(+Y)이 평행해져 View 행렬이 망가진다. ±89도 정도로 제한한다.
    //   힌트: 수평 반지름 = distance * cos(pitch), 높이 = distance * sin(pitch)
    //   힌트: yaw = 0일 때 눈이 target의 -Z 쪽에 오도록: x = 반지름 * sin(yaw), z = -반지름 * cos(yaw)
    (void)target;
    (void)distance;
    (void)yaw;
    (void)pitch;
}
