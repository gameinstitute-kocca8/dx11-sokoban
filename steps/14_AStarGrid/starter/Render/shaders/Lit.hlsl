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
    output.normal = mul(input.normal, (float3x3)World);
    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET {
    float3 n = normalize(input.normal);
    float diffuse = saturate(dot(n, -LightDirection.xyz));
    float ambient = LightDirection.w;
    float3 lit = Color.rgb * (ambient + (1.0f - ambient) * diffuse);
    return float4(lit, Color.a);
}
