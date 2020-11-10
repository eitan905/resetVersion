#define _USE_MATH_DEFINES
#include <cmath>
#include <imgui/imgui.h>
 
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <nfd.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Renderer.h"
#include "Scene.h"
#include "Utils.h"



static float mouse_x;
static float mouse_y;
static float previous_mouse_x = 0;
static float previous_mouse_y = 0;

float sign(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3)
{
	return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool PointInTriangle(glm::vec2 pt, glm::vec2 v1, glm::vec2 v2, glm::vec2 v3)
{
	float d1, d2, d3;
	bool has_neg, has_pos;

	d1 = sign(pt, v1, v2);
	d2 = sign(pt, v2, v3);
	d3 = sign(pt, v3, v1);

	has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(has_neg && has_pos);
}


/**
 * Fields
 */
bool show_demo_window = false;
bool show_another_window = false;
glm::vec4 clear_color = glm::vec4(0.8f, 0.8f, 0.8f, 1.00f);

/**
 * Function declarations
 */
static void GlfwErrorCallback(int error, const char* description);
GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name);
ImGuiIO& SetupDearImgui(GLFWwindow* window);
void StartFrame();
void RenderFrame(GLFWwindow* window, Scene& scene, Renderer& renderer, ImGuiIO& io);
void Cleanup(GLFWwindow* window);
void DrawImguiMenus(ImGuiIO& io, Scene& scene);

/**
 * Function implementation
 */
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	// TODO: Handle mouse scroll here
}

int main(int argc, char **argv)
{
	int windowWidth = 1280, windowHeight = 720;
	GLFWwindow* window = SetupGlfwWindow(windowWidth, windowHeight, "Mesh Viewer");
	if (!window)
		return 1;

	int frameBufferWidth, frameBufferHeight;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);


	Renderer renderer = Renderer(frameBufferWidth, frameBufferHeight);
	Scene scene = Scene();

	scene.AddModel(Utils::LoadMeshModel("C:\\Users\\Eitan\\Desktop\\bunny.txt"));
	

	
	ImGuiIO& io = SetupDearImgui(window);
	glfwSetScrollCallback(window, ScrollCallback);
    while (!glfwWindowShouldClose(window))
    {

        glfwPollEvents();
		StartFrame();
		DrawImguiMenus(io, scene);
		RenderFrame(window, scene, renderer, io);
    }

	Cleanup(window);
    return 0;
}

static void GlfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name)
{
	glfwSetErrorCallback(GlfwErrorCallback);
	if (!glfwInit())
		return NULL;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	#if __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif
	
	GLFWwindow* window = glfwCreateWindow(w, h, window_name, NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync
						 // very importent!! initialization of glad
						 // https://stackoverflow.com/questions/48582444/imgui-with-the-glad-opengl-loader-throws-segmentation-fault-core-dumped

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	return window;
}

ImGuiIO& SetupDearImgui(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	ImGui::StyleColorsDark();
	return io;
}

void StartFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void RenderFrame(GLFWwindow* window, Scene& scene, Renderer& renderer, ImGuiIO& io)
{
	MeshModel& obj = scene.GetModel(0);
	ImGui::Render();
	int frameBufferWidth, frameBufferHeight;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);
	
	if (frameBufferWidth != renderer.GetViewportWidth() || frameBufferHeight != renderer.GetViewportHeight())
	{
		// TODO: Set new aspect ratio
	}

	if (!io.WantCaptureKeyboard)
	{
		// TODO: Handle keyboard events here
		if (io.KeysDown[68])
		{
			obj.LocalTranslateTransform(4,0,0);		
		}
		if (io.KeysDown[65])
		{
			obj.LocalTranslateTransform(-4, 0, 0);
		}
		if (io.KeysDown[83])
		{
			obj.LocalTranslateTransform(0, -4, 0);
		}
		if (io.KeysDown[87])
		{
			obj.LocalTranslateTransform(0, 4, 0);
		}

	}



	if (!io.WantCaptureMouse)
	{

		// TODO: Handle mouse events here
		std::vector<MeshModel*> models;
		if (io.MouseDown[0])
		{
			bool isMouseOnModel = false;
			mouse_x = io.MousePos[0];
			mouse_y = io.MousePos[1];
				for (int i = 0; i < scene.GetModelCount(); i++) {
					MeshModel temp_obj = scene.GetModel(i);
					for (int j = 0; j < obj.getVerticesSize(); j++) {
						glm::vec4 temp = temp_obj.GetTransform() * glm::vec4(temp_obj.getVerticeAtIndex(j), 1);
						temp_obj.getVerticeAtIndex(j)[0] = temp[0];
						temp_obj.getVerticeAtIndex(j)[1] = temp[1];
						temp_obj.getVerticeAtIndex(j)[2] = temp[2];
					}
					for (int j = 0; j < temp_obj.GetFacesCount(); j++) {
						Face face = temp_obj.GetFace(j);
						int point0 = face.GetVertexIndex(0) - 1;
						int point1 = face.GetVertexIndex(1) - 1;
						int point2 = face.GetVertexIndex(2) - 1;

						glm::vec2 p(mouse_x, mouse_y);
						glm::vec2 p1(temp_obj.getVerticeAtIndex(point0)[0], temp_obj.getVerticeAtIndex(point0)[1]);
						glm::vec2 p2(temp_obj.getVerticeAtIndex(point1)[0], temp_obj.getVerticeAtIndex(point1)[1]);
						glm::vec2 p3(temp_obj.getVerticeAtIndex(point2)[0], temp_obj.getVerticeAtIndex(point2)[1]);
						if (PointInTriangle(p, p1, p2, p3)) {
							isMouseOnModel = true;
						}
					}
					if (isMouseOnModel) {
						std::cout << "found model" ;
						models.push_back(&scene.GetModel(i));
						isMouseOnModel = false;
					}
				}
			
			for (int t = 0; t < models.size(); t++) {
				if (previous_mouse_x != 0) {
					models[t]->LocalTranslateTransform(mouse_x - previous_mouse_x, previous_mouse_y - mouse_y , 0);
				}
				previous_mouse_x = mouse_x;
				previous_mouse_y = mouse_y;
			}
		}
		models.empty();
	}

	renderer.ClearColorBuffer(clear_color);
	renderer.Render(scene);
	renderer.SwapBuffers();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwMakeContextCurrent(window);
	glfwSwapBuffers(window);
}

void Cleanup(GLFWwindow* window)
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void DrawImguiMenus(ImGuiIO& io, Scene& scene)
{
	/**
	 * MeshViewer menu
	 */
	ImGui::Begin("MeshViewer Menu");
	
	// Menu Bar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open", "CTRL+O"))
			{
				nfdchar_t* outPath = NULL;
				nfdresult_t result = NFD_OpenDialog("obj;", NULL, &outPath);
				if (result == NFD_OKAY)
				{
					scene.AddModel(Utils::LoadMeshModel(outPath));
					free(outPath);
				}
				else if (result == NFD_CANCEL)
				{
				}
				else
				{
				}

			}
			ImGui::EndMenu();
		}

		// TODO: Add more menubar items (if you want to)
		ImGui::EndMainMenuBar();
	}

	// Controls
	ImGui::ColorEdit3("Clear Color", (float*)&clear_color);
	// TODO: Add more controls as needed
	
	ImGui::End();

	/**
	 * Imgui demo - you can remove it once you are familiar with imgui
	 */
	
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		MeshModel& obj = scene.GetModel(scene.GetActiveModelIndex());
		static float rotate = 0.0f,x=0.0f, y = 0.0f, z = 0.0f,alpha=0.0f;
		static float& scaleValue = obj.GetScaleBarValue();
		
		static int i1 = 0,i2=0,i3=0;
		static int counter = 0;
		//ImGuiInputTextFlags INPUT_TEXT_CHARS_DECIMAL =0;
		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
	
		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);


		static const char* currentModels[]{ "1" ,"2","3","4","5","6"};
		static int selecteItem = scene.GetActiveModelIndex();
		
		ImGui::ListBox("active model", &selecteItem, currentModels, 6, 2);

		ImGui::InputFloat("x", &x, 0.01f, 1.0f);
		ImGui::InputFloat("y", &y, 0.01f, 1.0f);
		ImGui::InputFloat("z", &z, 0.01f, 1.0f);
		ImGui::InputFloat("alfa", &alpha, 0.01f, 1.0f);

		bool translationFlag = ImGui::Button("Translate");
		ImGui::SameLine();
		bool scalingFlag = ImGui::Button("Scale");
		ImGui::SameLine();
		bool rotateFlag = ImGui::Button("Rotate");
		ImGui::SameLine();

		
		


		static bool checkedLocal = true;
		static bool checkedWorld = false;

		ImGui::SameLine();
		//ImGui::RadioButton("scaling ", &alpha_flags);
		bool localFlag = ImGui::Checkbox ("local", &checkedLocal);
		ImGui::SameLine();
		bool worldFlag = ImGui::Checkbox ("world", &checkedWorld);

		if (translationFlag) {
			
			if (checkedLocal) {
				obj.LocalTranslateTransform(x, y, z);
			}
			if (checkedWorld) {
				obj.WorldTranslateTransform(x, y, z);
			}
			x = y = z = 0.0f;

		}

		if (scalingFlag) {
			
			if (checkedLocal) {
				obj.LocalScaleTransform(x, y, z);
			}
			if (checkedWorld) {
				obj.WorldScaleTransform(x, y, z);
			}
			x = y = z = 0.0f;
		}


		if (rotateFlag) {

			if (checkedLocal) {
				obj.LocalRotationTransform(alpha);
				alpha = 0.0f;
			}
			if (checkedWorld) {
				obj.WorldRotationTransform(alpha);
				alpha = 0.0f;
			}



		}


	



		
	
		
		//ImGui::SameLine();
		//ImGui::RadioButton("Both", &alpha_flags, ImGuiColorEditFlags_AlphaPreviewHalf);


		
		ImGui::SliderFloat("ScaleSlider", &scaleValue, 1.0f, 1000.0f);
		ImGui::SliderFloat("Rotate_X", &rotate, 0.0f, 360.0f);      // Edit 1 float using a slider from 0.0f to 1.0f
		obj.SetRotateBarValue(rotate);
		
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
		

	
		
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}
}

