// Camera.h - 이 수업의 첫 '엔진' 클래스
//
// 04단계에서는 View와 Projection 행렬을 OnUpdate 안에서 직접 만들었다.
// 08단계 이후 소코반, 07단계의 CubeViewer가 모두 같은 일을 하게 되므로 여기서 한 번 묶는다.
//   NEED: 05단계에서 View/Projection 계산이 여러 프로그램에 반복될 것이 확실해져서 추가
//
// 이 클래스가 하지 않는 것
//   - 입력 처리 (09단계에서 입력이 카메라의 Orbit 값을 바꿀 뿐, 카메라는 키보드를 모른다)
//   - 그리기, 셰이더, 버퍼 (카메라는 행렬 두 개를 만들 뿐이다)
#pragma once

#include "SimpleMath.h"  // DirectXTK

class Camera {
public:
    using Matrix = DirectX::SimpleMath::Matrix;
    using Vector3 = DirectX::SimpleMath::Vector3;

    // 렌즈: 세로 시야각(라디안), 화면비(가로/세로), 가까운/먼 클립 거리. Projection 행렬을 만든다.
    void SetLens(float fovY, float aspect, float nearZ, float farZ);

    // 눈 위치에서 target을 바라본다. View 행렬을 만든다.
    void LookAt(const Vector3& eye, const Vector3& target, const Vector3& up = Vector3::UnitY);

    // target을 중심으로 distance만큼 떨어진 구면 위에 카메라를 둔다.
    //   yaw   : Y축 둘레 각도(라디안). 0이면 target의 -Z 쪽에서 +Z 방향을 본다.
    //   pitch : 위로 올려다보는 높이 각도(라디안). 0이면 수평, 양수면 위에서 내려다본다.
    void Orbit(const Vector3& target, float distance, float yaw, float pitch);

    const Matrix& View() const { return view_; }
    const Matrix& Projection() const { return projection_; }
    const Vector3& Position() const { return position_; }

private:
    Vector3 position_;
    Matrix view_;        // 기본값은 단위 행렬
    Matrix projection_;  // 기본값은 단위 행렬
};
