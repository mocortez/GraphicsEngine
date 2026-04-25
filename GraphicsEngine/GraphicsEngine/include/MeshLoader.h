#pragma once
#include "Prerequisites.h"
#include <vector>
#include <string>
#include "Point3D.h"

class MeshLoader
{
public:
	static std::vector<Point3D> loadOBJ(const std::string& path);
};