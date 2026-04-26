High-level shader language
/**
 * Shaders.hlsl
 * Pipeline de renderizado para visualización de fotogrametría.
 */

// Estructura de entrada (Desde el Vertex Buffer / Point3D)
// Debe coincidir exactamente con el Input Layout definido en C++
struct VS_INPUT {
    float3 position : POSITION; // 12 bytes del Vector3D
    float4 color    : COLOR;    // 16 bytes (RGBA), empieza en el byte 16 del struct C++
};

// Estructura de salida (Hacia el Pixel Shader)
struct VS_OUTPUT {
    float4 position : SV_POSITION; // SV_POSITION es un semántico especial del sistema
    float4 color    : COLOR;
};

// --- VERTEX SHADER ---
VS_OUTPUT vs_main(VS_INPUT input) {
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    /**
     * IMPORTANTE:
     * Por ahora pasamos la posición directamente a la pantalla.
     * El componente 'w' debe ser 1.0f para que el punto sea visible.
     */
    output.position = float4(input.position, 1.0f);
    
    // Pasamos el color al Pixel Shader para que lo interpole
    output.color = input.color;
    
    return output;
}

// --- PIXEL SHADER ---
float4 ps_main(VS_OUTPUT input) : SV_TARGET {
    /**
     * El color que llega aquí ya ha sido interpolado por el rasterizador
     * entre los vértices del triángulo.
     */
    return input.color;
}