#include <stdlib.h>
#include <stdio.h>

#include <string>
#include <algorithm>

// CEF
#include <include/cef_browser.h>
#include <include/cef_app.h>
#include <include/cef_render_handler.h>
#include <include/cef_client.h>

// GL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class RenderHandler : public CefRenderHandler
{
public:
	RenderHandler(int w, int h) : width_(w), height_(h), tex_(0) {}

public:
	void init()
	{
		glGenTextures(1, &tex_);
		glBindTexture(GL_TEXTURE_2D, tex_);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		const unsigned char data[] = {
			255, 0, 0, 255,
			0, 255, 0, 255,
			0, 0, 255, 255,
			255, 255, 255, 255,
		};
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	void resize(int w, int h)
	{
		width_ = w;
		height_ = h;
	}

	// CefRenderHandler interface
public:
	bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
	{
		rect = CefRect(0, 0, width_, height_);
		return true;
	}

	void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height)
	{
		glBindTexture(GL_TEXTURE_2D, tex_);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (unsigned char*)buffer);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// CefBase interface
public:
	IMPLEMENT_REFCOUNTING(RenderHandler);

public:
	GLuint tex() const { return tex_; }

private:
	int width_;
	int height_;

	GLuint tex_;
};

// for manual render handler
class BrowserClient : public CefClient
{
public:
	BrowserClient(RenderHandler *renderHandler)
		: m_renderHandler(renderHandler)
	{
		;
	}

	virtual CefRefPtr<CefRenderHandler> GetRenderHandler() {
		return m_renderHandler;
	}

	CefRefPtr<CefRenderHandler> m_renderHandler;

	IMPLEMENT_REFCOUNTING(BrowserClient);
};


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

void draw(GLFWwindow *window)
{
	float ratio;
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	ratio = width / (float)height;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef((float)glfwGetTime() * 10.f, 0.f, 0.f, 1.f);

	glBegin(GL_TRIANGLE_FAN);
	// 3 2
	// 0 1

	// 0
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.8f, -0.8f, 0.f);

	// 1
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.8f, -0.8f, 0.f);

	// 2
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.8f, 0.8f, 0.f);

	// 3
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.8f, 0.8f, 0.f);

	glEnd();
}

void display(GLFWwindow* window)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	draw(window);
	glfwSwapBuffers(window);
}

void reshape_callback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}


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

	RenderHandler *renderHandler = new RenderHandler(width, height);
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

	glClearColor(0.0, 0.0, 0.0, 0.0);

	while (!glfwWindowShouldClose(window))
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderHandler->tex());
		// draw
		display(window);
		glDisable(GL_TEXTURE_2D);

		// update
		glfwPollEvents();

		// cef
		CefDoMessageLoopWork();
	}

	// close cef
	browser->GetHost()->CloseBrowser(true);
	CefDoMessageLoopWork();

	browser = nullptr;
	browserClient = nullptr;
	CefShutdown();

	if (window) {
		shutdown_glfw(window);
	}

	return 0;  
}