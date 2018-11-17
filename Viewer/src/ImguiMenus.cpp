#pragma once
#include "ImguiMenus.h"
#include <stdio.h>
#include <stdlib.h>
#include<iostream>
// open file dialog cross platform https://github.com/mlabbe/nativefiledialog
#include <nfd.h>

bool showDemoWindow = false;
bool showAnotherWindow = false;
bool showFile = false;
bool ortho = false;
char * textureName = "textures/egg.png";

glm::vec4 Active_light_Color = glm::vec4(1, 1, 1, 1);
glm::vec4 ambient_percentages = glm::vec4(1, 1, 1, 1);

glm::vec4 clearColor = glm::vec4(0.55f, 0.55f, 0.55f, 1.00f);
glm::vec4 meshColor = glm::vec4(1, 1, 1, 1);
glm::vec4 normalColor = glm::vec4(0, 1, 0, 1);


int counter = 0;
bool phong = 0;
bool gouraud = 1;
bool flat = 0;
bool uniform=1;
bool anti_aliasing = 0;
bool bloom = 0;
bool blur = 0;
bool add_texture = 0;

bool want_normals = 0;
bool want_lines = 0;
bool want_normals_per_face = 0;
int num_of_prims=0;
float temp;
const glm::vec4& GetClearColor()
{
	return clearColor;
}

void DrawImguiMenus(ImGuiIO& io, Scene* scene)
{
	ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Once);
	// 1. Show a simple window.
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
	{
		ImGui::Begin("Test Menu");
		ImGui::Text("Hello, world!");                     // Display some text (you can use a format string too)
	if(scene->ActiveModel!=0){
		ImGui::SliderFloat3("Scale - Model", (float*)&scene->getActiveModel()->scale, 0.0f, 5.0f); // ImGui::SameLine(); if (ImGui::Button("Reset scale")) resetScale();
		ImGui::SliderFloat3("Rotation - Model", (float*)&scene->getActiveModel()->angle, 0.0f, 180); // ImGui::SameLine(); if (ImGui::Button("Reset rotation")) resetScale(); ///////BUG
		ImGui::SliderFloat3("Object's Translation", (float*)&scene->getActiveModel()->tran, -1.0, 1.0);
		ImGui::Checkbox("Bounding Box", &scene->getActiveModel()->box);

		//		ImGui::SameLine(); if (ImGui::Button("Reset Translation")) resetTranslation();
	}
		ImGui::SliderFloat("Uniform scaling", &temp, 0.0, 5.0); ImGui::SameLine();
		if (ImGui::Button("Use Uniform scaling")) scene->Wscale = glm::vec3(temp);
		ImGui::SliderFloat3("Scale - World", (float*)&scene->Wscale, 0.0f, 5.0f); //ImGui::SameLine(); if (ImGui::Button("Reset")) resetScale();
		ImGui::SliderFloat3("World's rotations", (float*)&scene->Wangle, 0.0f, 180.0f);
		ImGui::SliderFloat3("World's Translation", (float*)&scene->Wtrans, -1.0, 1.0);
//		ImGui::SameLine(); if (ImGui::Button("Reset Translation")) resetTranslation();

//		if (ImGui::Button("Reset ALL Values")) ResetValues();

		ImGui::ColorEdit3("clear color", (float*)&clearColor); // Edit 3 floats representing a color
		ImGui::ColorEdit3("mesh color", (float*)&meshColor); // Edit 3 floats representing a color
		ImGui::ColorEdit3("normal color", (float*)&normalColor); // Edit 3 floats representing a color
		if (scene->ActiveModel != 0) 
		{
			ImGui::ColorEdit3("model diffuse color", (float*)&(scene->getActiveModel()->model_colors[0])); // Edit 3 floats representing a color
			ImGui::ColorEdit3("model specular color", (float*)&(scene->getActiveModel()->model_colors[1])); // Edit 3 floats representing a color
			ImGui::ColorEdit3("model ambient color", (float*)&(scene->getActiveModel()->model_colors[2]));  // Edit 3 floats representing a color
			ImGui::SliderInt("shiness", &scene->getActiveModel()->shiness, 1, 10);

		}

		ImGui::Checkbox("Demo Window", &showDemoWindow);      // Edit bools storing our windows open/close state
		ImGui::Checkbox("Another Window", &showAnotherWindow);
		/*
		if (ImGui::Button("remove Model"))
		{
			scene->remove_module((scene->ActiveModel)-1);
			if (counter)
			{
				counter--;
				scene->ActiveModel = counter;
			}
		}*/
		ImGui::Checkbox("vertex normals", &want_normals);
		ImGui::Checkbox("draw only lines", &want_lines);
		ImGui::Checkbox("face normals", &want_normals_per_face);
		ImGui::Checkbox("add anti-aliasing effect", &anti_aliasing);
		ImGui::Checkbox("uniform color", &uniform);
		ImGui::Checkbox("add texture", &add_texture);
		ImGui::Checkbox("bloom", &bloom);
		


		


		if (ImGui::Button("add primitive"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
		{
			num_of_prims++;
			scene->LoadPrimitiveModel();
			//scene->setActiveModel(num_of_prims);
			
		}
		ImGui::SameLine();
		ImGui::Text("sum = %d", num_of_prims);

		//////


		if (ImGui::Button("Button"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
		{
			if (counter < scene->getNumOfModels())
			{
				counter++;
				scene->setActiveModel(counter);
			}
		}
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::SameLine();
		if (ImGui::Button("Clear"))
		{
			counter = 1;
			scene->ActiveModel = counter;
		}

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}
	
		ImGui::Begin("Lights");
	{	
		if (ImGui::Button("Add Point source light"))
		{
			scene->AddLight("point");
		}
		if (ImGui::Button("Add paralel source light"))
		{
			scene->AddLight("paralel");
		}
		ImGui::Text("Active Light src: %d \n", scene->ActiveLight);
		ImGui::Text("num of Light src's: %d \n", scene->numberOfLights);

		if (scene->getActiveLight() != 0)
		{
			ImGui::ColorEdit3("Light diffuse color", (float*)&(scene->getActiveLight()->light_color[0])); // Edit 3 floats representing a color
			ImGui::ColorEdit3("Light specular color", (float*)&(scene->getActiveLight()->light_color[1])); // Edit 3 floats representing a color
			ImGui::ColorEdit3("Light ambient color", (float*)&(scene->getActiveLight()->light_color[2]));  // Edit 3 floats representing a color

		}

		

		if (scene->numberOfLights != 0) {
			ImGui::SliderFloat3("Scale", (float*)&scene->getActiveLight()->scale, 0.0f, 10.0f); // ImGui::SameLine(); if (ImGui::Button("Reset scale")) resetScale();
			ImGui::SliderFloat3("Rotation", (float*)&scene->getActiveLight()->angle, 0.0f, 180); // ImGui::SameLine(); if (ImGui::Button("Reset rotation")) resetScale(); ///////BUG
			ImGui::SliderFloat3("Translation", (float*)&scene->getActiveLight()->tran, -1000.0, 1000.000);
		}
		if (ImGui::Button("back"))
		{
			if (scene->ActiveLight != 0)
			{
				scene->ActiveLight--;
				
			}
		}
		if (ImGui::Button("next"))
		{
			if (scene->ActiveLight < scene->numberOfLights - 1)
			{
				scene->ActiveLight++;

			}
		}
		if (ImGui::Checkbox(" flat shading ", &flat))
		{
			gouraud = 0;
			phong = 0;
		}
		if (ImGui::Checkbox(" gouraud shading ", &gouraud))
		{
			flat = 0;
			phong = 0;
		}
		if (ImGui::Checkbox(" phong shading ", &phong))
		{
			flat = 0;
			gouraud = 0;
		}


		ImGui::End();
	}

	ImGui::Begin("Cameras");
	ImGui::Text("Active Camera: %d \nOut of: %d cameras", scene->ActiveCamera, scene->numberOfCameras - 1);
	if (ImGui::Button("Add Camera")) scene->AddCamera();
	if (ImGui::Button("Delete Current Camera")) scene->removeCamera(scene->ActiveCamera);
	ImGui::InputInt("Active Camera", &scene->ActiveCamera);
	if (scene->ActiveCamera >= scene->numberOfCameras) scene->ActiveCamera--;
	if (scene->ActiveCamera == -1) scene->ActiveCamera = 0;
	ImGui::SliderFloat3("eye", (float*)&scene->getActiveCamera()->eyeC, -5.0, 5.0);

	ImGui::SameLine(); if (ImGui::Button("Reset eye")) 
		scene->getActiveCamera()->eyeC = glm::vec3(1, 1, 1);

//	if (ImGui::Button("Focus")) scene->getActiveCamera()->focus(scene->getActiveModel()->tran);

	ImGui::SliderFloat3("at", (float*)&scene->getActiveCamera()->att, -5.0, 5.0);
	ImGui::SameLine(); if (ImGui::Button("Reset at")) 
		scene->getActiveCamera()->att = glm::vec3(0, 0, 0);


	ImGui::SliderFloat3("up", (float*)&scene->getActiveCamera()->up, -1.0, 1.0);
	ImGui::SameLine(); if (ImGui::Button("Reset up")) 	
		scene->getActiveCamera()->up = glm::vec3(0, 1, 0);


	ImGui::SliderFloat("top", &scene->getActiveCamera()->upp, -5.0f, 5.0f); ImGui::SameLine(); if (ImGui::Button("Reset top"))
	{
		scene->getActiveCamera()->upp = 1;
	}
	ImGui::SliderFloat("bottom", &scene->getActiveCamera()->bottom, -5.0f, 5.0f); ImGui::SameLine(); if (ImGui::Button("Reset bottom")) {
		scene->getActiveCamera()->bottom = 0;;
	}
	ImGui::SliderFloat("right", &scene->getActiveCamera()->right, -5.0f, 5.0f); ImGui::SameLine(); if (ImGui::Button("Reset right"))
	{
		scene->getActiveCamera()->right = 1;
	}
	ImGui::SliderFloat("left", &scene->getActiveCamera()->left, -5.0f, 5.0f); ImGui::SameLine(); if (ImGui::Button("Reset left"))
	{
		scene->getActiveCamera()->left = -1;
	}
	ImGui::SliderFloat("zNear", &scene->getActiveCamera()->zNear, 0.0f, 5.0f); ImGui::SameLine(); if (ImGui::Button("Reset zNear")) {
		scene->getActiveCamera()->zNear=1;
	}
	ImGui::SliderFloat("zFar", &scene->getActiveCamera()->zFar, 0.0f, 5.0f); ImGui::SameLine(); if (ImGui::Button("Reset zFar"))
	{
		scene->getActiveCamera()->zFar = 100000;
	}
	if (ImGui::Checkbox("Orthoginal?", &ortho)) { scene->getActiveCamera()->zFar = 0.9; scene->getActiveCamera()->zNear = 1;} //Ramma2ot
	// if (ImGui::Button("Apply")) {
	scene->ChangeProjection(scene->getActiveCamera()->left
		, scene->getActiveCamera()->right
		, scene->getActiveCamera()->bottom
		, scene->getActiveCamera()->upp
		, scene->getActiveCamera()->zNear
		, scene->getActiveCamera()->zFar, ortho);
	scene->ChangeTransform(glm::vec4(scene->getActiveCamera()->eyeC,0),
		glm::vec4(scene->getActiveCamera()->att, 0),
		glm::vec4(scene->getActiveCamera()->up, 0));

	// }
	ImGui::End();
	// 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name your windows.
	if (showAnotherWindow)
	{
		int val[2]; val[0] = io.MousePos.x; val[1] = io.MousePos.y;
		ImGui::Begin("Another Window", &showAnotherWindow);
		ImGui::InputInt2("(x,y)", val, 3);
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			showAnotherWindow = false;
		ImGui::End();
	}
	
	

	// 3. Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
	if (showDemoWindow)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
		ImGui::ShowDemoWindow(&showDemoWindow);
	}

	// Demonstrate creating a fullscreen menu bar and populating it.
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoFocusOnAppearing;
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open", "CTRL+O"))
				{
					nfdchar_t *outPath = NULL;
					//NOT SURE, MAY FIX
					nfdresult_t result = NFD_OpenDialog("obj;png,jpg", NULL, &outPath);
					if (result == NFD_OKAY) {
						// ImGui::Text("Hello from another window!");
						scene->LoadOBJModel(outPath);
						free(outPath);
					}
					else if (result == NFD_CANCEL) {
					}
					else {
					}

				}
				if (ImGui::MenuItem("Change texture file")) {
					nfdchar_t *outPath = NULL;
					//NOT SURE, MAY FIX
					nfdresult_t result = NFD_OpenDialog("png,jpg;obj", NULL, &outPath);
					if (result == NFD_OKAY) {
						// ImGui::Text("Hello from another window!");
						textureName = outPath;
						free(outPath);
						std::cout << "tis a: " << textureName << std::endl;
					}
					else if (result == NFD_CANCEL) {
					}
					else {
					}
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
				if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
				ImGui::Separator();
				if (ImGui::MenuItem("Cut", "CTRL+X")) {}
				if (ImGui::MenuItem("Copy", "CTRL+C")) {}
				if (ImGui::MenuItem("Paste", "CTRL+V")) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("Show Demo Menu")) { showDemoWindow = true; }
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}
}
