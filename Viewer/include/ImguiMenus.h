#pragma once
#include <imgui/imgui.h>
#include "Scene.h"

#define PI 3.14159265359

void DrawImguiMenus(ImGuiIO& io, Scene* scene);
const glm::vec4& GetClearColor();
extern glm::vec4 meshColor;
extern glm::vec4 normalColor;
extern glm::vec4 model_colors[3];
extern bool want_normals;
extern bool want_normals_per_face;
extern bool flat;
extern bool gouraud;
extern int num_of_prims;
extern int counter;
extern bool phong;
extern bool flat;
extern bool gouraud;
extern bool uniform;
extern bool want_lines;
extern bool per_fragment;
extern bool per_vertex;
extern bool anti_aliasing;
extern bool bloom;
extern bool blur;
extern bool add_texture;
extern char* textureName;

extern glm::vec4 Active_light_Color;
extern glm::vec4 ambient_percentages;



