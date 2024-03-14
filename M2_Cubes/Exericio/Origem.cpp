// Default libs C++.
#include <iostream>
#include <string>
#include <assert.h>

// GLAD.
#include <glad/glad.h>

// GLFW.
#include <GLFW/glfw3.h>

// GLM.
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

// Definições da janela.
const GLuint WIDTH = 1000, HEIGHT = 1000;
const char* WINDOW_TITLE = "M2_Cubes - Igor Bartmann";

// Variáveis auxiliares.
bool rotateX = false, rotateY = false, rotateZ = false;

// Função para configurar callback de entrada via teclado.
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (action != GLFW_PRESS)
	{
		return;
	}

	switch (key)
	{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_X:
			rotateX = true;
			rotateY = false;
			rotateZ = false;
			break;
		case GLFW_KEY_Y:
			rotateX = false;
			rotateY = true;
			rotateZ = false;
		case GLFW_KEY_Z:
			rotateX = false;
			rotateY = false;
			rotateZ = true;
			break;
	}
}

// Função para configurar os shaders.
int setupProgramShader()
{
	// Codigo-Fonte do Vertex Shader e Fragment Shader (em GLSL).
	const GLchar* vertexShaderSource =
		"#version 450\n"
		"layout (location = 0) in vec3 position;\n"
		"layout (location = 1) in vec3 color;\n"
		"uniform mat4 model;\n"
		"out vec4 finalColor;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = model * vec4(position, 1.0);\n"
		"   finalColor = vec4(color, 1.0);\n"
		"}\0";

	const GLchar* fragmentShaderSource =
		"#version 450\n"
		"in vec4 finalColor;\n"
		"out vec4 color;\n"
		"void main()\n"
		"{\n"
		"   color = finalColor;\n"
		"}\n\0";

	// Criar Vertex Shader.
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Chegar erros de compilação.
	GLint success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Criar Fragment Shader.
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Checar erros de compilação.
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Vinculando Vertex Shader e Fragment Shader ao Program Shader.
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Chacar erros de vinculação.
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		GLchar infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

// Função para configurar as coordenadas e buffers.
int setupGeometry()
{
	// Coordenadas (x, y, z, r, g, b).
	GLfloat vertices[] = 
	{
		-0.5, -0.5, -0.5, 1.0, 1.0, 0.0,
		-0.5, -0.5,  0.5, 0.0, 1.0, 1.0,
		0.5,  -0.5, -0.5, 1.0, 0.0, 1.0,

		-0.5, -0.5, 0.5,  1.0, 1.0, 0.0,
		0.5,  -0.5, 0.5,  0.0, 1.0, 1.0,
		0.5,  -0.5, -0.5, 1.0, 0.0, 1.0,

		-0.5, -0.5, -0.5, 1.0, 1.0, 0.0,
		0.0,  0.5,  0.0,  1.0, 1.0, 0.0,
		0.5,  -0.5, -0.5, 1.0, 1.0, 0.0,

		-0.5, -0.5, -0.5, 1.0, 0.0, 1.0,
		0.0,  0.5,  0.0,  1.0, 0.0, 1.0,
		-0.5, -0.5, 0.5,  1.0, 0.0, 1.0,

		-0.5, -0.5, 0.5, 1.0, 1.0, 0.0,
		0.0,  0.5,  0.0, 1.0, 1.0, 0.0,
		0.5,  -0.5, 0.5, 1.0, 1.0, 0.0,

		0.5, -0.5, 0.5,  0.0, 1.0, 1.0,
		0.0, 0.5,  0.0,  0.0, 1.0, 1.0,
		0.5, -0.5, -0.5, 0.0, 1.0, 1.0,
	};

	// Criar VBO.
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Criar VAO.
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Configurar atributo de posição (x, y, z).
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Configurar atributo de cor (r, g, b).
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Desvincular VBO atualmente vinculado.
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincular VAO atualmente vinculado.
	glBindVertexArray(0);

	// Retornar VAO criado.
	return VAO;
}

int main()
{
	// Inicializar GLFW.
	glfwInit();

	// Definir versão.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Criar janela GLFW.
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, WINDOW_TITLE, nullptr, nullptr);

	// Vincular janela ao contexto atual.
	glfwMakeContextCurrent(window);

	// Registrar função de callback para a janela GLFW.
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carregar todos os ponteiros das funções da OpenGL.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	// Obter e imprimir informações de versão.
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definir dimensões da view port de acordo com a janela da aplicação.
	int current_width, current_height;
	glfwGetFramebufferSize(window, &current_width, &current_height);
	glViewport(0, 0, current_width, current_height);

	// Obter a configuração do Program Shader.
	GLuint shaderID = setupProgramShader();

	// Obter a configuração geométrica.
	GLuint VAO = setupGeometry();

	// Vinculação do program shader.
	glUseProgram(shaderID);

	// Criar matriz identidade.
	glm::mat4 model = glm::mat4(1);
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	// Passar matriz identidade para a entrada (uniform) do vertex shader.
	GLint modelLoc = glGetUniformLocation(shaderID, "model");
	glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

	// Habilita teste de profundidade.
	glEnable(GL_DEPTH_TEST);

	// Laço principal da execução.
	while (!glfwWindowShouldClose(window))
	{
		// Chacar e tratar eventos de input.
		glfwPollEvents();

		// Limpar o buffer de cor.
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		model = glm::mat4(1);
		float angle = (GLfloat)glfwGetTime();
		if (rotateX)
		{
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else if (rotateY)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else if (rotateZ)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
		}

		// Passar matriz identidade para a entrada (uniform) do vertex shader.
		glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

		// Bin
		// Chamada de desenho - drawcall
		// Poligono Preenchido - GL_TRIANGLES

		// Vincular VAO.
		glBindVertexArray(VAO);

		// Chamar função de desenho.
		glDrawArrays(GL_TRIANGLES, 0, 18);
		glDrawArrays(GL_POINTS, 0, 18);
		
		// Desvincular VAO.
		glBindVertexArray(0);

		// Trocar buffer da janela.
		glfwSwapBuffers(window);
	}

	// Deleta VAO para desalocar buffer.
	glDeleteVertexArrays(1, &VAO);

	// Finalizar execução da GLFW.
	glfwTerminate();

	return 0;
}
