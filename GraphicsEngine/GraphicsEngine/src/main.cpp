#include <windows.h>
#include "AppWindow.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {

    AppWindow app;

    if (app.init()) {
        while (app.isRun()) {
            app.broadcast();
            // Sleep(0) permite que Windows procese otros hilos. 
            // Es una red de seguridad mientras desarrollamos.
            Sleep(0);
        }
    }

    return 0;
}