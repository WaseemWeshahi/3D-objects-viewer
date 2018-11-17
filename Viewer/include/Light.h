#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <string>


/*
 * Light class. Holds light source information and data.
 test nave
 */
class Light
{
public:
	Light();
	~Light();
	glm::vec3 scale, angle, tran;
	bool type;
	glm::vec4 light_color[3];

	virtual void calculate_new_pos()=0;
	virtual glm::vec4 get_pos(glm::vec3 vertex = { 0,0,0 }) = 0;
	virtual glm::vec3 get_ambient_percentages() = 0;
	virtual void set_ambient_percentages(glm::vec4 newPercentage) = 0;
	virtual void draw_source() = 0;
	virtual void set_matrixes(glm::mat4x4 world_trans,glm::mat4x4 cTrans,glm::mat4x4 projection) = 0;


protected:
	glm::mat4x4 worldTransform;
	glm::mat4x4 srcTransform;
	glm::mat4x4 cTransform;
	glm::mat4x4 projection_mat;
	glm::vec3 ambient_percentages;

};

class PointLight :public Light
{
private:
	glm::vec4 position,firs_pos;
public:
	PointLight(glm::vec3 my_pos = { 0,0,0});
	void calculate_new_pos();
	glm::vec4 get_pos(glm::vec3 vertex);
 glm::vec3 get_ambient_percentages() ;
 void set_matrixes(glm::mat4x4 world_trans, glm::mat4x4 cTrans, glm::mat4x4 projection);
 void set_ambient_percentages(glm::vec4 newPercentage);
 void draw_source();



};

class ParallelLight : public Light
{
private:
	glm::vec4 direction;
	glm::vec3 boundaries[4];
	glm::vec3 first_boundaries[4];


public:
	ParallelLight();
	void calculate_new_pos();
	glm::vec4 get_pos(glm::vec3 vertex);
	glm::vec3 get_ambient_percentages();
	void set_matrixes(glm::mat4x4 world_trans, glm::mat4x4 cTrans, glm::mat4x4 projection);
	void set_ambient_percentages(glm::vec4 newPercentage);
	void draw_source();


};
