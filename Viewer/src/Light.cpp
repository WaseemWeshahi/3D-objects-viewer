#include "Light.h"
#include "InitShader.h"
#include <imgui/imgui.h>
#include<GLFW/glfw3.h>
#include<GLFW/glfw3native.h>
#include <ImguiMenus.h>
#include<iostream>
#include<math.h>


Light::Light()
{
}


Light::~Light()
{
}

//////////////////////////
/////////////////////////
////////////////////////
///////////////////////


PointLight::PointLight(glm::vec3 my_pos)
{
	position.x = 1;
	position.y = 1;
	position.z = 1;
	position.w = 1;

	type=0;
	ambient_percentages = glm::vec3(0);


	light_color[0] = glm::vec4(1.0);
	light_color[1] = glm::vec4(1.0);
	light_color[2] = glm::vec4(1.0);

	worldTransform  = glm::mat4(1.0);
	scale = glm::vec3(1.0);
	angle = glm::vec3(0.0);
	tran = glm::vec3(0.0);
	tran = glm::vec3(0);
	firs_pos = position;
}


void PointLight::set_matrixes(glm::mat4x4 world_trans, glm::mat4x4 cTrans, glm::mat4x4 projection)
{
	worldTransform = world_trans;
	cTransform = cTrans;
	projection_mat = projection;
}

void PointLight::draw_source()
{
	vector<glm::vec3> vert;
	vert.push_back(position);
	vector<glm::vec3> colorss;
	colorss.push_back(glm::vec3{ 1.0,1.0,1.0 });
	
	vector<glm::vec3>nvert;
	nvert.push_back(position);
	
	float* texturess = new float[2 * vert.size()];
	for (int i = 0; i < 2 * vert.size(); i++)
	{
		texturess[i] = 1.0;
	}
	unsigned int buffer;
	glGenVertexArrays(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, (vert.size() + colorss.size() + nvert.size()) * sizeof(glm::vec3) + sizeof(texturess), 0, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vert.size() * sizeof(glm::vec3), &vert[0]);
	glBufferSubData(GL_ARRAY_BUFFER, vert.size() * sizeof(glm::vec3), sizeof(texturess), &texturess[0]);
	glBufferSubData(GL_ARRAY_BUFFER, vert.size() * sizeof(glm::vec3) + sizeof(texturess), colorss.size() * sizeof(glm::vec3), &colorss[0]);
	glBufferSubData(GL_ARRAY_BUFFER, (vert.size() + colorss.size()) * sizeof(glm::vec3) + sizeof(texturess), nvert.size() * sizeof(glm::vec3), &nvert[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid *)(vert.size() * sizeof(glm::vec3)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid *)((vert.size()) * sizeof(glm::vec3) + sizeof(texturess)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid *)((vert.size() + colorss.size()) * sizeof(glm::vec3) + sizeof(texturess)));
	glDrawArrays(GL_POINT, 0, (vert).size());
}
void PointLight::calculate_new_pos()
{
	position = firs_pos;
	glm::mat4 scales(1.0), rotation_y(1.0), rotation_x(1.0), rotation_z(1.0), translate(1.0);

	//setting the translation matrix
	{
		translate[0][3] = tran.x;
		translate[1][3] = tran.y;
		translate[2][3] = tran.z;
		translate = glm::transpose(translate);
	}
	{
		rotation_y[0][0] = cos(angle.y*PI / 180);
		rotation_y[0][2] = sin(angle.y*PI / 180);
		rotation_y[1][1] = 1;
		rotation_y[2][0] = -sin(angle.y*PI / 180);
		rotation_y[2][2] = cos(angle.y*PI / 180);
		rotation_y[3][3] = 1;
		rotation_y = glm::transpose(rotation_y);
	}
	{
		rotation_z[0][0] = cos(angle.z*PI / 180);
		rotation_z[0][1] = -sin(angle.z*PI / 180);
		rotation_z[1][0] = sin(angle.z*PI / 180);
		rotation_z[1][1] = cos(angle.z*PI / 180);
		rotation_z[2][2] = 1;
		rotation_z[3][3] = 1;
		rotation_z = glm::transpose(rotation_z);
	}
	{
		rotation_x[0][0] = 1;
		rotation_x[1][2] = -sin(angle.x*PI / 180);
		rotation_x[2][1] = sin(angle.x*PI / 180);
		rotation_x[1][1] = cos(angle.x*PI / 180);
		rotation_x[2][2] = cos(angle.x*PI / 180);
		rotation_x[3][3] = 1;
		rotation_x = glm::transpose(rotation_x);
	}
	srcTransform = translate * rotation_x*rotation_y*rotation_z*scales;

	glm::mat4 overall = worldTransform * srcTransform;

	position = overall * position;
	
	
}
glm::vec4 PointLight::get_pos(glm::vec3 vertex)
{
	return position;
}
glm::vec3 PointLight::get_ambient_percentages()
{
	return ambient_percentages;
}

void PointLight::set_ambient_percentages(glm::vec4 newPercentage)
{
	ambient_percentages = newPercentage;
}

///////////////////
//////////////////
////////////////
//////////////
////////////
///////////
/////////
///////
/////
///

ParallelLight::ParallelLight()
{
	glm::vec3 temp = { 0,0,0 };

	light_color[0] = glm::vec4(1.0);
	light_color[1] = glm::vec4(1.0);
	light_color[2] = glm::vec4(1.0);

	worldTransform = glm::mat4(1.0);
	scale = glm::vec3(1.0);
	angle = glm::vec3(0.0);
	tran = glm::vec3(0.0);
	tran = glm::vec3(0);

	boundaries[0]=temp;// pushing 0,0,0
	temp.x = 1;
	boundaries[1]=temp;//pushing 1,0,0
	temp.z = 1;
	boundaries[2] = temp;//pushing 1,0,1
	temp.x = 0;
	boundaries[3] = temp;//pushing 0,0,1

	direction = glm::vec4{ 0,1,0,0 };
	type = 1;
	ambient_percentages = glm::vec3(1.0);
	first_boundaries[0].x = boundaries[0].x;
	first_boundaries[0].y = boundaries[0].y;
	first_boundaries[0].z = boundaries[0].z;

	first_boundaries[1].x = boundaries[1].x;
	first_boundaries[1].y = boundaries[1].y;
	first_boundaries[1].z = boundaries[1].z;

	first_boundaries[2].x = boundaries[2].x;
	first_boundaries[2].y = boundaries[2].y;
	first_boundaries[2].z = boundaries[2].z;

	first_boundaries[3].x = boundaries[3].x;
	first_boundaries[3].y = boundaries[3].y;
	first_boundaries[3].z = boundaries[3].z;
}
glm::vec3 ParallelLight::get_ambient_percentages()
{
	return ambient_percentages;
}


void ParallelLight::set_ambient_percentages(glm::vec4 newPercentage)
{
	ambient_percentages = newPercentage;
}

void ParallelLight::draw_source()
{
	vector<glm::vec3> vert;
	vert.push_back(boundaries[0]);
	vert.push_back(boundaries[1]);
	vert.push_back(boundaries[2]);

	vert.push_back(boundaries[0]);
	vert.push_back(boundaries[3]);
	vert.push_back(boundaries[2]);	
	
	vector<glm::vec3> colorss;
	colorss.push_back(glm::vec3{ 1.0,1.0,0.0 });
	colorss.push_back(glm::vec3{ 1.0,1.0,0.0 });
	colorss.push_back(glm::vec3{ 1.0,1.0,0.0 });
	colorss.push_back(glm::vec3{ 1.0,1.0,0.0 }); 
	colorss.push_back(glm::vec3{ 1.0,1.0,0.0 });
	colorss.push_back(glm::vec3{ 1.0,1.0,0.0 });

	vector<glm::vec3>nvert;
	nvert.push_back(boundaries[0]);
	nvert.push_back(boundaries[1]);
	nvert.push_back(boundaries[2]);
	nvert.push_back(boundaries[0]);
	nvert.push_back(boundaries[3]);
	nvert.push_back(boundaries[2]);

	float* texturess = new float[2 * vert.size()];
	for (int i = 0; i < 2 * vert.size(); i++)
	{
		texturess[i] = 1.0;
	}
	unsigned int buffer;
	glGenVertexArrays(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, (vert.size() + colorss.size() + nvert.size()) * sizeof(glm::vec3) + sizeof(texturess), 0, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vert.size() * sizeof(glm::vec3), &vert[0]);
	glBufferSubData(GL_ARRAY_BUFFER, vert.size() * sizeof(glm::vec3), sizeof(texturess), &texturess[0]);
	glBufferSubData(GL_ARRAY_BUFFER, vert.size() * sizeof(glm::vec3) + sizeof(texturess), colorss.size() * sizeof(glm::vec3), &colorss[0]);
	glBufferSubData(GL_ARRAY_BUFFER, (vert.size() + colorss.size()) * sizeof(glm::vec3) + sizeof(texturess), nvert.size() * sizeof(glm::vec3), &nvert[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid *)(vert.size() * sizeof(glm::vec3)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid *)((vert.size()) * sizeof(glm::vec3) + sizeof(texturess)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid *)((vert.size() + colorss.size()) * sizeof(glm::vec3) + sizeof(texturess)));
	glDrawArrays(GL_TRIANGLES, 0, (vert).size());
}

void ParallelLight::calculate_new_pos()

{
	glm::mat4 scales(1.0), rotation_y(1.0), rotation_x(1.0), rotation_z(1.0), translate(1.0);
	int i;
	boundaries[0].x = first_boundaries[0].x;
	boundaries[0].y = first_boundaries[0].y;
	boundaries[0].z = first_boundaries[0].z;

	boundaries[1].x = first_boundaries[1].x;
	boundaries[1].y = first_boundaries[1].y;
	boundaries[1].z = first_boundaries[1].z;

	boundaries[2].x = first_boundaries[2].x;
	boundaries[2].y = first_boundaries[2].y;
	boundaries[2].z = first_boundaries[2].z;

	boundaries[3].x = first_boundaries[3].x;
	boundaries[3].y = first_boundaries[3].y;
	boundaries[3].z = first_boundaries[3].z;

	//setting scales matrix
	{
		scales[0][0] = scale.x;
		scales[1][1] = scale.y;
		scales[2][2] = scale.z;
	}
	
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
		//**********Watch out********
		
	}
	

	//setting the translation matrix
	{
		translate[0][3] = tran.x;
		translate[1][3] = tran.x;
		translate[2][3] = tran.x;
		translate = glm::transpose(translate);
	}

	srcTransform = translate * rotation_x*rotation_y*rotation_z*scales;

	glm::mat4 overall = worldTransform * srcTransform;

	for (i = 0; i < 4; i++)
	{
		boundaries[i] = glm::vec3(overall * glm::vec4(boundaries[i], 1.0));
		/*overall = projection_mat * cTransform;
		boundaries[i] = overall * glm::vec4(boundaries[i],1.0);*/
	}

	srcTransform = rotation_x*rotation_y*rotation_z;

	overall = worldTransform * srcTransform;
	//overall = projection_mat * cTransform;
	direction = overall * direction;

}

glm::vec4 ParallelLight::get_pos(glm::vec3 vertex)
{
	if ((vertex - boundaries[2]).y < 0)
	{
		return glm::vec4{ -1,-1,-1,-1 };
	}
	glm::vec3 pos,temp1,temp2;
	float angle,length_of,length2;
	
	temp1 = boundaries[2] - boundaries[3];
	temp2 = vertex - boundaries[3];
	
	length_of = sqrt((double)(temp2.x * temp2.x + temp2.y * temp2.y + temp2.z * temp2.z));
	
	temp1 = glm::normalize(temp1);
	temp2 = glm::normalize(temp2);

	angle = temp1.x*temp2.x + temp1.y*temp2.y + temp1.z*temp2.z; // need to make sure angle<=90
	if (acos(angle) > 90)
	{
		return glm::vec4(-1.0);

	}
	// computing sin angle

	angle = 1 - angle * angle; 

	angle = sqrt((double)angle);

	length2 = length_of;
	length_of = length_of * angle; // now length of has the length of the normal that connects the vertex to the surface

	length2 = sqrt((double)(length2 * length2 - length_of * length_of));
	
	pos.x =boundaries[3].x + length2;
	pos.y = boundaries[3].y;
	pos.z = boundaries[3].z;

	/*pos.x = vertex.x - length_of;
	pos.y = vertex.y - length_of;
	pos.z = vertex.z - length_of;
	*/

	if (pos.x-boundaries[3].x > boundaries[2].x-boundaries[3].x)
	{
		
		return glm::vec4(-1.0);
	}

	return glm::vec4(pos, 1.0);
	
	
	/*
	// needs to be checked they are not correct in case of a pixel below the source
	if (pos.x >= boundaries[0].x && pos.x <= boundaries[2].x && pos.y >= boundaries[0].y && pos.y <= boundaries[2].y)
	{
		return glm::vec4(pos, 1.0);
	}
	else if (pos.x >= boundaries[0].x && pos.x <= boundaries[2].x)
	{
		return glm::vec4{pos.x,boundaries[2].y,boundaries[2].z,1};
	}
	else if (pos.x <= boundaries[0].x )
	{
		return glm::vec4(boundaries[0],1.0);
	}
	else if ( pos.x >= boundaries[2].x)
	{
		return glm::vec4(boundaries[2],1.0);
	}*/
	
}
void ParallelLight::set_matrixes(glm::mat4x4 world_trans, glm::mat4x4 cTrans, glm::mat4x4 projection)
{
	worldTransform = world_trans;
	cTransform = cTrans;
	projection_mat = projection;

}

