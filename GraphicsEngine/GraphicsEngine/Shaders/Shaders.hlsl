// Shaders.hlsl - Archivo Limpio
struct VS_INPUT {
    float3 position : POSITION;
    float4 color    : COLOR;
};

struct VS_OUTPUT {
    float4 position : SV_POSITION;
    float4 color    : COLOR;
};

VS_OUTPUT vs_main(VS_INPUT input) {
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.position = float4(input.position, 1.0f);
    output.color = input.color;
    return output;
}

float4 ps_main(VS_OUTPUT input) : SV_TARGET {
    return input.color;
}