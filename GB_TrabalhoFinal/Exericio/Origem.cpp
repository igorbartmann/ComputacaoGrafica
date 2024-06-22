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

// MESH.
#include "Mesh.h"

// Camera.
#include "Camera.h"

// STB_IMAGE.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// USING.
using namespace std;

// Definições da janela.
const GLuint WIDTH = 1000, HEIGHT = 1000;
const char* WINDOW_TITLE = "Trabalho Final - GB (Igor Bartmann e Lucas)";

// Variáveis auxiliares.
Camera camera;
float fov = 1.0f;

// Função para configurar callback de entrada via teclado.
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

	// Movimento.
	if (key == GLFW_KEY_W)
	{
		camera.moveFront();
	}
	else if (key == GLFW_KEY_S)
	{
		camera.moveBack();
	}
	else if (key == GLFW_KEY_A)
	{
		camera.moveLeft();
	}
	else if (key == GLFW_KEY_D)
	{
		camera.moveRight();
	}
}

// Função para configurar o callback de entrada via mouse.
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	camera.updateMatrixByMousePosition(xpos, ypos);
}

// Função para configurar o callback de entrada via scroll.
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
	{
		fov -= yoffset;
	}
	else if (fov <= 1.0f) 
	{
		fov = 1.0f;
	}
	else if (fov >= 45.0f)
	{
		fov = 45.0f;
	}
}

// Função para carregar um arquivo obj.
int loadSimpleOBJ(string filepath, int& nVerts, glm::vec3 color = glm::vec3(1.0, 0.0, 1.0))
{
	vector <glm::vec3> vertices;
	vector <GLuint> indices;
	vector <glm::vec2> texCoords;
	vector <glm::vec3> normals;
	vector <GLfloat> vbuffer;

	ifstream inputFile;
	inputFile.open(filepath.c_str());
	if (inputFile.is_open())
	{
		char line[100];
		string sline;

		while (!inputFile.eof())
		{
			inputFile.getline(line, 100);
			sline = line;

			string word;
			istringstream ssline(line);
			ssline >> word;

			if (word == "v")
			{
				glm::vec3 v;
				ssline >> v.x >> v.y >> v.z;
				vertices.push_back(v);
			}
			if (word == "vt")
			{
				glm::vec2 vt;
				ssline >> vt.s >> vt.t;
				texCoords.push_back(vt);
			}
			if (word == "vn")
			{
				glm::vec3 vn;
				ssline >> vn.x >> vn.y >> vn.z;
				normals.push_back(vn);
			}
			if (word == "f")
			{
				string tokens[3];
				ssline >> tokens[0] >> tokens[1] >> tokens[2];

				for (int i = 0; i < 3; i++)
				{
					//Recuperando os indices de v
					int pos = tokens[i].find("/");
					string token = tokens[i].substr(0, pos);
					int index = atoi(token.c_str()) - 1;
					indices.push_back(index);

					vbuffer.push_back(vertices[index].x);
					vbuffer.push_back(vertices[index].y);
					vbuffer.push_back(vertices[index].z);
					vbuffer.push_back(color.r);
					vbuffer.push_back(color.g);
					vbuffer.push_back(color.b);

					//Recuperando os indices de vts
					tokens[i] = tokens[i].substr(pos + 1);
					pos = tokens[i].find("/");
					token = tokens[i].substr(0, pos);
					index = atoi(token.c_str()) - 1;

					vbuffer.push_back(texCoords[index].s);
					vbuffer.push_back(texCoords[index].t);

					//Recuperando os indices de vns
					tokens[i] = tokens[i].substr(pos + 1);
					index = atoi(tokens[i].c_str()) - 1;

					vbuffer.push_back(normals[index].x);
					vbuffer.push_back(normals[index].y);
					vbuffer.push_back(normals[index].z);
				}
			}
		}
	}
	else
	{
		cout << "Problema ao encontrar o arquivo " << filepath << endl;
	}

	inputFile.close();

	GLuint VBO, VAO;

	nVerts = vbuffer.size() / 11;

	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);

	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, vbuffer.size() * sizeof(GLfloat), vbuffer.data(), GL_STATIC_DRAW);

	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);

	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);

	//Atributo posição (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Atributo coordenada de textura (s, t)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	//Atributo normal do vértice (x, y, z)
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança.
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}

// Função principal do programa.
int main()
{
	// Inicializar GLFW.
	glfwInit();

	// Definir versão.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Criar janela GLFW.
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, WINDOW_TITLE, nullptr, nullptr);

	// Vincular janela ao contexto atual.
	glfwMakeContextCurrent(window);

	// Registrar função de callback via teclado para a janela GLFW.
	glfwSetKeyCallback(window, key_callback);

	// Registrar função de callback via mouse para a janela GLFW.
	glfwSetCursorPosCallback(window, mouse_callback);

	// Registrar função de callback via scroll para a janela GLFW.
	glfwSetScrollCallback(window, scroll_callback);

	// Definir a posição do cursor.
	glfwSetCursorPos(window, ((double)WIDTH / 2), ((double)HEIGHT / 2));

	// Desabilitar o desenho do cursor.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
	Shader shader("../shaders_archives/Shader.vs", "../shaders_archives/Shader.fs");

	// Vincular o program shader.
	glUseProgram(shader.ID);

	// Câmera.
	camera.initialize((float)current_width, (float)current_height);

	// Matriz de visualização (posição e orientação da câmera).
	glm::mat4 cameraView = camera.getCameraView();
	shader.setMat4("view", value_ptr(cameraView));

	// Matriz de perspectiva (definindo o volume de visualização - frustum).
	glm::mat4 cameraProjection = camera.getCameraProjection();
	shader.setMat4("projection", glm::value_ptr(cameraProjection));

	// Habilita teste de profundidade.
	glEnable(GL_DEPTH_TEST);

	// Carregar a geometrica armazenada.
	int nVertsCube1, nVertsSuzanne, nVertsCube2;
	GLuint VAO1 = loadSimpleOBJ("../models_archives/cube_model/cube.obj", nVertsCube1, glm::vec3(1.0, 0.0, 0.0));
	GLuint VAO2 = loadSimpleOBJ("../models_archives/suzanne_model/suzanneTriLowPoly.obj", nVertsSuzanne, glm::vec3(0.0, 1.0, 0.0));
	GLuint VAO3 = loadSimpleOBJ("../models_archives/cube_model/cube.obj", nVertsCube2, glm::vec3(1.0, 1.0, 0.0));

	// Definir o objeto (malha) do cubo.
	Mesh cube1, suzanne, cube2;
	cube1.initialize(VAO1, nVertsCube1, &shader, glm::vec3(-2.0, 0.0, 0.0));
	suzanne.initialize(VAO2, nVertsSuzanne, &shader, glm::vec3(0.0, 0.0, 0.0));
	cube2.initialize(VAO3, nVertsCube2, &shader, glm::vec3(2.0, 0.0, 0.0));

	//Definindo as propriedades do material da superficie
	shader.setFloat("ka", 0.2);
	shader.setFloat("kd", 0.5);
	shader.setFloat("ks", 0.5);
	shader.setFloat("q", 10.0);

	//Definindo a fonte de luz pontual
	shader.setVec3("lightPos", -2.0, 10.0, 2.0);
	shader.setVec3("lightColor", 1.0, 1.0, 0.0);

	// Laço principal da execução.
	while (!glfwWindowShouldClose(window))
	{
		// Checar e tratar eventos de input.
		glfwPollEvents();

		// Limpar o buffer de cor.
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Defibir aluta da linha e do ponto.
		glLineWidth(10);
		glPointSize(20);

		// Atualizar a posição e orientação da câmera.
		camera.recalculateCameraView();
		glm::mat4 cameraView = camera.getCameraView();
		shader.setMat4("view", glm::value_ptr(cameraView));

		// Atualizar o shader com a posição da câmera
		glm::vec3 cameraPosition = camera.getCameraPosition();
		shader.setVec3("cameraPos", cameraPosition.x, cameraPosition.y, cameraPosition.z);

		// Chamada de desenho - drawcall
		shader.setFloat("q", 10.0);
		cube1.update();
		cube1.draw();

		shader.setFloat("q", 1.0);
		suzanne.update();
		suzanne.draw();

		shader.setFloat("q", 250.0);
		cube2.update();
		cube2.draw();

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}

	// Deleta VAO para desalocar buffer.
	glDeleteVertexArrays(1, &VAO1);
	glDeleteVertexArrays(1, &VAO2);
	glDeleteVertexArrays(1, &VAO3);

	// Finalizar execução da GLFW.
	glfwTerminate();

	return 0;
}
