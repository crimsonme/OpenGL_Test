#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "myshader.h"
#include"camera.h"
#include "model.h"


#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);



// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

// camera
Camera camera(glm::vec3(0.0f, 6.0f, 20.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//帧率和时间
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f;//上一幀的時間
//light
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

namespace FileSystem
{

	std::string RootPath = "";

	void SetRootPath(const std::string& InRootPath)
	{
		RootPath = InRootPath;
	}

	std::string GetPath(const std::string& InPath)
	{
		return RootPath + InPath;
	}
}

int main()
{
	
	FileSystem::SetRootPath("E:\\Test_VS\\project\\OpenGLproject\\");
	
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	//启用深度测试
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//启用模板测试   
	glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);//所有物体全覆盖蒙版刷新

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// build and compile our shader program
	// ------------------------------------
	Shader ourShader("base.vs", "base.fs"); // you can name your shader files however you like
	Shader lightShader("light.vs", "light.fs");
	Shader lampShader("lamp.vs", "lamp.fs");
	Shader stencilShader("base.vs", "stencil_single_color.fs");
	Shader alphaShader("alpha.vs","alpha.fs");
	Shader uboShader("ubosimple.vs", "red.fs");



	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// positions          // colors           // texture coords
		 4.0f,  4.0f, 0.0f,   0.0f, 0.0f, 1.0f,  1.0f, 1.0f, // top right
		 4.0f, -4.0f, 0.0f,   0.0f, 0.0f, 1.0f,  1.0f, 0.0f, // bottom right
		-4.0f, -4.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-4.0f, 4.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	float vertices1[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	unsigned int VBO1, VAO1;
	glGenVertexArrays(1, &VAO1);
	glGenBuffers(1, &VBO1);

	glBindVertexArray(VAO1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//灯光位置方块
	float vertices2[] = {
	   -0.2f, -0.2f, -0.2f,
		0.2f, -0.2f, -0.2f,
		0.2f,  0.2f, -0.2f,
		0.2f,  0.2f, -0.2f,
	   -0.2f,  0.2f, -0.2f,
	   -0.2f, -0.2f, -0.2f,

	   -0.2f, -0.2f,  0.2f,
		0.2f, -0.2f,  0.2f,
		0.2f,  0.2f,  0.2f,
		0.2f,  0.2f,  0.2f,
	   -0.2f,  0.2f,  0.2f,
	   -0.2f, -0.2f,  0.2f,

	   -0.2f,  0.2f,  0.2f,
	   -0.2f,  0.2f, -0.2f,
	   -0.2f, -0.2f, -0.2f,
	   -0.2f, -0.2f, -0.2f,
	   -0.2f, -0.2f,  0.2f,
	   -0.2f,  0.2f,  0.2f,

		0.2f,  0.2f,  0.2f,
		0.2f,  0.2f, -0.2f,
		0.2f, -0.2f, -0.2f,
		0.2f, -0.2f, -0.2f,
		0.2f, -0.2f,  0.2f,
		0.2f,  0.2f,  0.2f,

	   -0.2f, -0.2f, -0.2f,
		0.2f, -0.2f, -0.2f,
		0.2f, -0.2f,  0.2f,
		0.2f, -0.2f,  0.2f,
	   -0.2f, -0.2f,  0.2f,
	   -0.2f, -0.2f, -0.2f,

	   -0.2f,  0.2f, -0.2f,
		0.2f,  0.2f, -0.2f,
		0.2f,  0.2f,  0.2f,
		0.2f,  0.2f,  0.2f,
	   -0.2f,  0.2f,  0.2f,
	   -0.2f,  0.2f, -0.2f,
	};
	unsigned int VBO2, VAO2;
	glGenVertexArrays(1, &VAO2);
	//glBindVertexArray(VAO2);
	glGenBuffers(1, &VBO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	glBindVertexArray(VAO2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(0);



	float vertices3[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};
	unsigned int VBO3, VAO3;
	glGenVertexArrays(1, &VAO3);

	glGenBuffers(1, &VBO3);
	glBindBuffer(GL_ARRAY_BUFFER, VBO3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices3), vertices3, GL_STATIC_DRAW);
	glBindVertexArray(VAO3);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);




	float vertices4[] = {
		// positions          // colors           // texture coords
		 4.0f,  4.0f, 0.0f,   0.0f,  1.0f, 0.0f,  // top right
		 4.0f, -4.0f, 0.0f,  0.0f,  1.0f, 0.0f,  // bottom right
		-4.0f, -4.0f, 0.0f, 0.0f,  1.0f, 0.0f,  // bottom left
		-4.0f, 4.0f, 0.0f,   0.0f,  1.0f, 0.0f// top left 
	};
	unsigned int indices2[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	unsigned int VBO4, VAO4, EBO4;
	glGenVertexArrays(1, &VAO4);
	glGenBuffers(1, &VBO4);//初始化局部变量
	glGenBuffers(1, &EBO4);

	glBindVertexArray(VAO4);

	glBindBuffer(GL_ARRAY_BUFFER, VBO4);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices4), vertices4, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO4);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), indices2, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	float transparentVertices[] = {
		// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};
	unsigned int transparentVAO, transparentVBO;
	glGenVertexArrays(1, &transparentVAO);
	glGenBuffers(1, &transparentVBO);
	glBindVertexArray(transparentVAO);
	glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);




	//创建载入纹理

		//stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	unsigned char*data;
	unsigned int texture[2];
	glGenTextures(2, texture);

	glBindTexture(GL_TEXTURE_2D, texture[0]); //綁定狀態機
	data = stbi_load("box.jpg", &width, &height, &nrChannels, 0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
	//第二张纹理

	glBindTexture(GL_TEXTURE_2D, texture[1]);
	data = stbi_load("star.png", &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	//透明纹理
	unsigned int transparentTexture;
	glGenTextures(1, &transparentTexture);//创建一个纹理
	glBindTexture(GL_TEXTURE_2D, transparentTexture);
	data = stbi_load("triangle.png", &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);


	//漫反射纹理

	unsigned int diffuseMap;
	glGenTextures(1, &diffuseMap);

	glBindTexture(GL_TEXTURE_2D, diffuseMap); //綁定狀態機
	data = stbi_load("star2.jpg", &width, &height, &nrChannels, 0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	unsigned int specularMap;
	glGenTextures(1, &specularMap);

	glBindTexture(GL_TEXTURE_2D, specularMap); //綁定狀態機
	data = stbi_load("star2_H.jpg", &width, &height, &nrChannels, 0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);



	//箱子位置数组
	glm::vec3 cubePositions[] = {
	glm::vec3(3.0f,  3.0f,  3.0f),
	glm::vec3(2.0f,  2.0f, -3.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -2.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -1.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
	};


	// positions of the point lights
	glm::vec3 pointLightPositions[] = {
		glm::vec3(4.0f,  1.0f,  -2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -5.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	unsigned int uniformBlockIndexBase = glGetUniformBlockIndex(uboShader.ID, "Matrices");
	glUniformBlockBinding(uboShader.ID, uniformBlockIndexBase, 0);
	unsigned int uboMatrices;
	glGenBuffers(1, &uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));




	//用Assimp加载模型
	
	Model ourModel(FileSystem::GetPath("ahong/sphere.obj"));

	Model manModel(FileSystem::GetPath("nanosuit/nanosuit.obj"));

	//unsigned int transparentTexture = loadTexture(FileSystem::GetPath("OpenGLproject/triangle.png").c_str());








	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;



		// render
		//清缓存信息
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT| GL_STENCIL_BUFFER_BIT); // also clear the depth buffer now!

		
	    
		lightShader.use();
		lightShader.setInt("material.diffuse", 0);
		lightShader.setInt("material.specular", 1);
		lightShader.setVec3("viewPos", camera.Position);

		lightShader.setFloat("material.shininess", 8.0f);
		lightShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		lightShader.setVec3("dirLight.ambient", 0.3f, 0.1f, 0.13f);
		lightShader.setVec3("dirLight.diffuse", 1.0f, 1.0f, 1.0f);
		lightShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// point light 1
		lightShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		lightShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		lightShader.setVec3("pointLights[0].diffuse", 0.0f, 0.0f, 0.5f);
		lightShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		lightShader.setFloat("pointLights[0].constant", 1.0f);
		lightShader.setFloat("pointLights[0].linear", 0.1);
		lightShader.setFloat("pointLights[0].quadratic", 0.3);
		// point light 2
		lightShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		lightShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		lightShader.setVec3("pointLights[1].diffuse", 0.5f, 0.0f, 0.0f);
		lightShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		lightShader.setFloat("pointLights[1].constant", 1.0f);
		lightShader.setFloat("pointLights[1].linear", 0.1);
		lightShader.setFloat("pointLights[1].quadratic", 0.3);
		// point light 3
		lightShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		lightShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		lightShader.setVec3("pointLights[2].diffuse", 0.0f, 0.5f, 0.0f);
		lightShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		lightShader.setFloat("pointLights[2].constant", 1.0f);
		lightShader.setFloat("pointLights[2].linear", 0.1);
		lightShader.setFloat("pointLights[2].quadratic", 0.3);
		// point light 4
		lightShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		lightShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		lightShader.setVec3("pointLights[3].diffuse", 0.5f, 0.5f, 0.0f);
		lightShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		lightShader.setFloat("pointLights[3].constant", 1.0f);
		lightShader.setFloat("pointLights[3].linear", 0.1);
		lightShader.setFloat("pointLights[3].quadratic", 0.3);
		// spotLight
		lightShader.setVec3("spotLight.position", camera.Position);
		lightShader.setVec3("spotLight.direction", camera.Front);
		lightShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		lightShader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
		lightShader.setVec3("spotLight.specular", 0.0f, 0.0f, 0.0f);
		lightShader.setFloat("spotLight.constant", 1.0f);
		lightShader.setFloat("spotLight.linear", 0.09);
		lightShader.setFloat("spotLight.quadratic", 0.002);
		lightShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		lightShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

		// material properties

		
		//glUniform3f(glGetUniformLocation(lightShader.ID, "lightColor", 0.5f, 0.5f, 0.5f);
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		lightShader.setMat4("projection", projection);
		lightShader.setMat4("view", view);

		
		// 绘制载入模型
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 6.0f, 0.5f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));	// it's a bit too big for our scene, so scale it down
		lightShader.setMat4("model", model);
		ourModel.Draw(lightShader);

		

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));	// it's a bit too big for our scene, so scale it down
		lightShader.setMat4("model", model);
		manModel.Draw(lightShader);
		

		//绘制受光平面
		model = glm::mat4(1.0f);  // 确保先将矩阵transform初始化为单位矩阵
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		
		lightShader.setMat4("model", model);
		

		//激活绑定贴图
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		//绑定模型
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(10.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		lightShader.setMat4("model", model);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-10.0f, 0.0f, -0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		lightShader.setMat4("model", model);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//绘制受光箱子
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-3.0f, 2.0f, 0.0f));
		lightShader.setMat4("model", model);
		glBindVertexArray(VAO1);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(5.0f, 2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		lightShader.setMat4("model", model);
		glBindVertexArray(VAO3);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-5.0f, 2.0f, -0.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		lightShader.setMat4("model", model);
		glBindVertexArray(VAO3);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(3.0f, 2.0f, -0.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		lightShader.setMat4("model", model);
		glBindVertexArray(VAO3);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		//绘制灯光箱子
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);

		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		lampShader.setMat4("model", model);

		glBindVertexArray(VAO2);
		for (unsigned int i = 0; i < 4; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			lampShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glDrawArrays(GL_TRIANGLES, 0, 36);


		


		//绘制贴图箱子

		ourShader.use(); 
		ourShader.setInt("texture1", 0);
		ourShader.setInt("texture2", 1);
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-10.0f, 2.0f, -2.0f));
		model = glm::scale(model, glm::vec3(3.0f));
		ourShader.setMat4("model", model);
		//绑定纹理属性
		glActiveTexture(GL_TEXTURE0);//纹理单元0
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture[1]);

		

		glBindVertexArray(VAO1);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-10.0f, 2.0f, 2.0f));
		model = glm::scale(model, glm::vec3(3.0f));
		ourShader.setMat4("model", model);

		//glBindVertexArray(VAO1);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		
		
		


		glStencilFunc(GL_ALWAYS, 1, 0xFF); //所有片段的模板缓冲都更新为1
		glStencilMask(0xFF);//启用模板缓冲写入
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(12.0f, 2.0, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f));
		ourShader.setMat4("model", model);


		glDrawArrays(GL_TRIANGLES, 0, 36);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);//只绘制箱子上模板不为Ⅰ的片段
		glStencilMask(0x00);//禁用模板缓冲写入
		//glDisable(GL_DEPTH_TEST);
		//放大的箱子模型
		stencilShader.use();
		stencilShader.setMat4("projection", projection);
		stencilShader.setMat4("view", view);
		float scale = 3.1;
		model = glm::mat4(1.0f);

		model = glm::translate(model, glm::vec3(12.0f, 2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(scale, scale, scale));

		stencilShader.setMat4("model", model);

		//在结束时重启模板和深度测试
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glStencilMask(0xFF);//重新启用模板缓冲写入
		glStencilFunc(GL_ALWAYS, 0, 0xFF);//所有片段模板缓冲更新为0
		glEnable(GL_DEPTH_TEST);

		//绘制透明片
		alphaShader.use();
		alphaShader.setInt("texture5", 0);
		alphaShader.setMat4("projection", projection);
		alphaShader.setMat4("view", view);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(1.0f, 3.0f, 0.0f));

		glBindVertexArray(transparentVAO);
		glActiveTexture(GL_TEXTURE0);//纹理单元0
		glBindTexture(GL_TEXTURE_2D, transparentTexture);
		alphaShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 6);





		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO1);

	glDeleteBuffers(1, &VBO1);
	glDeleteBuffers(1, &VBO);

	glDeleteVertexArrays(1, &VAO2);
	glDeleteVertexArrays(1, &VAO3);
	glDeleteBuffers(1, &VBO3);

	
	glDeleteVertexArrays(1, &transparentVAO);
	
	glDeleteBuffers(1, &transparentVBO);






	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}