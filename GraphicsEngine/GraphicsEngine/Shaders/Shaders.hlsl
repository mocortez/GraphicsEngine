// Shaders.hlsl

// Nuevo: El Constant Buffer (register b0 indica el primer slot)
cbuffer constant : register(b0)
{
    row_major float4x4 m_world; 
};

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
    
    // Multiplicamos la posición del vértice por la matriz de mundo
    // mul() es la función de HLSL para multiplicación de matrices
    output.position = mul(float4(input.position, 1.0f), m_world);
    
    output.color = input.color;
    return output;
}

float4 ps_main(VS_OUTPUT input) : SV_TARGET {
    return input.color;
}