#pragma once
#include <glm/glm.hpp>
#include <string>
#include "Model.h"
extern float max;

using namespace std;

/*
* MeshModel class. Mesh model object represents a triangle mesh (loaded fron an obj file).
*
*/
class MeshModel : public Model
{
protected:
	glm::mat4 translatey(glm::vec3 trans);
	glm::mat4 scaley(glm::vec3 sc);
	glm::mat4 rotateX(float deg);
	glm::mat4 rotateY(float deg);
	glm::mat4 rotateZ(float deg);
	glm::vec4 calc_color(glm::vec3 vertex, Light* src, glm::vec3 vertex_normal, glm::vec4 arr[3]);
	glm::vec3 ambient_color(glm::vec3 vertex, Light* src, glm::vec3 vertex_normal, glm::vec4 arr[3]);
	glm::vec3 specular_color(glm::vec3 vertex, Light* src, glm::vec3 vertex_normal, glm::vec4 arr[3]);
	float dotProduct(glm::vec3 v0, glm::vec3 v1);
	glm::vec3 diffuse_color(glm::vec3 vertex, Light* src, glm::vec3 vertex_normal, glm::vec4 arr[3]);
	void renderPixel(int i, int j, float k, glm::vec3 color, float *colorBuffer, float *zBuffer, int width, int height);

	MeshModel() {}
	vector<glm::vec3> vertices;
	vector<glm::vec3> normals;
	vector<glm::vec3> normals_per_face;
	vector<glm::vec3> center_points;
	vector<glm::vec3> vertexPositions;
	vector<glm::vec3> triangles;
	vector<glm::vec3> nTriangles;
	vector <glm::vec2> texTriangles;


	//The origin of the model
	glm::vec3 origin;
	// Add more attributes.
	glm::mat4x4 worldTransform;
	glm::mat4x4 normalTransform;
	glm::mat4x4 objectTransform;
	glm::mat4x4 projectionmat;
	glm::mat4x4 Ctrans;
	void calcOrigin();



public:
	MeshModel(const char * fileName );
	MeshModel(bool from_prim);

	~MeshModel();
	void LoadFile(const string& fileName);

	virtual vector<glm::vec3>* Draw(vector<Light*> lights, float *colorBuffer, float *zBuffer, int width, int height);
	virtual void SetCameraTransform(glm::mat4x4 ctransform) ;
	virtual void SetProjection(glm::mat4x4 projection) ;
	virtual vector<glm::vec3>* get_normal();
	vector<glm::vec3>* get_center_points();

	void set_face_normal(vector<glm::vec3> new_normals);
	vector<glm::vec3>* get_face_normal();
	virtual bool get_if_prim();
	virtual glm::mat4x4 get_object_transform() ;
	glm::mat4x4 getWorldTransformation();

	virtual glm::mat4 getTransform();

	virtual void setWorldTransformation(glm::vec3 _Wscale, glm::vec3 _Wangle, glm::vec3 _Wtrans);
	glm::vec3 getOrigin();
	size_t numberOfFaces;


};

//#########################33
