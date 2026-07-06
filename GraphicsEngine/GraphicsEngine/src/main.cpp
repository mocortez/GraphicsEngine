#include <windows.h>
#include "AppWindow.h"
#include <iostream>
#include <objbase.h> // Necesario para CoInitializeEx y CoUninitialize

int main() {
    // 1. INICIALIZAR SUBSISTEMA COM PARA WIC (Multithreaded o SingleThreaded)
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr)) {
        std::cerr << "FALLO CRITICO: No se pudo inicializar el subsistema COM de Windows." << std::endl;
        system("pause");
        return -1;
    }

    AppWindow app;

    std::cout << "Iniciando Engine..." << std::endl;

    if (app.init()) {
        std::cout << "DirectX 11 Inicializado correctamente en NVIDIA RTX 3060" << std::endl;
        while (app.isRun()) {
            app.broadcast();
        }
    }
    else {
        std::cerr << "FALLO CRITICO: Revisa los errores del Shader arriba." << std::endl;
        system("pause");
    }

    // 2. LIBERAR SUBSISTEMA COM AL CERRAR EL MOTOR
    CoUninitialize();

    return 0;
}