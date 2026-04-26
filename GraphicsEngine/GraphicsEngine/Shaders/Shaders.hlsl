High-level shader language
struct VS_INPUT {
    float3 position : POSITION; // Recibe los 12 bytes del Vector3D
    float4 color : COLOR;       // Recibe los 16 bytes (RGBA)
};

struct VS_OUTPUT {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VS_OUTPUT vs_main(VS_INPUT input) {
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    // W debe ser 1.0 para que el rasterizador lo dibuje
    output.position = float4(input.position, 1.0f);
    output.color = input.color;
    
    return output;
}

float4 ps_main(VS_OUTPUT input) : SV_TARGET {
    return input.color;
}