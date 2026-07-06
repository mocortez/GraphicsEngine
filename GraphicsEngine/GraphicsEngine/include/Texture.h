#pragma once
#include <d3d11.h>
#include <string>

class Texture {
public:
    /**
     * @brief Constructor que carga y sube una imagen JPG a la memoria de la GPU.
     * @param full_path Ruta ancha (L"...") absoluta del archivo en el disco.
     */
    Texture(const std::wstring& full_path);
    ~Texture();

    /**
     * @brief Vincula la textura y el sampler al pipeline de DirectX 11.
     * @param slot Slot del Pixel Shader (t0, t1, etc.)
     */
    void bind(unsigned int slot);

    /**
     * @brief Verifica si la textura se cargó correctamente en la tarjeta de video.
     */
    bool isValid() const { return m_shader_res_view != nullptr; }

private:
    ID3D11Texture2D* m_texture = nullptr;
    ID3D11ShaderResourceView* m_shader_res_view = nullptr;
    ID3D11SamplerState* m_sampler_state = nullptr;
};
