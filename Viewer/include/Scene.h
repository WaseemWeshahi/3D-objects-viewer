#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Model.h"
#include "Light.h"
#include "Camera.h"

using namespace std;

class Scene {
private:
	int width, height;
	// 3*width*height
	float *colorBuffer;
	// width*height
	float *zBuffer;

	vector<Light*> lights;
	vector<Camera*> cameras;
	vector<Model*> models;
	glm::mat4x4 transform, projectionmat, normalTransformation;


	
	GLuint glScreenTex;
	GLuint glScreenVtc;
	GLuint program;
	GLuint vPosition;
	GLint  location1, location2, location3, location4,location5,
	  location6,location7,location8,location9,location10,location11,
		location12,location13,location14;
	GLint  vnormal;
	GLint  vcolor;
	unsigned char* image;
	unsigned int new_width;
	unsigned int new_height;
	GLuint init_buffer;

	void initOpenGLRendering();
	void createOpenGLBuffer();
	void createBuffers(int w, int h);

	// Draw's a pixel in location p with color color
	void putPixel(int i, int j, const glm::vec3& color = glm::vec3(1, 0, 1));
	void render_pixel(int i, int j, float k, glm::vec3 color);
	void drawAxis(glm::mat4 overall);
	void SetCameraTransform(const glm::mat4x4 & cTransform);
	void SetProjection(const glm::mat4x4 & projection);
	void draw_normals(vector<glm::vec3> normals,vector<glm::vec3>vertices);
	void face_normals(vector<glm::vec3> face_normals, vector<glm::vec3>centerals);
	void calculate_bounding_box(vector<glm::vec3> vertices);
public:
	
	
	Scene(int w, int h) : width(w), height(h), Wscale(1.0),Wtrans(0.0),Wangle(0.0){
		initOpenGLRendering();
		createBuffers(w, h);
	};
	Scene() :width(1280), height(720)
	{
		initOpenGLRendering();
		createBuffers(1280, 720);
	}

	// Loads an obj file into the scene.
	void LoadOBJModel(const char * fileName);
	void LoadPrimitiveModel(string modelTybe="");


	// Draws the current scene.
	void Draw();

	// Draws an example.
	void DrawDemo();

	//parameters for the World trnasformation
	glm::vec3 Wangle, Wscale, Wtrans;

	void setActiveModel(int active);
	void ChangeProjection(const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar, bool isOrtho);
	void ChangeTransform(const glm::vec4& eye, const glm::vec4& at, const glm::vec4& up);
	void AddCamera();
	void AddCamera(Camera& camera);
	void removeCamera(int index);
	Camera * getActiveCamera();
	Model* getActiveModel();
	Light* getActiveLight();
	int getNumOfModels();
	void setActiveCamera(int index);
	void remove_module(int index);
	void AddLight(string type);
	int ActiveModel = 0;
	int ActiveLight=0;
	int ActiveCamera = 0;
	int numberOfCameras = 0;
	int numberOfLights = 0;

	void SwapBuffers();

	// Sets the color buffer to a new color (all pixels are set to this color).
	void ClearColorBuffer(const glm::vec3& color);

	// Resize the buffer.
	void Viewport(int w, int h);

	// Clears the z buffer to zero.
	void ClearDepthBuffer();

	// Draw wide vertical and horizontal lines on the screen
	void SetDemoBuffer();

};
