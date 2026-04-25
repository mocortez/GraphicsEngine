#pragma once
#include "Prerequisites.h"
#include "Vector3D.h"

struct Point3D
{
	//Un punto en el espacio con un color asociado
	Vector3D position;
	float r, g, b;  //Color del punto (rojo, verde, azul)

	//Constructor default
	Point3D(Vector3D pos, float red, float green, float blue)
		: position(pos), r(red), g(green), b(blue) {
	}

};