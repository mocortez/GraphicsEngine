#include <iostream>
#include "MeshLoader.h"

int main() {
    // 1. Mensaje de bienvenida para saber que la consola funciona
    std::cout << "--- Graphics Engine: Iniciando cargador de fotogrametria ---" << std::endl;

    // 2. Definimos la ruta de tu modelo (pon el nombre de tu .obj aunque no exista aun)
    std::string path = "vuelo_dji.obj";

    // 3. LLAMAMOS a la funcion (Esto conecta el .h con el .cpp)
    std::vector<Point3D> puntos = MeshLoader::loadOBJ(path);

    // 4. Verificamos el resultado
    if (puntos.empty()) {
        std::cout << "Advertencia: No se cargaron puntos. Verifica que el archivo exista." << std::endl;
    }
    else {
        std::cout << "Exito: Se han procesado " << puntos.size() << " puntos." << std::endl;
    }

    // 5. Evita que la consola se cierre sola al terminar
    std::cout << "\nPresiona Enter para salir...";
    std::cin.get();

    return 0;
}

