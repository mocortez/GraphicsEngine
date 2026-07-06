#include "MeshLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include "Vector3D.h"
#include "Vector2D.h"
#include "Point3D.h"

bool MeshLoader::LoadOBJ(const std::string& filepath, std::vector<Point3D>& out_vertices) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cout << "MeshLoader Error: No se pudo abrir el archivo en " << filepath << "\n";
        return false;
    }

    std::vector<Vector3D> temp_positions;
    std::vector<Vector2D> temp_uvs;
    std::vector<Vector3D> temp_normals;
    out_vertices.clear();

    std::string line;
    while (std::getline(file, line)) {
        std::replace(line.begin(), line.end(), '\r', ' ');
        std::stringstream ss(line);
        std::string type;
        ss >> type;

        // 1. Parseo de posiciones usando .x, .y, .z de tu Vector3D
        if (type == "v") {
            float x, y, z;
            ss >> x >> y >> z;
            temp_positions.push_back(Vector3D(x, y, z));
        }
        // 2. Coordenadas de textura (UVs) con inversión en V para el espacio de DirectX
        else if (type == "vt") {
            float u, v;
            ss >> u >> v;
            temp_uvs.push_back(Vector2D(u, 1.0f - v));
        }
        // 3. Normales usando .x, .y, .z de tu Vector3D
        else if (type == "vn") {
            float x, y, z;
            ss >> x >> y >> z;
            temp_normals.push_back(Vector3D(x, y, z));
        }
        // 4. Procesamiento e indexación dinámica de caras
        else if (type == "f") {
            std::string token;
            std::vector<Point3D> face_vertices;

            while (ss >> token) {
                std::stringstream tokenss(token);
                std::string part;

                int vp_idx = -1;
                int vt_idx = -1;
                int vn_idx = -1;

                if (std::getline(tokenss, part, '/')) {
                    if (!part.empty()) vp_idx = std::stoi(part) - 1;
                }
                if (std::getline(tokenss, part, '/')) {
                    if (!part.empty()) vt_idx = std::stoi(part) - 1;
                }
                if (std::getline(tokenss, part, '/')) {
                    if (!part.empty()) vn_idx = std::stoi(part) - 1;
                }

                // Extracción de UVs usando los getters de tu Vector2D
                MeshUV uv = (vt_idx >= 0 && vt_idx < (int)temp_uvs.size())
                    ? MeshUV(temp_uvs[vt_idx].GetX(), temp_uvs[vt_idx].GetY())
                    : MeshUV(0.0f, 0.0f);

                Vector3D normal = (vn_idx >= 0 && vn_idx < (int)temp_normals.size())
                    ? temp_normals[vn_idx]
                    : Vector3D(0.0f, 1.0f, 0.0f);

                if (vp_idx >= 0 && vp_idx < (int)temp_positions.size()) {
                    // Sincronizado: Se crea el Point3D contiguo de 32 bytes sin paddings intermedios
                    face_vertices.push_back(Point3D(temp_positions[vp_idx], normal, uv));
                }
            }

            // Triangulación por abanico en caso de que la malla conserve algún Quad
            if (face_vertices.size() >= 3) {
                for (size_t i = 1; i < face_vertices.size() - 1; ++i) {
                    out_vertices.push_back(face_vertices[0]);
                    out_vertices.push_back(face_vertices[i]);
                    out_vertices.push_back(face_vertices[i + 1]);
                }
            }
        }
    }
    file.close();

    // =========================================================================
    // 5. AUTO-CENTRADO GEOMÉTRICO Y DIAGNÓSTICO REAL
    // =========================================================================
    if (!out_vertices.empty()) {
        Vector3D min_bound = out_vertices[0].position;
        Vector3D max_bound = out_vertices[0].position;

        // Búsqueda de límites espaciales (Bounding Box)
        for (const auto& vertex : out_vertices) {
            if (vertex.position.x < min_bound.x) min_bound.x = vertex.position.x;
            if (vertex.position.y < min_bound.y) min_bound.y = vertex.position.y;
            if (vertex.position.z < min_bound.z) min_bound.z = vertex.position.z;

            if (vertex.position.x > max_bound.x) max_bound.x = vertex.position.x;
            if (vertex.position.y > max_bound.y) max_bound.y = vertex.position.y;
            if (vertex.position.z > max_bound.z) max_bound.z = vertex.position.z;
        }

        // Cálculo del centroide real del objeto de fotogrametría
        Vector3D center(
            (min_bound.x + max_bound.x) * 0.5f,
            (min_bound.y + max_bound.y) * 0.5f,
            (min_bound.z + max_bound.z) * 0.5f
        );

        // Traslación offset: Restamos el centro para fijar el pivote del monumento en (0,0,0)
        for (auto& vertex : out_vertices) {
            vertex.position.x -= center.x;
            vertex.position.y -= center.y;
            vertex.position.z -= center.z;
        }

        // Cálculo dinámico real de caras basadas exclusivamente en los datos cargados
        size_t total_faces = out_vertices.size() / 3;

        std::cout << "MeshLoader: Modelo de " << total_faces << " caras centrado. Desfase de origen anulado: ("
            << center.x << ", " << center.y << ", " << center.z << ")\n";
    }

    std::cout << "MeshLoader: " << out_vertices.size() << " vertices cargados con exito.\n";
    return true;
}