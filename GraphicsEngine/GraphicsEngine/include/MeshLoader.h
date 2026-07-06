#pragma once
#include "Prerequisites.h"
#include <vector>
#include <string>
#include "Point3D.h"

class MeshLoader {
public:
    static bool LoadOBJ(const std::string& filepath, std::vector<Point3D>& out_vertices);
};