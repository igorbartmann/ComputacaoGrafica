// Default libs C++.
#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

// GLAD.
#include <glad/glad.h>

// GLFW.
#include <GLFW/glfw3.h>

// GLM.
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// SHADER.
#include "Shader.h"

// BEZIER.
#include "Bezier.h"

// USING.
using namespace std;

// Definições da janela.
const GLuint WIDTH = 1000, HEIGHT = 1000;
const char* WINDOW_TITLE = "M6_CurvasParametricas - Igor Bartmann";

// Variáveis auxiliares para controle.
int positionSquareA, positionSquareB;
bool mover = false;

// Configurar callback de entrada via teclado.
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (action != GLFW_PRESS)
	{
		return;
	}

	if (key == GLFW_KEY_ESCAPE)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_SPACE)
	{
		mover = true;
	}
}

// Gera o conjunto de pontos para o símbolo do infinito.
vector<glm::vec3> generateControlPointsSet()
{
	vector <glm::vec3> controlPoints;

	controlPoints.push_back(glm::vec3(-0.5, 0.0, 0.0));
	controlPoints.push_back(glm::vec3(-0.375, 0.25, 0.0));
	controlPoints.push_back(glm::vec3(-0.25, 0.5, 0.0));
	controlPoints.push_back(glm::vec3(-0.125, 0.25, 0.0));
	controlPoints.push_back(glm::vec3(0.0, 0.0, 0.0));

	controlPoints.push_back(glm::vec3(0.125, -0.25, 0.0));
	controlPoints.push_back(glm::vec3(0.25, -0.5, 0.0));
	controlPoints.push_back(glm::vec3(0.375, -0.25, 0.0));
	controlPoints.push_back(glm::vec3(0.5, 0.0, 0.0));

	controlPoints.push_back(glm::vec3(0.375, 0.25, 0.0));
	controlPoints.push_back(glm::vec3(0.25, 0.5, 0.0));
	controlPoints.push_back(glm::vec3(0.125, 0.25, 0.0));
	controlPoints.push_back(glm::vec3(0.0, 0.0, 0.0));

	controlPoints.push_back(glm::vec3(-0.125, -0.25, 0.0));
	controlPoints.push_back(glm::vec3(-0.25, -0.5, 0.0));
	controlPoints.push_back(glm::vec3(-0.375, -0.25, 0.0));
	controlPoints.push_back(glm::vec3(-0.5, 0.0, 0.0));

	// - REPETE DEVIDO CALCULO DO BEZIER - //

	controlPoints.push_back(glm::vec3(-0.5, 0.0, 0.0));
	controlPoints.push_back(glm::vec3(-0.375, 0.25, 0.0));
	controlPoints.push_back(glm::vec3(-0.25, 0.5, 0.0));
	controlPoints.push_back(glm::vec3(-0.125, 0.25, 0.0));
	controlPoints.push_back(glm::vec3(0.0, 0.0, 0.0));

	controlPoints.push_back(glm::vec3(0.125, -0.25, 0.0));
	controlPoints.push_back(glm::vec3(0.25, -0.5, 0.0));
	controlPoints.push_back(glm::vec3(0.375, -0.25, 0.0));
	controlPoints.push_back(glm::vec3(0.5, 0.0, 0.0));

	controlPoints.push_back(glm::vec3(0.375, 0.25, 0.0));
	controlPoints.push_back(glm::vec3(0.25, 0.5, 0.0));
	controlPoints.push_back(glm::vec3(0.125, 0.25, 0.0));
	controlPoints.push_back(glm::vec3(0.0, 0.0, 0.0));

	controlPoints.push_back(glm::vec3(-0.125, -0.25, 0.0));
	controlPoints.push_back(glm::vec3(-0.25, -0.5, 0.0));
	controlPoints.push_back(glm::vec3(-0.375, -0.25, 0.0));
	controlPoints.push_back(glm::vec3(-0.5, 0.0, 0.0));

	return controlPoints;
}

// Gera o VAO/VBO a partir do conjunto de pontos.
GLuint generateVaoFromPointsSet(vector <glm::vec3> pointsSet)
{
	GLuint VBO, VAO;

	// Geração do identificador do VBO.
	glGenBuffers(1, &VBO);

	// Vincula o buffer.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Envia os dados do array de floats para o buffer da OpenGl.
	glBufferData(GL_ARRAY_BUFFER, pointsSet.size() * sizeof(GLfloat) * 3, pointsSet.data(), GL_STATIC_DRAW);

	// Geração do identificador do VAO.
	glGenVertexArrays(1, &VAO);

	// Vincula o VAO.
	glBindVertexArray(VAO);

	// Adiciona atributo de posição (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Desvincula o VBO.
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO.
	glBindVertexArray(0);

	return VAO;
}

// Gera o quadrado a partir do ponto.
vector <glm::vec3> generateSquareFomPoint(glm::vec3 point)
{
	vector <glm::vec3> vertices;

	GLfloat x = point.x;
	GLfloat y = point.y;
	GLfloat z = point.z;

	vertices.push_back(glm::vec3((x - 0.1f), (y + 0.1f), z));
	vertices.push_back(glm::vec3((x - 0.1f), (y - 0.1f), z));
	vertices.push_back(glm::vec3((x + 0.1f), (y - 0.1f), z));
	vertices.push_back(glm::vec3((x - 0.1f), (y + 0.1f), z));
	vertices.push_back(glm::vec3((x + 0.1f), (y + 0.1f), z));
	vertices.push_back(glm::vec3((x + 0.1f), (y - 0.1f), z));

	return vertices;
}

// Função principal do programa.
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

	// Registrar função de callback via teclado para a janela GLFW.
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

	// Obter a configuração do Program Shader.
	Shader shader("../shaders_archives/Shader.vs", "../shaders_archives/Shader.fs");

	// Vincular o program shader.
	glUseProgram(shader.ID);

	// Habilita teste de profundidade.
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);

	// Gerar o conjunto de pontos.
	std::vector<glm::vec3> controlPoints = generateControlPointsSet();

	// Bezier para gerar os pontos intermediários da curva.
	Bezier bezier;
	bezier.setControlPoints(controlPoints);
	bezier.setShader(&shader);
	bezier.generateCurve(15);

	int i = 0;
	int nbCurvePoints = bezier.getNbCurvePoints();
	
	// Laço principal da execução.	
	glm::vec3 pointBaseSquareA;
	vector <glm::vec3> pointsSquareA;
	glm::vec3 pointBaseSquareB;
	vector <glm::vec3> pointsSquareB;

	while (!glfwWindowShouldClose(window))
	{
		// Checar e tratar eventos de input.
		glfwPollEvents();

		// Definir dimensões da view port de acordo com a janela da aplicação.
		int current_width, current_height;
		glfwGetFramebufferSize(window, &current_width, &current_height);
		glViewport(0, 0, current_width, current_height);

		// Limpar o buffer de cor.
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Defibir aluta da linha e do ponto.
		glLineWidth(10);
		glPointSize(20);

		//// Desenha o conjunto de pontos resultante do Bezier.
		//bezier.drawCurve(glm::vec4(1.0, 1.0, 0.0, 1.0));

		// Gera VAO para ponto de curva A.
		pointBaseSquareA = bezier.getPointOnCurve(positionSquareA);
		pointsSquareA = generateSquareFomPoint(pointBaseSquareA);
		GLuint VaoSquareA = generateVaoFromPointsSet(pointsSquareA);

		// Vincula o VAO do ponto de curva.
		glBindVertexArray(VaoSquareA);

		// Passa a cor para o shader (fragment shader).
		shader.setVec4("finalColor", 1, 0, 0, 1);

		// Chamada de desenho.
		glDrawArrays(GL_TRIANGLES, 0, pointsSquareA.size());

		// Desvincula o VAO do ponto de curva.
		glBindVertexArray(0);

		// Gera VAO para ponto de curva B.
		pointBaseSquareB = bezier.getPointOnCurve(positionSquareB);
		pointsSquareB = generateSquareFomPoint(pointBaseSquareB);
		GLuint VaoSquareB = generateVaoFromPointsSet(pointsSquareB);

		// Vincula o VAO do ponto de curva.
		glBindVertexArray(VaoSquareB);

		// Passa a cor para o shader (fragment shader).
		shader.setVec4("finalColor", 0, 1, 0, 1);

		// Chamada de desenho.
		glDrawArrays(GL_TRIANGLES, 0, pointsSquareB.size());

		// Desvincula o VAO do ponto de curva.
		glBindVertexArray(0);

		// Recalcula a variável i.
		if (mover)
		{
			i = (i + 1) % nbCurvePoints;
			positionSquareA = i;
			positionSquareB = nbCurvePoints - i;

			mover = false;
		}

		// Troca os buffers da tela
		glfwSwapBuffers(window);

		//// Deleta os VAOs.
		//glDeleteVertexArrays(1, &VaoSquareA);
		//glDeleteVertexArrays(1, &VaoSquareB);
	}

	// Finalizar execução da GLFW.
	glfwTerminate();

	return 0;
}