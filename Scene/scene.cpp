#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/shader_s.h>
#include <iostream>
#include <map>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void RenderText(unsigned int& VAO, unsigned int& VBO, std::string text, float);
void localBindArrayObject(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, float* vertices, unsigned int vertices_size, unsigned int* indices, unsigned int indices_size);
void localBindTextureObject(unsigned int& texture, bool isFlip, const char* filename_max);
struct Character {
	unsigned int TextureID;
	glm::ivec2 Size;
	glm::ivec2 Bearing;
	unsigned int Advance;
};
std::map<GLchar, Character> Characters;
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL",
		NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	// build and compile our shader zprogram
	// ------------------------------------
	Shader ourShader1("texture.vs", "texture.fs");
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices_sceme[] = {
		// positions // colors // texture coords (note that we changed them to 'zoom in' on our texture image)
		1.0f, 0.8f, 0.0f,    1.0f, 0.0f, 0.0f,   0.8f, 1.0f, // top right
		1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   0.8f, 0.15f, // bottom right
		-1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,   0.15f, 0.15f, // bottom left
		-1.0f, 0.8f, 0.0f,   1.0f, 1.0f, 0.0f,   0.15f, 1.0f // top left
	};
	float vertices_grass[] = {
		// positions // colors // texture coords (note that we changed them to 'zoom in' on our texture image)
		0.7f, 0.2f, 0.0f, 1.0f, 0.0f, 0.0f, 5.0f, 1.0f, // top right
		0.7f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 5.0f, 0.0f, // bottom right
		0.2f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
		0.2f, 0.2f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left
	};
	float vertices_man[] = {
		// positions // colors // texture coords (note that we changed them to 'zoom in' on our texture image)
		0.0f, 0.05f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
		0.0f, -0.45f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
		0.2f, -0.45f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
		0.2f, 0.05f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left
	};
	float vertices_moon[] = {
		// positions // colors // texture coords (note that we changed them to 'zoom in' on our texture image)
		-0.85f, 0.95f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
		-0.85f, 0.8f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
		-1.0f, 0.8f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
		-1.0f, 0.95f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left
	};
	float vertices_cat[] = {
		// positions // colors // texture coords
		-0.05f, 0.58f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
		-0.05f, 0.38f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
		-0.15f, 0.38f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
		-0.15f, 0.58f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left
	};
	unsigned int indices[] = {
	0, 1, 3, // first triangle
	1, 2, 3 // second triangle
	};
	unsigned int VBO[5], VAO[5], EBO[5];
	glGenVertexArrays(5, VAO);
	glGenBuffers(5, VBO);
	glGenBuffers(5, EBO);
	localBindArrayObject(VAO[0], VBO[0], EBO[0], vertices_sceme, sizeof(vertices_sceme), indices, sizeof(indices));
	localBindArrayObject(VAO[1], VBO[1], EBO[1], vertices_grass, sizeof(vertices_sceme), indices, sizeof(indices));
	localBindArrayObject(VAO[2], VBO[2], EBO[2], vertices_man, sizeof(vertices_sceme), indices, sizeof(indices));
	localBindArrayObject(VAO[3], VBO[3], EBO[3], vertices_moon, sizeof(vertices_sceme), indices, sizeof(indices));
	localBindArrayObject(VAO[4], VBO[4], EBO[4], vertices_cat, sizeof(vertices_sceme), indices, sizeof(indices));
	unsigned int texture[5];
	glGenTextures(5, texture);
	localBindTextureObject(texture[0], true, FileSystem::getPath("resources/textures/lab3/mountain.png").c_str());
	localBindTextureObject(texture[1], true, FileSystem::getPath("resources/textures/lab3/grass.png").c_str());
	localBindTextureObject(texture[2], true, FileSystem::getPath("resources/textures/lab3/man.png").c_str());
	localBindTextureObject(texture[3], true, FileSystem::getPath("resources/textures/lab3/moon.png").c_str());
	localBindTextureObject(texture[4], true, FileSystem::getPath("resources/textures/lab3/cat.png").c_str());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//render loop
	//---------
	while (!glfwWindowShouldClose(window))
	{
		//input
		processInput(window);
		//render
		glClearColor(0.2f, 0.2f, 0.2f, 0.8f);
		glClear(GL_COLOR_BUFFER_BIT);
		//bind texture
		ourShader1.use();
		// Texture'ları bind et ve render et
		for (int i = 0; i < 5; ++i) {
			glBindTexture(GL_TEXTURE_2D, texture[i]);
			glBindVertexArray(VAO[i]);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(5, VAO);
	glDeleteBuffers(5, VBO);
	glDeleteBuffers(5, EBO);
	
	//glfw: terminate, clearing all previously allocated GLFW resources
	glfwTerminate();
	return 0;
}
	

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void localBindTextureObject(unsigned int& texture, bool isFlip, const char*	filename_max) 
{
	// Texture oluştur ve bind et
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Texture parametrelerini ayarla
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Texture yükleme işlemi
	stbi_set_flip_vertically_on_load(isFlip);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(filename_max, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	else {
		std::cout << "Texture yüklenemedi: " << filename_max << std::endl;
	}
	stbi_image_free(data);
}
void localBindArrayObject(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO,	float* vertices, unsigned int vertices_size, unsigned int* indices, unsigned int indices_size) 
{
	// VAO, VBO ve EBO'yu bağlayalım
	glBindVertexArray(VAO);
	// Vertex Buffer Object (VBO) oluştur ve bağla
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);
	// Element Buffer Object (EBO) oluştur ve bağla
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);
	// Vertex verilerinin düzenini ayarla
	// Konumlar
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Renkler
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 *
		sizeof(float)));
	glEnableVertexAttribArray(1);
	// Texture Koordinatları
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 *
		sizeof(float)));
	glEnableVertexAttribArray(2);
	// VAO'yu unbind et
	glBindVertexArray(0);
}

void RenderText(unsigned int& VAO, unsigned int& VBO, std::string text, float angle) 
{
	Shader ourShader2("4.1.metin.vs", "4.1.metin.fs");
	// Font boyutu ve konumu
	float x = 0.0f;
	float y = 0.0f;
	float scale = 1.0f;
	// Yazıyı render et
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);
	// Yazı karakterlerini render et
	for (auto& c : text) 
{
		// Her karakterin bilgilerini al
		Character ch = Characters[c];
		// Character'ın boyutlarına göre pozisyonları güncelle
		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
		// Character'ı VBO'ya yükle
		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		// Matrisi oluştur ve karakterin boyutlarına göre dönüşü yap
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(xpos, ypos, 0.0f));
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f,
			1.0f));
		model = glm::scale(model, glm::vec3(w, h, 1.0f));
		// Shader'a matrisi yolla
		ourShader2.setMat4("model", model);
		// Character'ı render et
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// X pozisyonunu ilerlet
		x += (ch.Advance >> 6) * scale;
}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}


