#include "MeshModel.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ImguiMenus.h>
#include<Light.h>
#define FACE_ELEMENTS 3

float max;

using namespace std;

// A struct for processing a single line in a wafefront obj file:
// https://en.wikipedia.org/wiki/Wavefront_.obj_file
struct FaceIdx
{
	// For each of the following 
	// Saves vertex indices
	int v[FACE_ELEMENTS];
	// Saves vertex normal indices
	int vn[FACE_ELEMENTS];
	// Saves vertex texture indices
	int vt[FACE_ELEMENTS];

	FaceIdx()
	{
		for (int i = 0; i < FACE_ELEMENTS + 1; i++)
			v[i] = vn[i] = vt[i] = 0;
	}

	FaceIdx(std::istream& issLine)
	{
		for (int i = 0; i < FACE_ELEMENTS + 1; i++)
			v[i] = vn[i] = vt[i] = 0;

		char c;
		for (int i = 0; i < FACE_ELEMENTS; i++)
		{
			issLine >> std::ws >> v[i] >> std::ws;
			if (issLine.peek() != '/')
			{
				continue;
			}
			issLine >> c >> std::ws;
			if (issLine.peek() == '/')
			{
				issLine >> c >> std::ws >> vn[i];
				continue;
			}
			else
			{
				issLine >> vt[i];
			}
			if (issLine.peek() != '/')
			{
				continue;
			}
			issLine >> c >> vn[i];
		}
	}
};

glm::vec3 vec3fFromStream(std::istream& issLine)
{
	float x, y, z;
	issLine >> x >> std::ws >> y >> std::ws >> z;
	return glm::vec3(x, y, z);
}

glm::vec2 vec2fFromStream(std::istream& issLine)
{
	float x, y;
	issLine >> x >> std::ws >> y;
	return glm::vec2(x, y);
}


MeshModel::MeshModel(const char * fileName)
{
	worldTransform = normalTransform = glm::mat4(1.0);
	scale = glm::vec3(1.0);
	angle = glm::vec3(0.0);
	tran = glm::vec3(0.0);
	tran = glm::vec3(0);
	model_colors[0] = glm::vec4(1.0);
	model_colors[1] = glm::vec4(1.0);
	model_colors[2] = glm::vec4(1.0);
	model_colors[2].y = 0; 	model_colors[2].z = 0;
	box = 0;
	shiness =1;


	LoadFile(fileName);
	calcOrigin();
}
MeshModel::MeshModel(bool from_prim)
{

	worldTransform = normalTransform = glm::mat4(1.0);
	scale = glm::vec3(1.0);
	angle = glm::vec3(0.0);
	tran = glm::vec3(0.0);
	tran = glm::vec3(0);
	model_colors[0] = glm::vec4(1.0);
	model_colors[1] = glm::vec4(1.0);
	model_colors[2] = glm::vec4(1.0);
	shiness = 1;
	box = 0;

}
glm::mat4x4 MeshModel::getWorldTransformation()
{
	return worldTransform;
}
glm::mat4 MeshModel::translatey(glm::vec3 trans)
{
	glm::mat4 result(1.0);
	result[0][3] = trans.x;
	result[1][3] = trans.y;
	result[2][3] = trans.z;
	result = glm::transpose(result); //????
	return result;
}

glm::mat4 MeshModel::scaley(glm::vec3 sc) {
	glm::mat4 result(1.0);
	result[0][0] = sc.x;
	result[1][1] = sc.y;
	result[2][2] = sc.z;
	return result;
}

glm::mat4 MeshModel::rotateX(float deg) {
	glm::mat4 rotation_x(1.0);
	rotation_x[0][0] = 1;
	rotation_x[1][2] = -sin(deg*PI / 180);
	rotation_x[2][1] = sin(deg*PI / 180);
	rotation_x[1][1] = cos(deg*PI / 180);
	rotation_x[2][2] = cos(deg*PI / 180);
	rotation_x[3][3] = 1;
	rotation_x = glm::transpose(rotation_x); //??
	return rotation_x;
}

glm::mat4 MeshModel::rotateY(float deg) {
	glm::mat4 rotation_y(1.0);
	rotation_y[0][0] = cos(deg*PI / 180);
	rotation_y[0][2] = sin(deg*PI / 180);
	rotation_y[1][1] = 1;
	rotation_y[2][0] = -sin(deg*PI / 180);
	rotation_y[2][2] = cos(deg*PI / 180);
	rotation_y[3][3] = 1;
	rotation_y = glm::transpose(rotation_y); //??
	return rotation_y;
}

glm::mat4 MeshModel::rotateZ(float deg) {
	glm::mat4 rotation_z(1.0);
	rotation_z[0][0] = cos(deg*PI / 180);
	rotation_z[0][1] = -sin(deg*PI / 180);
	rotation_z[1][0] = sin(deg*PI / 180);
	rotation_z[1][1] = cos(deg*PI / 180);
	rotation_z[2][2] = 1;
	rotation_z[3][3] = 1;
	rotation_z = glm::transpose(rotation_z); //??
	return rotation_z;
}

void MeshModel::calcOrigin() {
	float SumX=0, SumY=0, SumZ=0,NumOfVert;
	glm::vec3 curr;
	NumOfVert = triangles.size(); //might want to use vertices instead
	for (size_t i = 0; i < NumOfVert; i++) {
		curr = triangles[i];
		SumX += curr.x;
		SumY += curr.y;
		SumZ += curr.z;
	}
	origin = glm::vec3(SumX / NumOfVert, SumY / NumOfVert, SumZ / NumOfVert);
	
}

glm::vec3 MeshModel::getOrigin() {
	calcOrigin();
	return origin;
}

MeshModel::~MeshModel()
{
	
}
void MeshModel::LoadFile(const string& fileName)
{
	std::vector< glm::vec3 > temp_normals;
	std::vector<glm::vec2>temp_texture;
	
	/*std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;
	numberOfFaces = 0;
	cout << "in load" << endl;
	FILE * file = fopen(fileName, "r");
	if (file == NULL) {
		cout << "Impossible to open the file !" << endl;;
		return ;
	}
	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

				   // else : parse lineHeader
		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			cout << "im here!" << endl;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			numberOfFaces++;
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
			if (matches != 6) {
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}
	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		unsigned int vertexIndex = vertexIndices[i];
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		triangles.push_back(vertex);
		
	}
	for (unsigned int i = 0; i < normalIndices.size(); i++)
	{
		there_is_normals = 1;
		unsigned int normalIndex = normalIndices[i];
		glm::vec3 vertex = temp_normals[normalIndex - 1];
		nTriangles.push_back(vertex);
		
	}

	vertexPositions = triangles;
	normals = nTriangles;*/

	ifstream ifile(fileName.c_str());
	vector<FaceIdx> faces;
	// while not end of file
	while (!ifile.eof())
	{
		// get line
		string curLine;
		getline(ifile, curLine);

		// read the type of the line
		istringstream issLine(curLine);
		string lineType;

		issLine >> std::ws >> lineType;

		// based on the type parse data
		if (lineType == "v")
		{
			glm::vec3 v = vec3fFromStream(issLine);

			vertices.push_back(v);
		}
		else if (lineType == "f")
		{
			faces.push_back(issLine);
		}
		else if (lineType == "vn")
		{
			glm::vec3 v = vec3fFromStream(issLine);
			temp_normals.push_back(v);
			there_is_normals = 1;
		}
		else if (lineType == "vt")
		{
			glm::vec2 v = vec2fFromStream(issLine);
			temp_texture.push_back(v);
		}
		else if (lineType == "#" || lineType == "")
		{
			// comment / empty line
		}
		else
		{
			cout << "Found unknown line Type \"" << lineType << "\"";
		}
	}
	///this part makes sure the model's vertices fit in the viewframe
	glm::vec3 maxes(0.0);
	for each (glm::vec3 var in vertices)
	{
		maxes.x = std::fmax(maxes.x, var.x);
		maxes.y = std::fmax(maxes.y, var.y);
		maxes.z = std::fmax(maxes.z, var.z);
	}
	max = std::fmax(std::fmax(maxes.x, maxes.y), maxes.z);
	for (vector<glm::vec3>::iterator it = vertices.begin(); it != vertices.end(); it++) {
		*it = (*it / max) * (float)0.5;
	}
	//there is still a slight problem with normals when adding those couple of lines
	///TODO: FIX NORMALs' length when rendering
	for (vector<glm::vec3>::iterator it = normals.begin(); it != normals.end(); it++) {
		*it = (*it / max) * (float)0.001;
	}

	//Vertex_positions is an array of vec3. Every three elements define a triangle in 3D.
	//If the face part of the obj is
	//f 1 2 3
	//f 1 3 4
	//Then vertexPositions should contain:
	//vertexPositions={v1,v2,v3,v1,v3,v4}

	numberOfFaces = faces.size();

	vector<glm::vec3> temp_norm;
	for (int i = 0; i<vertices.size(); i++)
	{
		vector<glm::vec3> temp;
		int counter = 0;
		int k = 0;
		for (vector<FaceIdx>::iterator it = faces.begin(); it != faces.end(); ++it)
		{
			for (int j = 0; j < FACE_ELEMENTS; j++)
			{
				if (vertices[faces[k / 3].v[j] - 1] == vertices[i])
				{

					bool exists = 0;
					for (int m = 0; m < temp.size(); m++)
					{
						if (temp[m] == temp_normals[faces[k / 3].vn[j] - 1])
						{
							exists = 1;
						}
					}
					if (!exists)
					{
						counter++;
						temp.push_back(temp_normals[faces[k / 3].vn[j] - 1]);
		
					}

				}
				k++;
			}

		}
		glm::vec3 sum = glm::vec3(0.0);
		for (int m = 0; m < temp.size(); m++)
		{
			sum += temp[m];
		}


		sum.x /= counter;
		sum.y /= counter;
		sum.z /= counter;
		sum = glm::normalize(sum);

		
		temp_norm.push_back(sum);

	}

	// iterate through all stored faces and create triangles

	int k = 0;
	for (vector<FaceIdx>::iterator it = faces.begin(); it != faces.end(); ++it)
	{
		for (int i = 0; i < FACE_ELEMENTS; i++)
		{
			//Note that, each face, has the *indexes* of the vertices that make up the face
			//those indexes start from 1 (and not 0 like the rest of the containers)
			//therfore we have to decreament by 1 to get the correct index in the vertices vector
			
			
			normals.push_back(temp_norm[faces[k / 3].v[i] - 1]);

			triangles.push_back(vertices[faces[k / 3].v[i] - 1]);

			nTriangles.push_back(normals[k]);
			if (temp_texture.size() != 0) {
				texTriangles.push_back(temp_texture[faces[k / 3].vt[i] - 1]);
			}
			k++;
		}
		
	}
	
	
	normals = nTriangles;
	vertexPositions = triangles;
}
	

vector<glm::vec3>* MeshModel::Draw(vector<Light*> lights, float *colorBuffer, float *zBuffer, int width, int height)
{

	triangles.clear();
	nTriangles.clear();
	float x_max, x_min, z_max, z_min, y_max, y_min;

	glm::mat4 scales(1.0), rotation_y(1.0), rotation_x(1.0), rotation_z(1.0), translate(1.0);
	glm::vec4 temp;
	glm::vec3 tVec,nVec;

	//setting the scale matrix
	scales = this->scaley(scale);

	//setting the rotation matrix
	{
		rotation_y = this->rotateY(angle.y);
		rotation_z = this->rotateZ(angle.z);
		rotation_x = this->rotateX(angle.x);
	}

	translate = this->translatey(tran);
	objectTransform = translate* rotation_x*rotation_y*rotation_z*scales;

	glm::mat4 overall = worldTransform*objectTransform;
	normalTransform = glm::inverse(overall);

	for (int i = 0; i < numberOfFaces*FACE_ELEMENTS; i++) {
		tVec = vertexPositions[i];
		nVec = normals[i];
		temp = glm::vec4(tVec, 1.0);


		tVec =  overall*temp;

		triangles.push_back(tVec);
	
		temp = glm::vec4(nVec, 0); //might change to 0

		nVec = temp* normalTransform;
		nTriangles.push_back( nVec);


	}
	overall = projectionmat * Ctrans;
	for (int i = 0; i < triangles.size(); i++) {
		tVec = triangles[i];
		nVec = nTriangles[i];
		temp = glm::vec4(tVec, 1.0);


		tVec = overall * temp;

		triangles[i] = tVec;

		temp = glm::vec4(nVec, 0); //might change to 0

		nVec = temp * glm::inverse(overall);
		nVec = glm::normalize(nVec);
		nTriangles[i] = nVec;


	}

	
	vector<glm::vec3> color;
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
		normals_per_face.push_back(face_normal);
		center_points.push_back(center);
	}
	
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
				/*renderPixel((int)(triangles[i].x), (int)(triangles[i].y), triangles[i].z, temp, colorBuffer, zBuffer, width, height);
				temp.x = colorBuffer[(int)(3*((int)(triangles[i].x)+ (int)(triangles[i].y) * width)+0)];
				temp.y = colorBuffer[(int)(3 * ((int)(triangles[i].x) + (int)(triangles[i].y) * width) + 1)];
				temp.z = colorBuffer[(int)(3 * ((int)(triangles[i].x) + (int)(triangles[i].y) * width) + 2)];*/
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
		
		for (int i = 0; i < triangles.size(); i++)
		{
			glm::vec3 temp = model_colors[2];
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
	float* texturess = new float[2*triangles.size()];
	if (texTriangles.size())
	{
		for (int i = 0; i < 2 * triangles.size(); i += 2)
		{

			texturess[i] = texTriangles[i / 2].x;
			texturess[i + 1] = texTriangles[i / 2].y;

		}
	}
	else {
		for (int i = 0; i < 2 * triangles.size(); i += 2)
		{

			texturess[i] = triangles[i / 2].x;
			texturess[i + 1] = triangles[i / 2].y;

		}
	}

		unsigned int buffer;
		glGenVertexArrays(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, (triangles.size() + color.size() + nTriangles.size()) * sizeof(glm::vec3)+sizeof(texturess), 0, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, triangles.size() * sizeof(glm::vec3), &triangles[0]);
		glBufferSubData(GL_ARRAY_BUFFER, triangles.size() * sizeof(glm::vec3), sizeof(texturess), &texturess[0]);
		glBufferSubData(GL_ARRAY_BUFFER, triangles.size() * sizeof(glm::vec3)+sizeof(texturess), color.size() * sizeof(glm::vec3), &color[0]);
		glBufferSubData(GL_ARRAY_BUFFER, (triangles.size() + color.size()) * sizeof(glm::vec3)+sizeof(texturess), nTriangles.size() * sizeof(glm::vec3), &nTriangles[0]);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (GLvoid *)(triangles.size() * sizeof(glm::vec3)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid *)((triangles.size() ) * sizeof(glm::vec3)+sizeof(texturess)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid *)((triangles.size()+color.size()) * sizeof(glm::vec3) + sizeof(texturess)));

		glBindBuffer(GL_ARRAY_BUFFER, buffer);

		return &triangles;
	
	

}

vector<glm::vec3>* MeshModel::get_center_points()
{
	return &center_points;
}


void MeshModel::renderPixel(int i, int j, float k, glm::vec3 color, float *colorBuffer, float *zBuffer,int width,int height)
{
	if (i < 0 || i >= width || j<0 || j>= height)
	{
		cout << "something wrong" << endl;
		return;
	}
	if (k <= zBuffer[i + j * width])
	{
		zBuffer[i + j * width] = k;
		colorBuffer[3*(i+j*width)+0] = color.x;
		colorBuffer[3 * (i + j * width) + 1] = color.y;
		colorBuffer[3 * (i + j * width) + 2] = color.z;

	}
}

glm::vec4 MeshModel::calc_color(glm::vec3 vertex, Light* src, glm::vec3 vertex_normal, glm::vec4 arr[3])
{
	glm::vec3 color;
	color = ambient_color(vertex, src, vertex_normal, arr) +
		specular_color(vertex, src, vertex_normal, arr) +
		diffuse_color(vertex, src, vertex_normal, arr);
	glm::vec4 final_color = glm::vec4(color, (arr[0] + arr[1] + arr[2]).a);
	return final_color;
}
glm::vec3 MeshModel::diffuse_color(glm::vec3 vertex, Light* src, glm::vec3 vertex_normal, glm::vec4 arr[3])
{
	glm::vec4 temp = glm::vec4(vertex, 1.0);
	glm::vec4 pos = src->get_pos() - temp;//pay attention to the returned value in case of fail
	glm::vec3 to_light = pos;
	if (pos.w == -1)
	{
		return glm::vec3(0.0);
	}
	float cos_angle;
	to_light = glm::normalize(to_light);
	cos_angle = dotProduct(vertex_normal, to_light);
	cos_angle = glm::clamp(cos_angle, (float)0.0, (float)1.0);
	glm::vec3 final_color = glm::vec3(arr[0])*cos_angle*glm::vec3(src->light_color[0]);
	if (cos_angle > 0.0) {
		final_color = final_color * (float)(1.0 - cos_angle);
	}
	return final_color;


}

float MeshModel::dotProduct(glm::vec3 v0, glm::vec3 v1) {
	return v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2];
}

glm::vec3 MeshModel::specular_color(glm::vec3 vertex, Light* src, glm::vec3 vertex_normal, glm::vec4 arr[3])
{
	glm::vec3 to_camera, color;
	glm::vec4 to_light, temp, reflection;
	float temp_result, cos_angle;

	temp = glm::vec4(vertex, 1.0);


	to_camera[0] = 0 - vertex[0];
	to_camera[1] = 0 - vertex[1];
	to_camera[2] = 0 - vertex[2];

	to_light = src->get_pos();
	if (to_light.w == -1)
		return glm::vec3(0.0);

	to_light -= temp;//pay attention to the returned value in case of fail
	to_light = glm::normalize(to_light);

	vertex_normal = glm::normalize(vertex_normal);

	temp_result = 2.0 * dotProduct(vertex_normal, to_light);

	temp.x = (temp_result * vertex_normal).x;
	temp.y = (temp_result * vertex_normal).y;
	temp.z = (temp_result * vertex_normal).z;
	temp.w = 1;

	reflection = temp - to_light;

	to_camera = (float)-1.0 * vertex;

	reflection = glm::normalize(reflection);
	to_camera = glm::normalize(to_camera);
	cos_angle = dotProduct(reflection, to_camera);
	cos_angle = glm::clamp(cos_angle, (float) 0.0, (float)1.0);
	cos_angle = pow(cos_angle, shiness);

	if (cos_angle > 0.0) {
		color = src->light_color[1] * cos_angle*model_colors[1];
	}
	else {
		color = glm::vec3(0.0, 0.0, 0.0);
	}

	return color;






}

glm::vec3 MeshModel::ambient_color(glm::vec3 vertex, Light* src, glm::vec3 vertex_normal, glm::vec4 arr[3])
{
	glm::vec3 color = src->light_color[2]*(arr[2]);
	return color;
}


 vector<glm::vec3>* MeshModel:: get_normal()
{
	 return &nTriangles;
}

void MeshModel::setWorldTransformation(glm::vec3 _Wscale, glm::vec3 _Wangle, glm::vec3 _Wtrans)
{
	//in this method, we get rotations in degrees, we have to change them to radians
	glm::mat4 scales(1.0), rotation_y(1.0), rotation_x(1.0), rotation_z(1.0), translate(1.0);

	//setting the scale matrix
	scales = this->scaley(_Wscale);

	//setting the rotation matrix
	{
		rotation_y = this->rotateY(_Wangle.y);
		rotation_z = this->rotateZ(_Wangle.z);
		rotation_x = this->rotateX(_Wangle.x);
	}

	translate = translatey(_Wtrans);

	worldTransform = translate*rotation_x*rotation_y*rotation_z*scales;
}

bool MeshModel::get_if_prim()
{
	return 0;
 }

glm::mat4x4 MeshModel::get_object_transform()
{
//	return objectTransform;
	return worldTransform;
}

glm::mat4 MeshModel::getTransform() {
	return worldTransform * objectTransform;
}
void MeshModel::set_face_normal(vector<glm::vec3> new_normals)
{
	normals_per_face = new_normals;
}

vector<glm::vec3>* MeshModel::get_face_normal()
{
	return &normals_per_face;
}

void MeshModel::SetCameraTransform(glm::mat4x4 ctransform)
{
	Ctrans = ctransform;
}
void MeshModel::SetProjection(glm::mat4x4 projection)
{
	projectionmat = projection;
}

