#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Ball.h"
#include "Texture.h"

float rotate = 0.0f;

// Window dimensions
const GLint WIDTH = 1920, HEIGHT = 1080;
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;
Texture field;
Texture ball;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.005f;

float curAngle = 0.0f;

bool sizeDirection = true;
float curSize = 0.4f;
float maxSize = 0.8f;
float minSize = 0.1f;

// Vertex Shader code
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	unsigned int indicesPlane[] = {
		0, 1, 3,
		2, 1, 3,
	};

	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,				0.0f, 1.0f,
		0.0f, -1.0f, 1.0f,				0.0f, 0.0f,
		1.0f, -1.0f, 0.0f,				1.0f, 0.0f,	
		0.0f, 1.0f, 0.0f,				1.0f, 1.0f
	};

	GLfloat verticesPlane[] = {
		-1.0f, 0.0f, -1.0f,				1.0f, 1.0f,
		-1.0f, 0.0f, 1.0f,				0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,				0.0f, 0.0f,
		1.0f, 0.0f, -1.0f,				1.0f, 0.0f
	};

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 20, 12);
	meshList.push_back(obj1);

	Mesh *plane = new Mesh();
	plane->CreateMesh(verticesPlane, indicesPlane, 20, 6);
	meshList.push_back(plane);
}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	//W,Vb,xAngle,yAngle,spinDir
	Ball myBall(80.0f, 36.0f, 0.0f, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	

	mainWindow = Window(800, 600);
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.5f);

	field = Texture("Textures/soccerfield.png");
	field.LoadTexture();
	ball = Texture("Textures/foolball.png");
	ball.LoadTexture();


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	// Loop until window closed
	while (!mainWindow.getShouldClose())
	{
		
		GLfloat now = glfwGetTime(); // SDL_GetPerformanceCounter();
		deltaTime = now - lastTime; // (now - lastTime)*1000/SDL_GetPerformanceFrequency();
		// Get + Handle user input events

		lastTime = now;

		glfwPollEvents();
		
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		

		// Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();

		glm::mat4 model = glm::mat4(1.0f);
		bool* keys = mainWindow.getsKeys();
		if (keys[GLFW_KEY_F])
		{
			myBall.kick();
		}
		if (myBall.getHasBeenKicked())
		{
			model = glm::translate(model, myBall.euler(deltaTime));
			rotate -= 0.05f;
			model = glm::rotate(model, rotate, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		
		
		
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		ball.UseTexture();
		meshList[0]->RenderMesh();

		model = glm::mat4(1.0f);
		//model = glm::rotate(model, )
		model = glm::scale(model, glm::vec3(67.0f, 1.0f, 103.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		field.UseTexture();
		meshList[1]->RenderMesh();
		
		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}