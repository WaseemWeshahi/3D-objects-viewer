#pragma once
#include <glm/glm.hpp>
#include <string>
#include"MeshModel.h"

using namespace std;

class PrimMeshModel : public MeshModel
{
public:
	PrimMeshModel();
	~PrimMeshModel();
	vector<glm::vec3>* Draw(vector<Light*> lights, float *colorBuffer, float *zBuffer, int width, int height);
	bool get_if_prim();


};