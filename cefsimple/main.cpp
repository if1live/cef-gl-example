#include <stdlib.h>
#include <stdio.h>

#include <string>
#include <algorithm>
#include <cassert>

// CEF
#include <include/cef_browser.h>
#include <include/cef_app.h>

// GL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "gl_core.h"

#include "render_handler.h"
#include "browser_client.h"


static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

void mouse_callback(GLFWwindow* window, int btn, int state, int mods)
{
	// send mouse click to browser
	//cefgui->mouseClick(btn, GLFW_PRESS);
	//cefgui->mouseClick(btn, GLFW_RELEASE);
}

void motion_callback(GLFWwindow* window, double x, double y)
{
	// send mouse movement to browser
	//cefgui->mouseMove((int)x, (int)y);
}

GLFWwindow *initialize_glfw_window(int w, int h)
{
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	GLFWwindow* window = glfwCreateWindow(w, h, "CEF + OpenGL", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	return window;
}

void shutdown_glfw(GLFWwindow *window)
{
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

bool initialize_glew_context()
{
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		// Problem: glewInit failed, something is seriously wrong
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
		return false;
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	return true;
}

bool is_main_process(int argc, char *argv[]) 
{
	for (int i = 0; i < argc; i++) 
	{
		std::string arg(argv[i]);
		auto found = arg.find("--channel");
		if (found != std::string::npos) {
			return false;
		}
	}
	return true;
}

void reshape_callback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}


GLint pos_loc = -1;
GLint texcoord_loc = -1;
GLint tex_loc = -1;
GLint mvp_loc = -1;


int main(int argc, char *argv[])
{
	bool main_process = is_main_process(argc, argv);

	int width = 640;
	int height = 480;

	GLFWwindow *window = nullptr;
	if(main_process) 
	{
		// create GL context
		window = initialize_glfw_window(width, height);

		// initialize glew context
		initialize_glew_context();
	}
	
	CefMainArgs args;
	int exit_code = CefExecuteProcess(args, nullptr, nullptr);
	if (exit_code >= 0) { return exit_code; }
	
	CefSettings settings;
	bool result = CefInitialize(args, settings, nullptr, nullptr);
	if (!result) {
		return -1;
	}

	RenderHandler *renderHandler = new RenderHandler();
	renderHandler->init();
	renderHandler->resize(1024, 1024);

	// create browser-window
	CefRefPtr<CefBrowser> browser;
	CefRefPtr<BrowserClient> browserClient;

	CefWindowInfo window_info;
	window_info.SetAsWindowless(nullptr, true);

	CefBrowserSettings browserSettings;
	// browserSettings.windowless_frame_rate = 60; // 30 is default

	browserClient = new BrowserClient(renderHandler);

	//std::string url = "http://deanm.github.io/pre3d/monster.html";
	//std::string url = "https://gae9.com";
	std::string url = "http://google.com";
	browser = CefBrowserHost::CreateBrowserSync(window_info, browserClient.get(), url, browserSettings, nullptr);

	// inject user-input by calling - non-trivial for non-windows - checkout the cefclient source and the platform specific cpp, like cefclient_osr_widget_gtk.cpp for linux
	// browser->GetHost()->SendKeyEvent(...);
	// browser->GetHost()->SendMouseMoveEvent(...);
	// browser->GetHost()->SendMouseClickEvent(...);
	// browser->GetHost()->SendMouseWheelEvent(...);

	// setup glfw
	glfwSwapInterval(1);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, motion_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);
	glfwSetFramebufferSizeCallback(window, reshape_callback);

	// shader
	GLuint prog = 0;
	if (window) 
	{
		prog = GLCore::createShaderProgram("shaders/tex.vert", "shaders/tex.frag");
		assert(prog != 0 && "shader compile failed");

		pos_loc = glGetAttribLocation(prog, "a_position");
		texcoord_loc = glGetAttribLocation(prog, "a_texcoord");
		tex_loc = glGetUniformLocation(prog, "s_tex");
		mvp_loc = glGetUniformLocation(prog, "u_mvp");
	}

	// initial GL state
	glViewport(0, 0, width, height);
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);

	// 2 3
	// 0 1
	float vertices[] = {
		-1, -1, 0,
		1, -1, 0,
		-1, 1, 0,
		1, 1, 0,
	};

	float texcoords[] = {
		0, 1,
		1, 1,
		0, 0,
		1, 0,
	};

	unsigned short indices[] = {
		0, 1, 3,
		0, 3, 2,
	};

	while (true)
	{
		if (window) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram(prog);

			glm::mat4 mvp = glm::ortho(-1, 1, -1, 1);
			//mvp *= glm::rotate((float)glfwGetTime() * 10.f, glm::vec3(0, 0, 1));
			glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp));

			glEnableVertexAttribArray(pos_loc);
			glEnableVertexAttribArray(texcoord_loc);

			// bind texture
			glBindTexture(GL_TEXTURE_2D, renderHandler->tex());
			glUniform1i(tex_loc, 0);

			// draw
			glVertexAttribPointer(pos_loc, 3, GL_FLOAT, GL_FALSE, 0, vertices);
			glVertexAttribPointer(texcoord_loc, 2, GL_FLOAT, GL_FALSE, 0, texcoords);
			glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_SHORT, indices);

			glfwSwapBuffers(window);

			// update
			glfwPollEvents();

			// cef
			CefDoMessageLoopWork();
		}
		else 
		{
			// cef
			CefDoMessageLoopWork();
			Sleep(50);
		}

		if (glfwWindowShouldClose(window)) 
		{
			break;
		}
	}

	// close cef
	browser->GetHost()->CloseBrowser(true);
	CefDoMessageLoopWork();

	browser = nullptr;
	browserClient = nullptr;
	CefShutdown();

	if (prog)
	{
		GLCore::deleteProgram(prog);
	}
	if (window) 
	{	
		shutdown_glfw(window);
	}

	return 0;  
}