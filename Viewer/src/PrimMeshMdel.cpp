#include "PrimMeshModel.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ImguiMenus.h>
#include<Light.h>
#define FACE_ELEMENTS 3


using namespace std;

PrimMeshModel::PrimMeshModel() :MeshModel::MeshModel(1)
{
	there_is_normals = 1;

	 float vertex_data[36*3] = {
		-1.0f,-1.0f,-1.0f, // triangle 1 : begin
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, // triangle 1 : end
		
		1.0f, 1.0f,-1.0f,// triangle 2 : begin
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f, // triangle 2 : end
		
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,//triangle 3
		1.0f,-1.0f,-1.0f,
		
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,//triangle 4
		-1.0f,-1.0f,-1.0f,

		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,//triangle 5
		-1.0f, 1.0f,-1.0f,
		
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,//triangle 6
		-1.0f,-1.0f,-1.0f,
		
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,//triangle 7
		1.0f,-1.0f, 1.0f,
		
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,//triangle 8
		1.0f, 1.0f,-1.0f,
		
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,//triangle 9
		1.0f,-1.0f, 1.0f,
		
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,//triangle 10
		-1.0f, 1.0f,-1.0f,
		
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,//triangle 11
		-1.0f, 1.0f, 1.0f,
		
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,//triangle 12
		1.0f,-1.0f, 1.0f
	};


	 for (int i = 0; i < 36 * 3; i+=3)
	 {
		 glm::vec3 temp;
		 temp.x = 0.5*vertex_data[i];
		 temp.y = 0.5*vertex_data[i+1];
		 temp.z = 0.5*vertex_data[i+2];

		 triangles.push_back(temp);

	 }

	for (int i = 0; i < 12; i++)
	{
		nTriangles.push_back(triangles[i*3+1] - triangles[i*3]);
		nTriangles.push_back(triangles[i*3 + 2] - triangles[i*3 + 1]);
		nTriangles.push_back(triangles[i*3] - triangles[i*3 + 2]);
	}
	
	vertexPositions = triangles;
	normals = nTriangles;






}

PrimMeshModel::~PrimMeshModel()
{
}

bool PrimMeshModel::get_if_prim()
{
	return 1;
}
vector<glm::vec3>* PrimMeshModel::Draw(vector<Light*> lights, float *colorBuffer, float *zBuffer, int width, int height)
{
	triangles.clear();
	nTriangles.clear();
	glm::mat4 scales(1.0), rotation_y(1.0), rotation_x(1.0), rotation_z(1.0), translate(1.0);
	glm::vec4 temp;
	glm::vec3 tVec, nVec;

	//setting the scale matrix
	scales[0][0] = scale.x;
	scales[1][1] = scale.y;
	scales[2][2] = scale.z;

	//setting the rotation matrix
	{
		rotation_y[0][0] = cos(angle.y*PI / 180);
		rotation_y[0][2] = sin(angle.y*PI / 180);
		rotation_y[1][1] = 1;
		rotation_y[2][0] = -sin(angle.y*PI / 180);
		rotation_y[2][2] = cos(angle.y*PI / 180);
		rotation_y[3][3] = 1;
		rotation_y = glm::transpose(rotation_y);

		rotation_z[0][0] = cos(angle.z*PI / 180);
		rotation_z[0][1] = -sin(angle.z*PI / 180);
		rotation_z[1][0] = sin(angle.z*PI / 180);
		rotation_z[1][1] = cos(angle.z*PI / 180);
		rotation_z[2][2] = 1;
		rotation_z[3][3] = 1;
		rotation_z = glm::transpose(rotation_z);

		rotation_x[0][0] = 1;
		rotation_x[1][2] = -sin(angle.x*PI / 180);
		rotation_x[2][1] = sin(angle.x*PI / 180);
		rotation_x[1][1] = cos(angle.x*PI / 180);
		rotation_x[2][2] = cos(angle.x*PI / 180);
		rotation_x[3][3] = 1;
		rotation_x = glm::transpose(rotation_x);


	}

	//setting the translation matrix
	{

		translate[0][3] = tran.x;
		translate[1][3] = tran.y;
		translate[2][3] = tran.z;
		translate = glm::transpose(translate);
	}


	objectTransform = translate * rotation_x*rotation_y*rotation_z*scales;

	glm::mat4 overall = worldTransform * objectTransform;
	normalTransform = glm::inverse(overall);

	for (int i = 0; i < 6 * FACE_ELEMENTS; i++) {
		tVec = vertexPositions[i];
		nVec = normals[i];

		temp = glm::vec4(tVec, 1.0);

		tVec = overall * temp;
		triangles.push_back(tVec);

		temp = glm::vec4(nVec, 0); //might change to 0

		nVec = temp * normalTransform;
		nVec = glm::normalize(nVec);
		nTriangles.push_back(nVec);


	}


	vector<glm::vec3> color;


	if (gouraud && lights.size() != 0) {

		for (int i = 0; i < triangles.size(); i++)
		{
			glm::vec3 temp;
			Light* src = lights[0];
			temp = calc_color(triangles[i], src, nTriangles[i], model_colors);

			for (int j = 1; j < lights.size(); j++) {
				src = lights[j];
				temp += glm::vec3(calc_color(triangles[i], src, nTriangles[i], model_colors));
			}
			//renderPixel((int)(triangles[i].x), (int)(triangles[i].y), triangles[i].z, temp, colorBuffer, zBuffer, width, height);
			//temp.x = colorBuffer[(int)(3*((int)(triangles[i].x)+ (int)(triangles[i].y) * width)+0)];
			//temp.y = colorBuffer[(int)(3 * ((int)(triangles[i].x) + (int)(triangles[i].y) * width) + 1)];
			//temp.z = colorBuffer[(int)(3 * ((int)(triangles[i].x) + (int)(triangles[i].y) * width) + 2)];
			if (!uniform)
			{
				if (i % 3 == 0)
				{
					temp = temp * (float)(cos((triangles[i].x / width) - 1) + sin((triangles[i].y / height) - 1) + cos(triangles[i].x));
				}
				if (i % 3 == 1)
				{
					temp = temp * (float)(cos((triangles[i].x / width) - 1) + sin(triangles[i].z));
				}
				if (i % 3 == 2)
				{
					temp = temp * (float)(cos((triangles[i].x / width) - 1));
				}

			}
			color.push_back(temp);

		}
	}
	else {
		if (flat)
		{
			vector<glm::vec3> temp_norm;
			for (int i = 0; i < triangles.size(); i += 3)
			{
				glm::vec3 v1 = triangles[i];
				glm::vec3 v2 = triangles[i + 1];
				glm::vec3 v3 = triangles[i + 2];



				glm::vec3 n1 = nTriangles[i];
				glm::vec3 n2 = nTriangles[i + 1];
				glm::vec3 n3 = nTriangles[i + 2];

				glm::vec3 face_normal;
				glm::vec3 center;

				center.x = (v1.x + v2.x + v3.x) / 3.0;
				center.y = (v1.y + v2.y + v3.y) / 3.0;
				center.z = (v1.z + v2.z + v3.z) / 3.0;

				face_normal.x = (n1.x + n2.x + n3.x) / 3.0;
				face_normal.y = (n1.y + n2.y + n3.y) / 3.0;
				face_normal.z = (n1.z + n2.z + n3.z) / 3.0;

				face_normal = glm::normalize(face_normal);
				temp_norm.push_back(face_normal);
				temp_norm.push_back(face_normal);
				temp_norm.push_back(face_normal);

			}
			nTriangles.clear();
			nTriangles = temp_norm;
		}
	float colors[36 * 3] = {0.583f, 0.771f, 0.014f,
		0.583f, 0.771f, 0.014f,
		0.583f, 0.771f, 0.014f,
		0.583f, 0.771f, 0.014f,
		0.583f, 0.771f, 0.014f,
		0.583f, 0.771f, 0.014f,
		0.583f, 0.771f, 0.014f,
		0.583f, 0.771f, 0.014f,
		0.359f, 0.583f, 0.152f,
		0.483f, 0.596f, 0.789f,
		0.559f, 0.861f, 0.639f,
		0.195f, 0.548f, 0.859f,
		0.014f, 0.184f, 0.576f,
		0.771f, 0.328f, 0.970f,
		0.406f, 0.615f, 0.116f,
		0.676f, 0.977f, 0.133f,
		0.971f, 0.572f, 0.833f,
		0.140f, 0.616f, 0.489f,
		0.997f, 0.513f, 0.064f,
		0.945f, 0.719f, 0.592f,
		0.543f, 0.021f, 0.978f,
		0.279f, 0.317f, 0.505f,
		0.167f, 0.620f, 0.077f,
		0.347f, 0.857f, 0.137f,
		0.055f, 0.953f, 0.042f,
		0.714f, 0.505f, 0.345f,
		0.783f, 0.290f, 0.734f,
		0.722f, 0.645f, 0.174f,
		0.302f, 0.455f, 0.848f,
		0.225f, 0.587f, 0.040f,
		0.517f, 0.713f, 0.338f,
		0.053f, 0.959f, 0.120f,
		0.393f, 0.621f, 0.362f,
		0.673f, 0.211f, 0.457f,
		0.820f, 0.883f, 0.371f,
		0.982f, 0.099f, 0.879f
	}; 
		for (int i = 0; i < 3*triangles.size(); i++)
		{
			glm::vec3 temp;
			temp.x = colors[i];
			temp.y = colors[i+1];
			temp.z = colors[i+2];

			if (!uniform)
			{
				if (i % 3 == 0)
				{
					temp = temp * (float)(cos((triangles[i].x / width) - 1) + sin((triangles[i].y / height) - 1) + cos(triangles[i].x));
				}
				if (i % 3 == 1)
				{
					temp = temp * (float)(cos((triangles[i].x / width) - 1) + sin(triangles[i].z));
				}
				if (i % 3 == 2)
				{
					temp = temp * (float)(cos((triangles[i].x / width) - 1));
				}

			}
			color.push_back(temp);
		}
	}
	float* texturess = new float[2 * triangles.size()];
	for (int i = 0; i < 2 * triangles.size(); i++)
	{
		texturess[i] = 1.0;
	}

	unsigned int buffer;
	glGenVertexArrays(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, (triangles.size() + color.size() + nTriangles.size()) * sizeof(glm::vec3) + sizeof(texturess), 0, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, triangles.size() * sizeof(glm::vec3), &triangles[0]);
	glBufferSubData(GL_ARRAY_BUFFER, triangles.size() * sizeof(glm::vec3), sizeof(texturess), &texturess[0]);
	glBufferSubData(GL_ARRAY_BUFFER, triangles.size() * sizeof(glm::vec3) + sizeof(texturess), color.size() * sizeof(glm::vec3), &color[0]);
	glBufferSubData(GL_ARRAY_BUFFER, (triangles.size() + color.size()) * sizeof(glm::vec3) + sizeof(texturess), nTriangles.size() * sizeof(glm::vec3), &nTriangles[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid *)(triangles.size() * sizeof(glm::vec3)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid *)((triangles.size()) * sizeof(glm::vec3) + sizeof(texturess)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid *)((triangles.size() + color.size()) * sizeof(glm::vec3) + sizeof(texturess)));

	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	return &triangles;

}
