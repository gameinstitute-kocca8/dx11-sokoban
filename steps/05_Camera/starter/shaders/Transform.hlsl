// Transform.hlsl

// C++의 PerObjectConstants와 같은 배치. register(b0) = VSSetConstantBuffers의 0번 슬롯.
cbuffer PerObject : register(b0) {
    float4x4 WorldViewProj;  // C++에서 Transpose()해서 넣는다 (row-major → HLSL 기본 column-major)
    float4 Tint;             // 같은 메시를 다른 색으로 그리기 위한 곱셈 색
};

struct VSInput {
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VSOutput {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VSOutput VSMain(VSInput input) {
    VSOutput output;
    output.position = float4(input.position, 1.0f);  // 변환 전: 모델 좌표를 그대로 클립 좌표로 사용
    output.color = input.color;
    output.position = mul(float4(input.position, 1.0f), WorldViewProj);
    output.color = input.color * Tint;
    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET {
    return input.color;
}
