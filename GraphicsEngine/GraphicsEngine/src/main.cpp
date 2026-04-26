#include <windows.h>
#include "AppWindow.h"
#include <iostream>

int main() {
    AppWindow app;
    if (app.init()) { // SOLO si inicia bien entramos al bucle
        while (app.isRun()) {
            app.broadcast();
        }
    }
    else {
        // Si falla, imprimimos y salimos de inmediato
        std::cout << "Error: No se pudo inicializar la AppWindow. Revisa los logs de los Shaders.\n";
        system("pause"); // Esto te deja ver el error antes de que se cierre
    }
    return 0;
}