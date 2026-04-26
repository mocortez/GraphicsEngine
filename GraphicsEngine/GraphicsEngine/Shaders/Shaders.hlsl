High-level shader language
// Estructura que entra desde el Vertex Buffer (Point3D)
struct VS_INPUT {
    float4 position : POSITION;
    float3 color : COLOR;
};

// Estructura que sale del Vertex Shader hacia el Pixel Shader
struct VS_OUTPUT {
    float4 position : SV_POSITION;
    float3 color : COLOR;
};

// --- VERTEX SHADER ---
VS_OUTPUT vs_main(VS_INPUT input) {
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    // Pasamos la posición directamente (en el futuro aquí aplicaremos la matriz MVP)
    output.position = input.position;
    
    // Pasamos el color al siguiente paso
    output.color = input.color;
    
    return output;
}

// --- PIXEL SHADER ---
float4 ps_main(VS_OUTPUT input) : SV_TARGET {
    // Dibujamos el píxel con el color que viene del vértice
    return float4(input.color, 1.0f);
}