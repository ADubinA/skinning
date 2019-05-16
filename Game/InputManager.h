#pragma once   //maybe should be static class
#include "display.h"
#include "game.h"


	void mouse_callback(GLFWwindow* window,int button, int action, int mods)
	{	
		if(action == GLFW_PRESS )
		{
			Game *scn = (Game*)glfwGetWindowUserPointer(window);
			double x2,y2;
			glfwGetCursorPos(window,&x2,&y2);
			scn->picking((int)x2,(int)y2);
		}
	}
	
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		Game *scn = (Game*)glfwGetWindowUserPointer(window);
		scn->shapeTransformation(scn->zCameraTranslate,yoffset);
		
	}
	
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		Game *scn = (Game*)glfwGetWindowUserPointer(window);

		if(action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			switch (key)
			{
				case GLFW_KEY_ESCAPE:			
					glfwSetWindowShouldClose(window,GLFW_TRUE);
				break;
				case GLFW_KEY_SPACE:
					scn->HideShape(0);
					if(scn->IsActive())
						scn->Deactivate();
					else
						scn->Deactivate();
				break;
					case GLFW_KEY_RIGHT:
					//scn->shapeTransformation(scn->zGlobalRotate,-20.1f);
						scn->shapeTransformation(scn->xGlobalTranslate,0.1);
						//scn->shapeTransformation(scn->yLocalTranslate,0.1);
						//scn->shapeTransformation(scn->zLocalTranslate,0.1);
					break;
				case GLFW_KEY_LEFT:
					scn->shapeTransformation(scn->xGlobalTranslate, -0.1);

					break;
				case GLFW_KEY_UP:
					scn->shapeTransformation(scn->yGlobalTranslate, 0.1);
					break;
				case GLFW_KEY_DOWN:
					scn->shapeTransformation(scn->yGlobalTranslate, -0.1);
					break;
				case GLFW_KEY_S:
					scn->tmp_test_mode = !scn->tmp_test_mode;
					break;
			default:
				break;
			}
		}
	}
	

	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
		Game *scn = (Game*)glfwGetWindowUserPointer(window);

		scn->updatePosition((float)xpos,(float)ypos);
		if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			scn->mouseProccessing(GLFW_MOUSE_BUTTON_RIGHT);
		}
		else if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			scn->mouseProccessing(GLFW_MOUSE_BUTTON_LEFT);
		}

	}

	void window_size_callback(GLFWwindow* window, int width, int height)
	{
		Game *scn = (Game*)glfwGetWindowUserPointer(window);

		scn->resize(width,height);
		//relation = (float)width/(float)height;
	}

	void init(Display &display)
	{
		display.addKeyCallBack(key_callback);
		display.addMouseCallBacks(mouse_callback,scroll_callback,cursor_position_callback);
		display.addResizeCallBack(window_size_callback);

	}
