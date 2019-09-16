#include "Window.h"



Window::Window()
{
	width = 800;
	height = 600;

	xChange = 0.0f;
	yChange = 0.0f;

	for (size_t i = 0; i < 1024; i++)
	{
			keys[i] = 0;
	}
}

Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;

	xChange = 0.0f;
	yChange = 0.0f;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}

int Window::Initialise()
{

	// Initialise GLFW
	if (!glfwInit())
	{
		printf("GLFW initialisation failed!");
		glfwTerminate();
		return 1;
	}

	// Setup GLFW window properties
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Core Profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Allow Forward Compatbility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	const int window_width = mode->width;
	const int window_height = mode->height;

	if (window_width > 0 && window_height > 0)
	{
		width = window_width;
		height = window_height;
	}

	// Create the window
	mainWindow = glfwCreateWindow(width, height, "Test Window", glfwGetPrimaryMonitor(), NULL);

	// Create the window
	if (!mainWindow)
	{
		printf("GLFW window creation failed!");
		glfwTerminate();
		return 1;
	}

	// Get Buffer Size information
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set context for GLEW to use
	glfwMakeContextCurrent(mainWindow);

	// Handle Key + Mouse Input
	createCallbacks();
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	GLenum error = glewInit();

	if (glewInit() != GLEW_OK)
	{
		printf("GLEW initialisation failed!");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST);

	// Setup Viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);

	glfwSetWindowUserPointer(mainWindow, this);
}

void Window::createCallbacks()
{
		glfwSetKeyCallback(mainWindow, handleKeys);
		glfwSetCursorPosCallback(mainWindow, handleMouse);
}
GLfloat Window::getXChange()
{
		GLfloat theChange = xChange;
		xChange = 0.0f;
		//printf("%f", theChange);
		return theChange;
}
GLfloat Window::getYChange()
{
		GLfloat theChange = yChange;
		yChange = 0.0f;
		//printf("%f", theChange);
		return theChange;
}

void Window::handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
		Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
				glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key >= 0 && key < 1024)
		{
				if (action == GLFW_PRESS)
				{
						theWindow->keys[key] = true;
				}
				else if (action == GLFW_RELEASE)
				{
						theWindow->keys[key] = false;
				}
		}
}
void Window::handleMouse(GLFWwindow* window, double xPos, double yPos)
{
		Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
		if (theWindow->mouseFirstMoved)
		{
				theWindow->lastX = xPos;
				theWindow->lastY = yPos;
				theWindow->mouseFirstMoved = false;
		}
		theWindow->xChange = xPos - theWindow->lastX;
		theWindow->yChange = theWindow->lastY - yPos;
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
}


Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}
