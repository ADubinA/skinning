#include "Menu.h"



Menu::Menu(Display *display, Game *scn)
{
	this->display = display;
	this->scn = scn;
	this->created = false;
	this->move = false;
	this-> f = 0.0f;


	show_demo_window = true;
	show_another_window = false;
	clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
}

void Menu::create()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.FontGlobalScale = 4.0f;

	ImGui_ImplGlfwGL3_Init(display->get_window(), true);
	ImGui::StyleColorsDark();
	this->created = true;

}

void Menu::DrawMenuScore()
{
	ImGui_ImplGlfwGL3_NewFrame();

	
	ImGui::Begin("Score Window");                          // Create a window called "Hello, world!" and append into it.
	ImGui::SetWindowSize("Score Window", ImVec2((float)1400, (float)800));

	//ImGui::Text("picked shape is: %d", scn->pickedShape);
	//ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
	//ImGui::Checkbox("Another Window", &show_another_window);

	//ImGui::SliderFloat("velocity", &this->f, -5.0f, 5.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	

	/*if (ImGui::Button("Pause")) 
	{
		if (scn->IsActive())
		{
			scn->Deactivate();
		}
		else
		{
			scn->Activate();
		}
	}  */ 
	// Buttons return true when clicked (most widgets return true when edited/activated)
		//scn->tmp_test_mode = !scn->tmp_test_mode;
		
	//ImGui::SameLine();
	if (scn->IsActive()) {
		ImGui::Text("your score is: %d", scn->score);
		ImGui::Text(".................................");
		switch( scn->score) {
		case 0:
			ImGui::Text("eat uranus (turquoise)");
			break;
		case 1:
			ImGui::Text("eat neptune (blue)");
			break;
		case 2:
			ImGui::Text("eat saturn, look for the rings on the planet!");
			break;
		case 3:
			ImGui::Text("eat jupiter, it has an eye!");
			break;
		case 4:
			ImGui::Text("eat venus, she is a lady ;)");
			break;
		case 5:
			ImGui::Text("eat earth, come home?");
			break;
		case 6:
			ImGui::Text("eat mars, where the aliens are");
			break;
		case 7:
			ImGui::Text("eat mercury, you are almost there");
			break;
		case 8:
			ImGui::Text("eat the sun");
			break;
		case 9:
			ImGui::Text("YOU WON!!!");
			break;
		}
	}
	else
		ImGui::Text("GAME OVER");

	//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
}

void Menu::DrawMenuStart()
{
	
	ImGui_ImplGlfwGL3_NewFrame();


	ImGui::Begin("Menu Window");                          // Create a window called "Hello, world!" and append into it.
	ImGui::SetWindowSize("Menu Window", ImVec2((float)1400, (float)800));

	//ImGui::Text("picked shape is: %d", scn->pickedShape);
	//ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
	//ImGui::Checkbox("Another Window", &show_another_window);

	//ImGui::SliderFloat("velocity", &this->f, -5.0f, 5.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

	if (ImGui::Button("Start game")) {

		//scn->Init();
		scn->Activate();
		scn->isStarted = true;

	}
	
	
	// Buttons return true when clicked (most widgets return true when edited/activated)
	//scn->tmp_test_mode = !scn->tmp_test_mode;

	//ImGui::SameLine();
	

	//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
}

void Menu::destroy()
{
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	this->created = false;

}


