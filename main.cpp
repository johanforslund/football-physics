#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <assimp/Importer.hpp>
#include "imgui\imgui.h"
#include "imgui\imgui_impl_glfw_gl3.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Ball.h"
#include "Texture.h"
#include "Light.h"

#include "Model.h"

#include "Skybox.h"

float rotate = 0.0f;

// Window dimensions
const float toRadians = 3.14159265f / 180.0f;

GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;
GLuint uniformAmbientIntensity = 0, uniformAmbientColour = 0, uniformDirection = 0, uniformDiffuseIntensity = 0, uniformDirectionalLightTransform = 0;

Window mainWindow;
std::vector<Shader> shaderList;
Shader directionalShadowShader;

Camera camera;

Model nanosuit;
Model stadiumMod;
Model ballMod;
Model wallMod;
Model goalMod;

Light mainLight;

Skybox skybox;

//W,Vb,xAngle,yAngle,spinDir
float angularVelocity = 100.0f;
glm::vec3 spinDirection = glm::vec3(0.0f, 1.0f, 0.0f);
float initVelocity = 40.0f;
float xAngle = -20.0f;
float yAngle = 20.0f;
glm::vec3 ballStartPosition = glm::vec3(0.0f, 0.31f, 0.0f);
Ball myBall(angularVelocity, initVelocity, xAngle, yAngle, spinDirection);

bool shouldFollowBall = false;
bool settingsActive = false;
bool setupStage = true;
glm::vec3 cameraSetupPosition = glm::vec3(0.0f, 110.0f, 0.0f);
GLfloat cameraSetupYaw = -90.0f;
GLfloat cameraSetupPitch = -70.0f;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader code
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	directionalShadowShader.CreateFromFiles("Shaders/directional_shadow_map.vert", "Shaders/directional_shadow_map.frag");
}

void RenderScene()
{
	glm::mat4 model = glm::mat4(1.0f);
	if (setupStage)
	{
		model = glm::translate(model, ballStartPosition);
	}
	else if (myBall.getHasBeenKicked())
	{
		model = glm::translate(model, myBall.euler(deltaTime, ballStartPosition) + ballStartPosition);
		rotate -= myBall.getAngularVelocity() / 200.0f;
		glm::vec3 spinDirectionDummy = myBall.getSpinDirection();
		model = glm::rotate(model, rotate, spinDirectionDummy);
	}
	else
	{
		model = glm::translate(model, ballStartPosition);
	}

	model = glm::scale(model, glm::vec3(0.06f, 0.06f, 0.06f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	ballMod.RenderModel();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-125.0f, 0.0f, 204.0f));
	model = glm::scale(model, glm::vec3(1.8f, 1.8f, 1.8f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	stadiumMod.RenderModel();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -50.0f));
	model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	wallMod.RenderModel();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -93.0f));
	model = glm::scale(model, glm::vec3(0.45f, 0.45f, 0.45f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	goalMod.RenderModel();
}

void DirectionalShadowMapPass(Light* light)
{
	directionalShadowShader.UseShader();

	glViewport(0, 0, light->getShadowMap()->GetShadowWidth(), light->getShadowMap()->GetShadowHeight());

	light->getShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = directionalShadowShader.GetModelLocation();
	directionalShadowShader.SetDirectionalLightTransform(&light->CalculateLightTransform());

	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	glViewport(0, 0, mainWindow.getWidth(), mainWindow.getHeight());
	// Clear window
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	skybox.DrawSkybox(viewMatrix, projectionMatrix);

	shaderList[0].UseShader();

	uniformModel = shaderList[0].GetModelLocation();
	uniformProjection = shaderList[0].GetProjectionLocation();
	uniformView = shaderList[0].GetViewLocation();
	uniformAmbientColour = shaderList[0].GetAmbientColourLocation();
	uniformAmbientIntensity = shaderList[0].GetAmbientIntensityLocation();
	uniformDirection = shaderList[0].GetDirectionLocation();
	uniformDiffuseIntensity = shaderList[0].GetDiffuseIntensityLocation();

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	shaderList[0].SetDirectionalLightTransform(&mainLight.CalculateLightTransform());

	mainLight.UseLight(uniformAmbientIntensity, uniformAmbientColour, uniformDiffuseIntensity, uniformDirection);
	mainLight.getShadowMap()->Read(GL_TEXTURE1);
	shaderList[0].SetTexture(0);
	shaderList[0].SetDirectionalShadowMap(1);

	RenderScene();
}

int main()
{
	mainWindow = Window(1366, 768);
	//W,Vb,xAngle,yAngle,spinDir
	mainWindow.Initialise();

	CreateShaders();

	camera = Camera(cameraSetupPosition, glm::vec3(0.0f, 1.0f, 0.0f), cameraSetupYaw, cameraSetupPitch, 5.0f, 0.5f);

	nanosuit = Model();
	nanosuit.LoadModel("Models/nanosuit.obj");

	stadiumMod = Model();
	stadiumMod.LoadModel("Models/Stadium.obj");

	ballMod = Model();
	ballMod.LoadModel("Models/soccerball.obj");

	wallMod = Model();
	wallMod.LoadModel("Models/wall.obj");

	goalMod = Model();
	goalMod.LoadModel("Models/goal.obj");

	mainLight = Light(2048, 2048, 0.9f, 0.9f, 1.0f, 0.4f, 34.0f, -150.0f, -66.0f, 0.5f);

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/bloody-heresy_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/bloody-heresy_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/bloody-heresy_up.tga");
	skyboxFaces.push_back("Textures/Skybox/bloody-heresy_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/bloody-heresy_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/bloody-heresy_ft.tga");

	skybox = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;

	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfwGL3_Init(mainWindow.mainWindow, true);
	ImGui::StyleColorsDark();

	// Loop until window closed
	while (!mainWindow.getShouldClose())
	{

		GLfloat now = glfwGetTime(); // SDL_GetPerformanceCounter();
		deltaTime = now - lastTime; // (now - lastTime)*1000/SDL_GetPerformanceFrequency();
		// Get + Handle user input events

		lastTime = now;

		glfwPollEvents();

		ImGui_ImplGlfwGL3_NewFrame();

		if (setupStage)
		{
			ballStartPosition.x += mainWindow.getXChange() * 0.1f;
			ballStartPosition.z -= mainWindow.getYChange() * 0.1f;
			camera.move(ballStartPosition + glm::vec3(0.0f, 60.0f, 25.0f), cameraSetupYaw, cameraSetupPitch);
		}
		else if (!settingsActive)
		{
			camera.keyControl(mainWindow.getsKeys(), deltaTime);
			camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}
			
		bool* keys = mainWindow.getsKeys();
		if (!setupStage && keys[GLFW_KEY_F])
		{
			if (!myBall.getHasBeenKicked())
			{
				myBall.reset(angularVelocity, initVelocity, xAngle, yAngle, spinDirection);
			}
			myBall.kick();
		}
		if (!setupStage && keys[GLFW_KEY_Q])
		{
			myBall.reset(angularVelocity, initVelocity, xAngle, yAngle, spinDirection);
			camera.move(cameraSetupPosition, cameraSetupYaw, cameraSetupPitch);
			//ballStartPosition = glm::vec3(0.0f, 0.31f, 0.0f);
			setupStage = true;
		}
		if (!setupStage && keys[GLFW_KEY_R])
		{
			myBall.reset(angularVelocity, initVelocity, xAngle, yAngle, spinDirection);
		}
		if (setupStage && ImGui::IsMouseClicked(0))
		{
			if (!shouldFollowBall)
			{
				camera.move(ballStartPosition + glm::vec3(0.0f, 5.0f, 20.0f), -90.0f, 0.0f);
			}
			setupStage = false;
		}
		if (keys[GLFW_KEY_LEFT_CONTROL])
		{
			settingsActive = true;
			glfwSetInputMode(mainWindow.mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else
		{
			settingsActive = false;
			glfwSetInputMode(mainWindow.mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		if (!setupStage && shouldFollowBall)
		{
			camera.followBall(ballStartPosition + myBall.getPosition());
		}
		
		DirectionalShadowMapPass(&mainLight);
		RenderPass(camera.calculateViewMatrix(), projection);

		ImGui::SetWindowCollapsed(true);
		ImGui::Begin("Settings");
		ImGui::SliderFloat("Initial Velocity", &initVelocity, 0.0f, 50.0f);
		ImGui::SliderFloat("Angular Velocity", &angularVelocity, -100.0f, 100.0f);
		ImGui::SliderFloat("X Angle", &xAngle, -60.0f, 60.0f);
		ImGui::SliderFloat("Y Angle", &yAngle, 0.0f, 89.0f);
		ImGui::Checkbox("Follow Ball?", &shouldFollowBall);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		
		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();

	return 0;
}