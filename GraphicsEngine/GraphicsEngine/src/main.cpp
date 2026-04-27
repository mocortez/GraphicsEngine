#include <windows.h>
#include "AppWindow.h"
#include <iostream>

int main() {
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
        // El system("pause") es vital para que la consola no se cierre 
        // antes de que puedas leer el error.
        system("pause");
    }

    return 0;
}