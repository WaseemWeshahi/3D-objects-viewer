#include "Scene.h"
#include "MeshModel.h"
#include "PrimMeshModel.h"
#include <string>
#include<iostream>
#include <ImguiMenus.h>
#include"Light.h"
#include "InitShader.h"
#include <glad/glad.h>    
#include <GLFW/glfw3.h>
#include "lodepng.h"
#include <string>

#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)

using namespace std;
using namespace lodepng;
void Scene::initOpenGLRendering()
{
	new_width = width;
	new_height = height;
    const char *filename = &(*textureName); //58+5+18
	unsigned error = lodepng_decode32_file(&image, &new_width, &new_height, filename);
	cout << lodepng_error_text(error) << endl;
	cout << &filename << endl;

	// Creates a unique identifier for an opengl texture.
	glGenTextures(1, &glScreenTex);
	// Same for vertex array object (VAO). VAO is a set of buffers that describe a renderable object.
	glGenVertexArrays(1, &glScreenVtc);
	// Makes this VAO the current one.
	glBindVertexArray(glScreenVtc);
	// Creates a unique identifier for a buffer.
	glGenBuffers(1, &init_buffer);
	// (-1, 1)____(1, 1)
	//       |\  |
	//       | \ | <--- The exture is drawn over two triangles that stretch over the screen.
	//       |__\|
	// (-1,-1)    (1,-1)
	const GLfloat vtc[] = {
		-1, -1,0,
		1, -1,0,
		-1,  1,0,
		-1,  1,0,
		1, -1,0,
		1,  1,0
	};
	const GLfloat col[] = {
		1,0,0,
		1,0,0,
		1,0,0,
		1,0,0,
		1,0,0,
		1,0,0,
	};
	const GLfloat tex[] = {
		0,0,
		1,0,
		0,1,
		0,1,
		1,0,
		1,1 };

	const GLfloat vn[] = {
		0, -1,
		0, -1,
		0,  1,
		0,  1,
		0, -1,
		0,  1
	};
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	// Makes this buffer the current one.
	glBindBuffer(GL_ARRAY_BUFFER, init_buffer);
	// This is the opengl way for doing malloc on the gpu. 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc) + sizeof(col) + sizeof(vn) + sizeof(tex), NULL, GL_STATIC_DRAW);
	// memcopy vtc to buffer[0,sizeof(vtc)-1]
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);
	// memcopy col to buffer[sizeof(vtc),sizeof(vtc)+sizeof(col)]
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vtc) + sizeof(tex), sizeof(col), col);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vtc) + sizeof(tex) + sizeof(col), sizeof(vn), vn);

	// Loads and compiles a sheder.
	program = InitShader("vshader.glsl", "fshader.glsl");
	// Make this program the current one.
	glUseProgram(program);
	// Tells the shader where to look for the vertex position data, and the data dimensions.
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// Same for texture coordinates data.
	GLint  vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)(sizeof(vtc)));
	glUniform1i(glGetUniformLocation(program, "texture"), 0);

	vcolor = glGetAttribLocation(program, "vcolor");
	glEnableVertexAttribArray(vcolor);
	glVertexAttribPointer(vcolor, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)(sizeof(vtc) + sizeof(tex)));
	vnormal = glGetAttribLocation(program, "vnormal");
	glEnableVertexAttribArray(vnormal);
	glVertexAttribPointer(vnormal, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)(sizeof(vtc) + sizeof(col) + sizeof(tex)));


	//glProgramUniform1i( program, glGetUniformLocation(program, "texture"), 0 );

	location1 = glGetUniformLocation(program, "u_light_ambient");
	//assert(location1 != -1);
	glUniform1fv(location1, 0, NULL);
	location2 = glGetUniformLocation(program, "material_shiness");
	//assert(location2 != -1);
	glUniform1i(location2, 1);
	location3 = glGetUniformLocation(program, "u_matirial_diffuse");
	glUniform4f(location3, 1.0, 1.0, 1.0, 1.0);
	location4 = glGetUniformLocation(program, "u_light_position");
	//assert(location4 != -1);
	glUniform1fv(location4, 0, NULL);
	//assert(location5 != -1);
	location6 = glGetUniformLocation(program, "phong_shading");
	glUniform1i(location6, 0);
	location7 = glGetUniformLocation(program, "u_matirial_specular");
	glUniform4f(location7, 1.0, 1.0, 1.0, 1.0);
	location8 = glGetUniformLocation(program, "u_light_spec");
	//assert(location1 != -1);
	glUniform1fv(location8, 0, NULL);
	location9 = glGetUniformLocation(program, "u_light_diff");
	//assert(location1 != -1);
	glUniform1fv(location9, 0, NULL);
	location5 = glGetUniformLocation(program, "first_time");
	glUniform1i(location5, 1);
	location10 = glGetUniformLocation(program, "number_of_lights");
	glUniform1i(location10, 0);
	location11 = glGetUniformLocation(program, "a_flat");
	glUniform1i(location10, 0);
	location12 = glGetUniformLocation(program, "with_bloom");
	glUniform1i(location12, 0);
	location13 = glGetUniformLocation(program, "with_blur");
	glUniform1i(location13, 0);
	location14 = glGetUniformLocation(program, "with_tex");
	glUniform1i(location14, 1);

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);


}
void Scene::createOpenGLBuffer()
{

// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);
	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, glScreenTex);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// malloc for a texture on the gpu.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, new_width, new_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glViewport(0, 0, width, height);
	
}
void Scene::createBuffers(int w, int h)
{
	createOpenGLBuffer(); //Do not remove this line.

	colorBuffer = new float[3 * w*h];
	zBuffer = new float[w*h];
	//bright = new float[3 * (h + 4)*(w + 4)];
	//blurred = new float[3 * w*h];
	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			putPixel(i, j, glm::vec3(1.0f, 1.0f, 1.0f));
			zBuffer[i + j * width] = 1.0; //need to pick one of the values
		}
	}
	/*
	//creating a bright buffer
	for (int i = 0; i < w + 4; i++) {
		for (int j = 0; j < h + 4; j++) {
			bright[i + j * (width + 2)] = blurred[i + j * (width + 2)] = 0;
		}
	}

	//creating the blurring kernel
	for (int i = -2; i < 3; i++) {
		for (int j = -2; j < 3; j++) {
			kernel[j + 2][i + 2] = (1 / 1024.0)* exp(-(i*i) - (j*j) / (2 * PI*PI ));
		}
	}*/

}
void Scene::SetDemoBuffer()
{
	int r = 5;
	// Wide red vertical line
	glm::vec4 red = glm::vec4(1, 0, 0, 1);
	for (int i = 0; i<height; i++)
	{
		for (int r0 = 0; r0 < r; r0++)
		{
			putPixel((width / 2) + r0, i, red);
			putPixel((width / 2) - r0, i, red);
		}
	}
	// Wide magenta horizontal line
	glm::vec4 magenta = glm::vec4(1, 0, 1, 1);
	for (int i = 0; i<width; i++)
	{
		for (int r0 = 0; r0 < r; r0++)
		{
			putPixel(i, (height / 2) + r0, red);
			putPixel(i, (height / 2) - r0, red);
		}

	}
}
void Scene::SwapBuffers()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);
	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, glScreenTex);
	// memcopy's colorBuffer into the gpu.
	if (add_texture)
	{

		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, new_width, new_height, GL_RGBA, GL_UNSIGNED_BYTE, image);
	}
	else
	{
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, colorBuffer);
	}
	// Tells opengl to use mipmapping
	glGenerateMipmap(GL_TEXTURE_2D);
	// Make glScreenVtc current VAO
	glBindVertexArray(glScreenVtc);
	// Finally renders the data.
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
void Scene::putPixel(int i, int j, const glm::vec3& color)
{
	if (i < 0) return; if (i >= width) return;
	if (j < 0) return; if (j >= height) return;
	colorBuffer[INDEX(width, i, j, 0)] = color.x;
	colorBuffer[INDEX(width, i, j, 1)] = color.y;
	colorBuffer[INDEX(width, i, j, 2)] = color.z;
}
void Scene::render_pixel(int i, int j, float k, glm::vec3 color)
{
	if (i < 0) return; if (i >= width) return;
	if (j < 0) return; if (j >= height) return;
	if (k <= zBuffer[i + j * width])
	{
		zBuffer[i + j * width] = k;
		putPixel(i, j, color);

	}
}
void Scene::ClearColorBuffer(const glm::vec3& color)
{
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			putPixel(i, j, color);
			//blurred[i + j * width] = 0;
		}
	}
}

void Scene::ClearDepthBuffer()
{
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{

			zBuffer[i + j * width] = 1; //or is it i*height? not sure
		}
	}
}

void Scene::Viewport(int w, int h)
{
	if (w == width && h == height)
	{
		return;
	}
	width = w;
	height = h;
	delete[] colorBuffer;
	colorBuffer = new float[3 * h*w];

	/*delete[] bright;
	bright = new float[3 * (h + 4) * (w + 4)];

	delete[] blurred;
	blurred = new float[3 * w*h];
	*/
	delete[] zBuffer;
	zBuffer = new float[w*h];

	createOpenGLBuffer();
}

void Scene::LoadOBJModel(const char * fileName)
{
	MeshModel *model = new MeshModel(fileName);
	models.push_back(model);
	counter++;
	ImGui::Text("counter = %d", counter);
	ActiveModel = counter;

}

void Scene::LoadPrimitiveModel(string modelTybe)
{
	
		PrimMeshModel* model = new PrimMeshModel();
		models.push_back(model);
		counter++;
		ImGui::Text("counter = %d", counter);
		ActiveModel = counter;
		cout << "done load prim" << endl;

	

}
void Scene::drawAxis(glm::mat4 overall)
{

	glm::vec4 orig(0, 0, 0, 1), x(1, 0, 0, 1), y(0, 1, 0, 1), z(0, 0, 1, 1);
	overall = projectionmat * transform*overall;
	orig = overall * orig;
	x = overall * x;
	y = overall * y;
	z = overall * z;
	//orig = orig / orig.w;
	x = x;
	y = y;
	z = z;
	vector<glm::vec3> vert;
	vert.push_back(orig);
	vert.push_back(x);
	vert.push_back(orig);
	vert.push_back(y);
	vert.push_back(orig);
	vert.push_back(z);
	vector<glm::vec3> colorss;
	colorss.push_back(glm::vec3{ 1.0,0.0,0.0 });
	colorss.push_back(glm::vec3{ 1.0,0.0,0.0 });
	colorss.push_back(glm::vec3{ 0.0,1.0,0.0 });
	colorss.push_back(glm::vec3{ 0.0,1.0,0.0 });
	colorss.push_back(glm::vec3{ 0.0,0.0,1.0 });
	colorss.push_back(glm::vec3{ 0.0,0.0,1.0 });
	vector<glm::vec3>nvert;
	nvert.push_back(orig);
	nvert.push_back(x);
	nvert.push_back(orig);
	nvert.push_back(y);
	nvert.push_back(orig);
	nvert.push_back(z);
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
	glDrawArrays(GL_LINES, 0, (vert).size());


}

void Scene::SetCameraTransform(const glm::mat4x4 & cTransform)
{
	transform = cTransform;
}
void Scene::SetProjection(const glm::mat4x4 & projection)
{
	projectionmat = projection;
}
void Scene::Draw()
{
	vector<glm::vec3>* vertices;
	vector<glm::vec3>* normals;
	int size;
	int i;
	// 1. Send the renderer the current camera transform and the projection
	
	SetCameraTransform(cameras[ActiveCamera]->get_transformation());
	SetProjection((cameras[ActiveCamera])->get_projection());
	
	if (lights.size() != 0)
	{
		models[0]->setWorldTransformation(Wscale, Wangle, Wtrans);
		lights[ActiveLight]->set_matrixes(models[0]->getWorldTransformation(),(cameras[ActiveCamera]->get_transformation()), (cameras[ActiveCamera])->get_projection());
		lights[ActiveLight]->calculate_new_pos();
		if (lights.size()) 
		{
			float* light_ambient; float* light_diff; float* light_spec; float* light_pos;
			light_ambient = new float[4 * lights.size()];
			light_diff = new float[4 * lights.size()];
			light_spec = new float[4 * lights.size()];
			light_pos = new float[4 * lights.size()];
			for (int j = 0; j < 4 * lights.size(); j += 4)
			{
				light_ambient[j] = lights[j / 4]->light_color[2].x;
				light_ambient[j + 1] = lights[j / 4]->light_color[2].y;
				light_ambient[j + 2] = lights[j / 4]->light_color[2].z;
				light_ambient[j + 3] = lights[j / 4]->light_color[2].w;

				light_diff[j] = lights[j / 4]->light_color[0].x;
				light_diff[j + 1] = lights[j / 4]->light_color[0].y;
				light_diff[j + 2] = lights[j / 4]->light_color[0].z;
				light_diff[j + 3] = lights[j / 4]->light_color[0].w;

				light_spec[j] = lights[j / 4]->light_color[1].x;
				light_spec[j + 1] = lights[j / 4]->light_color[1].y;
				light_spec[j + 2] = lights[j / 4]->light_color[1].z;
				light_spec[j + 3] = lights[j / 4]->light_color[1].w;

				light_pos[j] = lights[j / 4]->get_pos().x;
				light_pos[j + 1] = lights[j / 4]->get_pos().y;
				light_pos[j + 2] = lights[j / 4]->get_pos().z;
				light_pos[j + 3] = lights[j / 4]->get_pos().w;


			}
			glUniform1fv(location1, 4*lights.size(), light_ambient);
			glUniform1fv(location9, 4 * lights.size(), light_diff);
			glUniform1fv(location8, 4 * lights.size(), light_spec);
			glUniform1fv(location4, 4 * lights.size(), light_pos);
			glUniform1i(location6, phong);
			glUniform1i(location11, flat);


		}

	}
	// 2. Tell all models to draw themselves
	for (i = 0; i<models.size(); i++)
	{
		glUniform1i(location5, 0);
		models[i]->SetCameraTransform(cameras[ActiveCamera]->get_transformation());
		models[i]->SetProjection((cameras[ActiveCamera])->get_projection());
		models[i]->setWorldTransformation(Wscale,Wangle,Wtrans);
		drawAxis(models[i]->getTransform());
		drawAxis(models[i]->get_object_transform()); //Actually gets world transform//
		glUniform1i(location2, models[i]->shiness);

		
		if(lights.size()){
			vertices = (models[i])->Draw(lights,colorBuffer,zBuffer,width,height);
		}
		else
		{
			vertices = (models[i])->Draw(lights, colorBuffer, zBuffer, width, height);
		}
		glUniform4f(location3, (models[i]->model_colors[0]).x, (models[i]->model_colors[0]).y, (models[i]->model_colors[0]).z, (models[i]->model_colors[0]).w);
		glUniform4f(location7, (models[i]->model_colors[1]).x, (models[i]->model_colors[1]).y, (models[i]->model_colors[1]).z, (models[i]->model_colors[1]).w);
		glUniform1i(location10, lights.size());
		glUniform1i(location12, bloom);
		glUniform1i(location13, blur);
		glUniform1i(location14, add_texture);
		

		glDrawArrays(GL_TRIANGLES, 0, (*vertices).size());
			
		if (models[i]->box)
		{
			calculate_bounding_box(*vertices);
		}
		if (models[i]->there_is_normals)
		{

			normals = models[i]->get_normal();
			if (want_normals)
			{
				draw_normals(*vertices, *normals);
			}
			if (want_normals_per_face)
			{
				face_normals(*(models[i]->get_face_normal()), *(models[i]->get_center_points()));
			}
		}
		
	}
	if (lights.size())
	{
		for (int i = 0; i < lights.size(); i++)
		{
			lights[i]->draw_source();
		}
	}
	SwapBuffers();

}
void Scene:: calculate_bounding_box(vector<glm::vec3> vertices)
{
	float x_max, x_min, z_max, z_min, y_max, y_min;
	glm::vec3 v = (vertices)[0];

	x_max = v.x;

	x_min = v.x;

	y_max = v.y;

	y_min = v.y;

	z_max = v.z;

	z_min = v.z;
	
	for (int i = 0; i < vertices.size(); i++)

	{

		glm::vec3 v1 = (vertices)[i];

		//checking v1

		if (v1.x > x_max)

			x_max = v1.x;



		if (v1.y > y_max)

			y_max = v1.y;



		if (v1.z > z_max)

			z_max = v1.z;



		if (v1.x < x_min)

			x_min = v1.x;



		if (v1.y < y_min)

			y_min = v1.y;



		if (v1.z < z_min)

			z_min = v1.z;




	}
	
	vector<glm::vec3> vertex_data;
	vector<glm::vec3> new_vretices;
	vector<glm::vec3> box_color;
	vector<glm::vec3> nTriangles;

	


	new_vretices.push_back(glm::vec3{ x_min,y_min,z_max });//0
	new_vretices.push_back(glm::vec3{ x_max,y_min,z_max });//1
	new_vretices.push_back(glm::vec3{ x_min,y_max,z_max });//2
	new_vretices.push_back(glm::vec3{ x_max,y_max,z_max });//3
	new_vretices.push_back(glm::vec3{ x_min,y_max,z_min });//4
	new_vretices.push_back(glm::vec3{ x_max,y_max,z_min });//5
	new_vretices.push_back(glm::vec3{ x_min,y_min,z_min });//6
	new_vretices.push_back(glm::vec3{ x_max,y_min,z_min });//7

	// solid drawing
	
		

		vertex_data.push_back(new_vretices[0]);
		vertex_data.push_back(new_vretices[1]);
		vertex_data.push_back(new_vretices[2]);


		vertex_data.push_back(new_vretices[2]);
		vertex_data.push_back(new_vretices[1]);
		vertex_data.push_back(new_vretices[3]);

		vertex_data.push_back(new_vretices[2]);
		vertex_data.push_back(new_vretices[3]);
		vertex_data.push_back(new_vretices[4]);

		vertex_data.push_back(new_vretices[4]);
		vertex_data.push_back(new_vretices[3]);
		vertex_data.push_back(new_vretices[5]);

		vertex_data.push_back(new_vretices[4]);
		vertex_data.push_back(new_vretices[5]);
		vertex_data.push_back(new_vretices[6]);

		vertex_data.push_back(new_vretices[6]);
		vertex_data.push_back(new_vretices[5]);
		vertex_data.push_back(new_vretices[7]);

		vertex_data.push_back(new_vretices[6]);
		vertex_data.push_back(new_vretices[7]);
		vertex_data.push_back(new_vretices[0]);

		vertex_data.push_back(new_vretices[0]);
		vertex_data.push_back(new_vretices[7]);
		vertex_data.push_back(new_vretices[1]);

		vertex_data.push_back(new_vretices[1]);
		vertex_data.push_back(new_vretices[7]);
		vertex_data.push_back(new_vretices[3]);

		vertex_data.push_back(new_vretices[3]);
		vertex_data.push_back(new_vretices[7]);
		vertex_data.push_back(new_vretices[5]);

		vertex_data.push_back(new_vretices[6]);
		vertex_data.push_back(new_vretices[0]);
		vertex_data.push_back(new_vretices[4]);

		vertex_data.push_back(new_vretices[4]);
		vertex_data.push_back(new_vretices[0]);
		vertex_data.push_back(new_vretices[2]);


		for (int i = 0; i < 12; i++)
		{
			nTriangles.push_back(vertex_data[i * 3 + 1] - vertex_data[i * 3]);
			nTriangles.push_back(vertex_data[i * 3 + 2] - vertex_data[i * 3 + 1]);
			nTriangles.push_back(vertex_data[i * 3] - vertex_data[i * 3 + 2]);
		}


		for (int i = 0; i < vertex_data.size(); i++)
		{
			box_color.push_back(glm::vec3(1.0));
		}
		float* texturess = new float[vertex_data.size() * 2];
		for (int i = 0; i < 2 * vertex_data.size(); i++)
		{
			texturess[i] = 1.0;
		}
	
	//lines
	/*
	vertex_data.push_back(new_vretices[0]);
	vertex_data.push_back(new_vretices[1]);
	vertex_data.push_back(new_vretices[2]);
	vertex_data.push_back(new_vretices[0]);
	vertex_data.push_back(new_vretices[1]);
	vertex_data.push_back(new_vretices[2]);
	box_color.push_back(glm::vec3(1.0));
	box_color.push_back(glm::vec3(1.0));
	box_color.push_back(glm::vec3(1.0));
	box_color.push_back(glm::vec3(1.0));
	box_color.push_back(glm::vec3(1.0));
	box_color.push_back(glm::vec3(1.0));

	
	vertex_data.push_back(new_vretices[2]);
	vertex_data.push_back(new_vretices[1]);
	vertex_data.push_back(new_vretices[3]);
	vertex_data.push_back(new_vretices[2]);
	vertex_data.push_back(new_vretices[1]);
	vertex_data.push_back(new_vretices[3]);
	box_color.push_back(glm::vec3{1.0,0,0});
	box_color.push_back(glm::vec3{ 1.0,0,0 });
	box_color.push_back(glm::vec3{ 1.0,0,0 });
	box_color.push_back(glm::vec3{ 1.0,0,0 });
	box_color.push_back(glm::vec3{ 1.0,0,0 });
	box_color.push_back(glm::vec3{ 1.0,0,0 });


	vertex_data.push_back(new_vretices[2]);
	vertex_data.push_back(new_vretices[3]);
	vertex_data.push_back(new_vretices[4]);
	vertex_data.push_back(new_vretices[2]);
	vertex_data.push_back(new_vretices[3]);
	vertex_data.push_back(new_vretices[4]);
	box_color.push_back(glm::vec3{0,1.0,0 });
	box_color.push_back(glm::vec3{ 0,1.0,0 });
	box_color.push_back(glm::vec3{ 0,1.0,0 });
	box_color.push_back(glm::vec3{ 0,1.0,0 });
	box_color.push_back(glm::vec3{ 0,1.0,0 });
	box_color.push_back(glm::vec3{ 0,1.0,0 });


	vertex_data.push_back(new_vretices[4]);
	vertex_data.push_back(new_vretices[3]);
	vertex_data.push_back(new_vretices[5]);
	vertex_data.push_back(new_vretices[4]);
	vertex_data.push_back(new_vretices[3]);
	vertex_data.push_back(new_vretices[5]);
	box_color.push_back(glm::vec3{ 0,0,1.0 });
	box_color.push_back(glm::vec3{ 0,0,1.0 });
	box_color.push_back(glm::vec3{ 0,0,1.0 });
	box_color.push_back(glm::vec3{ 0,0,1.0 });
	box_color.push_back(glm::vec3{ 0,0,1.0 });
	box_color.push_back(glm::vec3{ 0,0,1.0 });


	vertex_data.push_back(new_vretices[4]);
	vertex_data.push_back(new_vretices[5]);
	vertex_data.push_back(new_vretices[6]);
	vertex_data.push_back(new_vretices[4]);
	vertex_data.push_back(new_vretices[5]);
	vertex_data.push_back(new_vretices[6]);
	box_color.push_back(glm::vec3{ 0,1.0,1.0 });
	box_color.push_back(glm::vec3{ 0,1.0,1.0 });
	box_color.push_back(glm::vec3{ 0,1.0,1.0 });
	box_color.push_back(glm::vec3{ 0,1.0,1.0 });
	box_color.push_back(glm::vec3{ 0,1.0,1.0 });
	box_color.push_back(glm::vec3{ 0,1.0,1.0 });


	vertex_data.push_back(new_vretices[6]);
	vertex_data.push_back(new_vretices[5]);
	vertex_data.push_back(new_vretices[7]);
	vertex_data.push_back(new_vretices[6]);
	vertex_data.push_back(new_vretices[5]);
	vertex_data.push_back(new_vretices[7]);
	box_color.push_back(glm::vec3{ 1.0,0,1.0 });
	box_color.push_back(glm::vec3{ 1.0,0,1.0 });
	box_color.push_back(glm::vec3{ 1.0,0,1.0 });
	box_color.push_back(glm::vec3{ 1.0,0,1.0 });
	box_color.push_back(glm::vec3{ 1.0,0,1.0 });
	box_color.push_back(glm::vec3{ 1.0,0,1.0 });

	vertex_data.push_back(new_vretices[6]);
	vertex_data.push_back(new_vretices[7]);
	vertex_data.push_back(new_vretices[0]);
	vertex_data.push_back(new_vretices[6]);
	vertex_data.push_back(new_vretices[7]);
	vertex_data.push_back(new_vretices[0]);
	box_color.push_back(glm::vec3{ 1.0,1.0,0 });
	box_color.push_back(glm::vec3{ 1.0,1.0,0 });
	box_color.push_back(glm::vec3{ 1.0,1.0,0 });
	box_color.push_back(glm::vec3{ 1.0,1.0,0 });
	box_color.push_back(glm::vec3{ 1.0,1.0,0 });
	box_color.push_back(glm::vec3{ 1.0,1.0,0 });
	box_color.push_back(glm::vec3{ 1.0,1.0,0 });

	vertex_data.push_back(new_vretices[0]);
	vertex_data.push_back(new_vretices[7]);
	vertex_data.push_back(new_vretices[1]);
	vertex_data.push_back(new_vretices[0]);
	vertex_data.push_back(new_vretices[7]);
	vertex_data.push_back(new_vretices[1]);
	box_color.push_back(glm::vec3{ 0.5,1.0,0.3 });
	box_color.push_back(glm::vec3{ 0.5,1.0,0.3 });
	box_color.push_back(glm::vec3{ 0.5,1.0,0.3 });
	box_color.push_back(glm::vec3{ 0.5,1.0,0.3 });
	box_color.push_back(glm::vec3{ 0.5,1.0,0.3 });
	box_color.push_back(glm::vec3{ 0.5,1.0,0.3 });

	vertex_data.push_back(new_vretices[1]);
	vertex_data.push_back(new_vretices[7]);
	vertex_data.push_back(new_vretices[3]);
	vertex_data.push_back(new_vretices[1]);
	vertex_data.push_back(new_vretices[7]);
	vertex_data.push_back(new_vretices[3]);
	box_color.push_back(glm::vec3{ 0.01,0.7,0.2 });
	box_color.push_back(glm::vec3{ 0.01,0.7,0.2 });
	box_color.push_back(glm::vec3{ 0.01,0.7,0.2 });
	box_color.push_back(glm::vec3{ 0.01,0.7,0.2 });
	box_color.push_back(glm::vec3{ 0.01,0.7,0.2 });
	box_color.push_back(glm::vec3{ 0.01,0.7,0.2 });

	vertex_data.push_back(new_vretices[3]);
	vertex_data.push_back(new_vretices[7]);
	vertex_data.push_back(new_vretices[5]);
	vertex_data.push_back(new_vretices[3]);
	vertex_data.push_back(new_vretices[7]);
	vertex_data.push_back(new_vretices[5]);
	box_color.push_back(glm::vec3{ 0.01,0.3,0.2 });
	box_color.push_back(glm::vec3{ 0.01,0.3,0.2 });
	box_color.push_back(glm::vec3{ 0.01,0.3,0.2 });
	box_color.push_back(glm::vec3{ 0.01,0.3,0.2 });
	box_color.push_back(glm::vec3{ 0.01,0.3,0.2 });
	box_color.push_back(glm::vec3{ 0.01,0.3,0.2 });

	vertex_data.push_back(new_vretices[6]);
	vertex_data.push_back(new_vretices[0]);
	vertex_data.push_back(new_vretices[4]);
	vertex_data.push_back(new_vretices[6]);
	vertex_data.push_back(new_vretices[0]);
	vertex_data.push_back(new_vretices[4]);
	box_color.push_back(glm::vec3{ 0.7,0.3,0.2 });
	box_color.push_back(glm::vec3{ 0.7,0.3,0.2 });
	box_color.push_back(glm::vec3{ 0.7,0.3,0.2 });
	box_color.push_back(glm::vec3{ 0.7,0.3,0.2 });
	box_color.push_back(glm::vec3{ 0.7,0.3,0.2 });
	box_color.push_back(glm::vec3{ 0.7,0.3,0.2 });

	vertex_data.push_back(new_vretices[4]);
	vertex_data.push_back(new_vretices[0]);
	vertex_data.push_back(new_vretices[2]);
	vertex_data.push_back(new_vretices[4]);
	vertex_data.push_back(new_vretices[0]);
	vertex_data.push_back(new_vretices[2]);
	box_color.push_back(glm::vec3{ 0.7,0.3,0.9 });
	box_color.push_back(glm::vec3{ 0.7,0.3,0.9 });
	box_color.push_back(glm::vec3{ 0.7,0.3,0.9 });
	box_color.push_back(glm::vec3{ 0.7,0.3,0.9 });
	box_color.push_back(glm::vec3{ 0.7,0.3,0.9 });
	box_color.push_back(glm::vec3{ 0.7,0.3,0.9 });
	

	vector<glm::vec3> nTriangles;
	for (int i = 0; i < 24; i++)
	{
		nTriangles.push_back(vertex_data[i * 3 + 1] - vertex_data[i * 3]);
		nTriangles.push_back(vertex_data[i * 3 + 2] - vertex_data[i * 3 + 1]);
		nTriangles.push_back(vertex_data[i * 3] - vertex_data[i * 3 + 2]);
	}


	float* texturess = new float[vertex_data.size() * 2];
	for (int i = 0; i < 2 * vertex_data.size(); i++)
	{
		texturess[i] = 1.0;
	}*/

	unsigned int buffer;
	glGenVertexArrays(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, (vertex_data.size() + box_color.size() + nTriangles.size()) * sizeof(glm::vec3) + sizeof(texturess), 0, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_data.size() * sizeof(glm::vec3), &vertex_data[0]);
	glBufferSubData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(glm::vec3), sizeof(texturess), &texturess[0]);
	glBufferSubData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(glm::vec3) + sizeof(texturess), box_color.size() * sizeof(glm::vec3), &box_color[0]);
	glBufferSubData(GL_ARRAY_BUFFER, (vertex_data.size() + box_color.size()) * sizeof(glm::vec3) + sizeof(texturess), nTriangles.size() * sizeof(glm::vec3), &nTriangles[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid *)(vertex_data.size() * sizeof(glm::vec3)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid *)((vertex_data.size()) * sizeof(glm::vec3) + sizeof(texturess)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid *)((vertex_data.size() + box_color.size()) * sizeof(glm::vec3) + sizeof(texturess)));
	glDrawArrays(GL_TRIANGLES, 0, (vertex_data).size());
	

}
void Scene::draw_normals(vector<glm::vec3> normals, vector<glm::vec3>vertices)
{
	vector<glm::vec3> to_draw;
	for (int i = 0; i < vertices.size(); i++)
	{
		to_draw.push_back(vertices[i]);
		to_draw.push_back( normals[i]);
	}

	vector<glm::vec3> the_color;
	for (int i = 0; i < 2*vertices.size(); i++)
	{
		the_color.push_back(normalColor);
	}

	vector<glm::vec3> the_normals;
	for (int i = 0; i < vertices.size(); i++)
	{
		the_normals.push_back(normals[i]);
		the_normals.push_back(glm::normalize(vertices[i] + normals[i]));
	}

	float* the_texture = new float[vertices.size() * 4];
	for (int i = 0; i < vertices.size() * 4; i++)
	{
		the_texture[i] = 1.0;
	}

	unsigned int buffer;
	glGenVertexArrays(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, (to_draw.size() + the_color.size() + the_normals.size()) * sizeof(glm::vec3) + sizeof(the_texture), 0, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, to_draw.size() * sizeof(glm::vec3), &to_draw[0]);
	glBufferSubData(GL_ARRAY_BUFFER, to_draw.size() * sizeof(glm::vec3), sizeof(the_texture), &the_texture[0]);
	glBufferSubData(GL_ARRAY_BUFFER, to_draw.size() * sizeof(glm::vec3) + sizeof(the_texture), the_color.size() * sizeof(glm::vec3), &the_color[0]);
	glBufferSubData(GL_ARRAY_BUFFER, (to_draw.size() + the_color.size()) * sizeof(glm::vec3) + sizeof(the_texture), the_normals.size() * sizeof(glm::vec3), &the_normals[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid *)(to_draw.size() * sizeof(glm::vec3)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid *)((to_draw.size()) * sizeof(glm::vec3) + sizeof(the_texture)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid *)((to_draw.size() + the_color.size()) * sizeof(glm::vec3) + sizeof(the_texture)));
	glDrawArrays(GL_LINES, 0, (to_draw).size());


}
void Scene::face_normals(vector<glm::vec3> face_normals, vector<glm::vec3>centerals)
{
	vector<glm::vec3> to_draw;
	for (int i = 0; i < centerals.size(); i++)
	{
		to_draw.push_back(centerals[i]);
		to_draw.push_back(face_normals[i]);
	}

	vector<glm::vec3> the_color;
	for (int i = 0; i < 2 * centerals.size(); i++)
	{
		the_color.push_back(normalColor);
	}

	vector<glm::vec3> the_normals;
	for (int i = 0; i < centerals.size(); i++)
	{
		the_normals.push_back(face_normals[i]);
		the_normals.push_back(glm::normalize(centerals[i] + face_normals[i]));
	}

	float* the_texture = new float[centerals.size() * 4];
	for (int i = 0; i < centerals.size() * 4; i++)
	{
		the_texture[i] = 1.0;
	}

	unsigned int buffer;
	glGenVertexArrays(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, (to_draw.size() + the_color.size() + the_normals.size()) * sizeof(glm::vec3) + sizeof(the_texture), 0, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, to_draw.size() * sizeof(glm::vec3), &to_draw[0]);
	glBufferSubData(GL_ARRAY_BUFFER, to_draw.size() * sizeof(glm::vec3), sizeof(the_texture), &the_texture[0]);
	glBufferSubData(GL_ARRAY_BUFFER, to_draw.size() * sizeof(glm::vec3) + sizeof(the_texture), the_color.size() * sizeof(glm::vec3), &the_color[0]);
	glBufferSubData(GL_ARRAY_BUFFER, (to_draw.size() + the_color.size()) * sizeof(glm::vec3) + sizeof(the_texture), the_normals.size() * sizeof(glm::vec3), &the_normals[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid *)(to_draw.size() * sizeof(glm::vec3)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid *)((to_draw.size()) * sizeof(glm::vec3) + sizeof(the_texture)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid *)((to_draw.size() + the_color.size()) * sizeof(glm::vec3) + sizeof(the_texture)));
	glDrawArrays(GL_LINES, 0, (to_draw).size());
}

void Scene::DrawDemo()
{
	SetDemoBuffer();
	SwapBuffers();
}

void Scene::ChangeProjection(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar, bool isOrtho)
{
	if (isOrtho) cameras[ActiveCamera]->Ortho(left, right, bottom, top, zNear, zFar);
	else cameras[ActiveCamera]->Frustum(left, right, bottom, top, zNear, zFar);
	return;
}

void Scene::ChangeTransform(const glm::vec4& eye, const glm::vec4& at, const glm::vec4& up) {
	cameras[ActiveCamera]->LookAt(eye, at, up);
}

void Scene::AddCamera()
{
	Camera* cam = new Camera();
	cameras.push_back(cam);
	numberOfCameras++;
	ActiveCamera = numberOfCameras - 1;
}

void Scene::AddCamera(Camera& camera)
{
	Camera* cam = new Camera(camera);
	cameras.push_back(cam);
	numberOfCameras++;
	ActiveCamera = numberOfCameras - 1;
}

void Scene::setActiveCamera(int index) {
	if (index <= 0 || index >= numberOfCameras) return;
	ActiveCamera = index;
}

void Scene::removeCamera(int index)
{
	if (numberOfCameras == 1) return;
	delete cameras[index];
	cameras.erase(cameras.begin() + index);
	numberOfCameras--;

	ActiveCamera = 0;
}

Camera* Scene::getActiveCamera() {
	return cameras[ActiveCamera];
}

Model * Scene::getActiveModel()
{
	if (!models.size()) return NULL;
	return models[ActiveModel-1]; //+-1 or [ActiveModel], potential bug
}

int Scene::getNumOfModels()
{
	return (int)models.size();
}

void Scene::setActiveModel(int active)
{
	ActiveModel = active;
}

void Scene::remove_module(int index)
{
	models.erase(models.begin() + index);
}

Light* Scene::getActiveLight()
{
	if (!lights.size())
		return NULL;
	return lights[ActiveLight ];
}

void Scene::AddLight(string type)
{
	Light* new_light;
	if (type == "point")
	{
		new_light = new PointLight();
	}

	if (type == "paralel")
	{
		new_light = new ParallelLight();
	}

	lights.push_back(new_light);
	
	numberOfLights++;
	ActiveLight = numberOfLights - 1;
}
