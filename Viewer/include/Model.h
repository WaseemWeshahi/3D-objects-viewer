#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <Light.h>

using namespace std;
/*
* Model class. An abstract class for all types of geometric data.
*/
class Model
{
public:

		virtual vector<glm::vec3>* get_normal() = 0;
		virtual vector<glm::vec3>* get_face_normal() = 0;
		virtual void set_face_normal(vector<glm::vec3> new_normals) = 0;
		virtual vector<glm::vec3>* get_center_points() = 0;

	//virtual void compute_bounding_box(glm::mat4x4 projectionmat, glm::mat4x4 transform, int width, int height, Renderer& renderer)=0;
	//sets the world matrix
	virtual void setWorldTransformation(glm::vec3 _Wscale, glm::vec3 _Wangle, glm::vec3 _Wtrans) = 0;
	
	virtual glm::mat4x4 getWorldTransformation() = 0;
	virtual void SetCameraTransform(glm::mat4x4 ctransform) = 0;
	virtual void SetProjection(glm::mat4x4 projection) = 0;
	virtual glm::mat4 getTransform() = 0;
	virtual glm::vec3 getOrigin() = 0;
	virtual bool get_if_prim() = 0;
	virtual vector<glm::vec3>* Draw(vector<Light*> lights, float *colorBuffer, float *zBuffer, int width, int height) = 0;

	virtual glm::mat4x4 get_object_transform() = 0;
	glm::vec3 scale, angle, tran; //consider changing tran
	glm::vec4 model_colors[3] ;
	int shiness;
	bool there_is_normals;
	bool box;



protected:
	virtual ~Model() {}
};

