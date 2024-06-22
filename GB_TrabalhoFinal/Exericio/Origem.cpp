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

// Defini��es da janela.
const GLuint WIDTH = 1000, HEIGHT = 1000;
const char* WINDOW_TITLE = "Trabalho Final - GB (Igor Bartmann e Lucas)";

// Vari�veis auxiliares.
Camera camera;
float fov = 1.0f;

// Fun��o para configurar callback de entrada via teclado.
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

// Fun��o para configurar o callback de entrada via mouse.
double mouseX, mouseY;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	camera.updateMatrixByMousePosition(xpos, ypos);
	mouseX = xpos;
    mouseY = ypos;
}


// Fun��o para configurar o callback de entrada via scroll.
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

// Fun��o para carregar um arquivo obj.
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

	//Gera��o do identificador do VBO
	glGenBuffers(1, &VBO);

	//Faz a conex�o (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, vbuffer.size() * sizeof(GLfloat), vbuffer.data(), GL_STATIC_DRAW);

	//Gera��o do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);

	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de v�rtices
	// e os ponteiros para os atributos
	glBindVertexArray(VAO);

	//Atributo posi��o (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Atributo coordenada de textura (s, t)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	//Atributo normal do v�rtice (x, y, z)
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	// Observe que isso � permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de v�rtice
	// atualmente vinculado - para que depois possamos desvincular com seguran�a.
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (� uma boa pr�tica desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}


GLuint crosshairVAO, crosshairVBO;
void initCrosshair() {
    float crosshairVertices[] = {
        0.0f, 0.0f // Center of the screen
    };

    glGenVertexArrays(1, &crosshairVAO);
    glGenBuffers(1, &crosshairVBO);

    glBindVertexArray(crosshairVAO);

    glBindBuffer(GL_ARRAY_BUFFER, crosshairVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


// Fun��o principal do programa.
int main()
{
	// Inicializar GLFW.
	glfwInit();

	// Definir vers�o.
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

	// Registrar fun��o de callback via teclado para a janela GLFW.
	glfwSetKeyCallback(window, key_callback);

	// Registrar fun��o de callback via mouse para a janela GLFW.
	glfwSetCursorPosCallback(window, mouse_callback);

	// Registrar fun��o de callback via scroll para a janela GLFW.
	glfwSetScrollCallback(window, scroll_callback);

	// Definir a posi��o do cursor.
	glfwSetCursorPos(window, ((double)WIDTH / 2), ((double)HEIGHT / 2));

	// Desabilitar o desenho do cursor.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// GLAD: carregar todos os ponteiros das fun��es da OpenGL.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	// Obter e imprimir informa��es de vers�o.
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definir dimens�es da view port de acordo com a janela da aplica��o.
	int current_width, current_height;
	glfwGetFramebufferSize(window, &current_width, &current_height);
	glViewport(0, 0, current_width, current_height);

	// Obter a configura��o do Program Shader.
	Shader shader("../shaders_archives/Shader.vs", "../shaders_archives/Shader.fs");

	// Crosshair
	initCrosshair();
	Shader crosshairShader("../shaders_archives/crosshair.vs", "../shaders_archives/crosshair.fs");

	// Vincular o program shader.
	glUseProgram(shader.ID);

	// C�mera.
	camera.initialize((float)current_width, (float)current_height);

	// Matriz de visualiza��o (posi��o e orienta��o da c�mera).
	glm::mat4 cameraView = camera.getCameraView();
	shader.setMat4("view", value_ptr(cameraView));

	// Matriz de perspectiva (definindo o volume de visualiza��o - frustum).
	glm::mat4 cameraProjection = camera.getCameraProjection();
	shader.setMat4("projection", glm::value_ptr(cameraProjection));

	// Habilita teste de profundidade.
	glEnable(GL_DEPTH_TEST);

	// Carregar a geometrica armazenada.
	int nVerts;
	GLuint VAO1 = loadSimpleOBJ("../cube_model/cube.obj", nVerts, glm::vec3(1.0, 0.0, 0.0));
	GLuint VAO2 = loadSimpleOBJ("../cube_model/cube.obj", nVerts, glm::vec3(0.0, 1.0, 0.0));
	GLuint VAO3 = loadSimpleOBJ("../cube_model/cube.obj", nVerts, glm::vec3(1.0, 1.0, 1.0));

	// Definir o objeto (malha) do cubo.
	Mesh cube1, cube2, cube3;
	cube1.initialize(VAO1, nVerts, &shader, glm::vec3(-2.75, 0.0, 0.0));
	cube2.initialize(VAO2, nVerts, &shader);
	cube3.initialize(VAO3, nVerts, &shader, glm::vec3(2.75, 0.0, 0.0));

	//Definindo as propriedades do material da superficie
	shader.setFloat("ka", 0.2);
	shader.setFloat("kd", 0.5);
	shader.setFloat("ks", 0.5);
	shader.setFloat("q", 10.0);

	//Definindo a fonte de luz pontual
	shader.setVec3("lightPos", -2.0, 10.0, 2.0);
	shader.setVec3("lightColor", 1.0, 1.0, 0.0);

	// La�o principal da execu��o.
	while (!glfwWindowShouldClose(window))
	{
		glUseProgram(shader.ID);
		// Checar e tratar eventos de input.
		glfwPollEvents();

		// Limpar o buffer de cor.
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Defibir aluta da linha e do ponto.
		glLineWidth(10);
		glPointSize(20);

		// Atualizar a posi��o e orienta��o da c�mera.
		camera.recalculateCameraView();
		glm::mat4 cameraView = camera.getCameraView();
		shader.setMat4("view", glm::value_ptr(cameraView));

		// Atualizar o shader com a posi��o da c�mera
		glm::vec3 cameraPosition = camera.getCameraPosition();
		shader.setVec3("cameraPos", cameraPosition.x, cameraPosition.y, cameraPosition.z);

		// Chamada de desenho - drawcall
		shader.setFloat("q", 10.0);
		cube1.update();
		cube1.draw();

		shader.setFloat("q", 1.0);
		cube2.update();
		cube2.draw();

		shader.setFloat("q", 250.0);
		cube3.update();
		cube3.draw();

		glUseProgram(crosshairShader.ID);
		glBindVertexArray(crosshairVAO);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(0);

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}

	// Deleta VAO para desalocar buffer.
	glDeleteVertexArrays(1, &VAO1);
	glDeleteVertexArrays(1, &VAO2);
	glDeleteVertexArrays(1, &VAO3);
	glDeleteVertexArrays(1, &crosshairVAO);

	// Finalizar execu��o da GLFW.
	glfwTerminate();

	return 0;
}
