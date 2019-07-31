#pragma once
#include "display.h"
#include "imgui\imgui.h"
#include "imgui\imgui_impl_glfw_gl3.h"
#include "scene.h"
#include "game.h"

class Menu
{
public:
	
	Display *display;
	Game *scn;
	bool show_demo_window;
	bool show_another_window;
	bool move ;
	bool created;
	float f;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	Menu(Display *display, Game *scn);
	void create();
	void DrawMenuScore();
	void DrawMenuStart();
	void destroy();
	
};

