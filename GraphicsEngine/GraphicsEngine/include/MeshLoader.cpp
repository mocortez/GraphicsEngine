#include "MeshLoader.h"
#include "Prerequisites.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::vector<Point3D> MeshLoader::loadOBJ(const std::string& path) {
	std::vector<Point3D> points;
	std::ifstream file(path);

	if (!file.is_open()) {
		std::cerr << "Error al abrir el archivo: " << path << std::endl;
		return points;
	}

	std::string line;
	while (std::getline(file, line)) {
		if (line.substr(0, 2) == "v ") {
			std::istringstream ss(line.substr(2));
			float x, y, z, r, g, b;

			// Intentamos leer las coordenadas y el color (si está presente)
			if (ss >> x >> y >> z) {
				if (! (ss >> r >> g >> b)) {
					r = g = b = 1.0f; // Color blanco por defecto
				}

				// Creamos un punto con la posición y el color
				Vector3D pos(x, y, z);
				// Añadimos el Point3D al vector
				points.emplace_back(pos, r, g, b);
			}
		}
	}

	file.close();
	std::cout << "Archivo OBJ cargado: " << points.size() << " puntos encontrados." << std::endl;
	return points;
}