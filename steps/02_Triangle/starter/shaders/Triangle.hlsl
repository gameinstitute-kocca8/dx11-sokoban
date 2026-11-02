// Triangle.hlsl
// C++의 Vertex 구조체, Input Layout, 이 VSInput은 같은 배치를 서로 다른 세 곳에서 적은 것이다.

struct VSInput {
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VSOutput {
    float4 position : SV_POSITION;  // 래스터라이저가 쓰는 클립 공간 위치
    float4 color : COLOR;           // 삼각형 내부에서 보간되어 PS로 전달된다
};

VSOutput VSMain(VSInput input) {
    VSOutput output = (VSOutput)0;
    // TODO(02): 입력 위치를 클립 공간 위치(w = 1)로 넘기고, 색을 그대로 전달한다.
    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET {
    return input.color;
}
