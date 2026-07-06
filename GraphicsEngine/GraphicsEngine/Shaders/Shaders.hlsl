// =========================================================================
// 1. RECURSOS GLOBALES (Constant Buffers, Texturas y Samplers)
// =========================================================================
cbuffer AppWindowCBData : register(b0)
{
    matrix m_world;
    matrix m_view;
    matrix m_proj;
    float m_has_texture; 
    float3 padding_cb;   
};

Texture2D my_texture : register(t0);
SamplerState my_sampler : register(s0);

// =========================================================================
// 2. ESTRUCTURAS DE DATOS
// =========================================================================
struct VS_INPUT
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

// =========================================================================
// 3. VERTEX SHADER
// =========================================================================
VS_OUTPUT vs_main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    float4 w_pos = mul(m_world, float4(input.pos, 1.0f));
    float4 v_pos = mul(m_view, w_pos);
    output.pos   = mul(m_proj, v_pos);
    
    output.texcoord = input.texcoord;
    output.normal = mul((float3x3)m_world, input.normal);
    
    return output;
}

// =========================================================================
// 4. PIXEL SHADER
// =========================================================================
float4 ps_main(VS_OUTPUT input) : SV_TARGET
{
    if (m_has_texture < 0.5f)
    {
        float intensity = saturate(dot(normalize(input.normal), normalize(float3(0.5f, 1.0f, -0.5f))));
        float3 ambient = float3(0.2f, 0.2f, 0.2f);
        float3 diffuse = float3(0.5f, 0.5f, 0.5f) * intensity;
        return float4(ambient + diffuse, 1.0f);
    }
    
    float4 tex_color = my_texture.Sample(my_sampler, input.texcoord);
    
    // CORRECCIÓN MATEMÁTICA: Multiplicador 1.0f plano para mantener el albedo real sin quemar la exposición
    float3 final_color = tex_color.rgb * 1.0f;
    
    return float4(final_color, 1.0f);
}