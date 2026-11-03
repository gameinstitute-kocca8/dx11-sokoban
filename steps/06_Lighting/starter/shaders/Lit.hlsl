// Lit.hlsl - 방향광 하나 + 주변광, Lambert 확산 조명

// C++의 PerObjectConstants와 같은 배치 (160바이트)
cbuffer PerObject : register(b0) {
    float4x4 WorldViewProj;
    float4x4 World;           // 법선을 월드 방향으로 돌리는 데 쓴다
    float4 Color;             // 재질 색
    float4 LightDirection;    // xyz: 빛이 나아가는 방향(단위 벡터), w: 주변광 세기
};

struct VSInput {
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct VSOutput {
    float4 position : SV_POSITION;
    float3 normal : NORMAL;   // 월드 공간 법선. 삼각형 안에서 보간된다
};

VSOutput VSMain(VSInput input) {
    VSOutput output;
    output.position = mul(float4(input.position, 1.0f), WorldViewProj);
    output.normal = float3(0.0f, 1.0f, 0.0f);
    // TODO(06): 법선을 World 행렬로 돌린다. 위치가 아니므로 이동(4행)은 빼고 3x3만 쓴다.
    //   힌트: mul(normal, (float3x3)World). 크기 변환(Scale)이 섞이면 길이가 바뀌므로 PS에서 다시 정규화한다.
    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET {
    float3 lit = Color.rgb;
    // TODO(06): Lambert 조명: 밝기 = 주변광 + (1 - 주변광) * max(0, dot(법선, 빛을 향하는 방향))
    //   힌트: 빛을 향하는 방향 = -LightDirection.xyz. 보간된 법선은 길이가 1이 아니므로 normalize한다.
    //   힌트: saturate(x)는 x를 0~1로 자른다. max(0, x) 대신 써도 된다.
    return float4(lit, Color.a);
}
