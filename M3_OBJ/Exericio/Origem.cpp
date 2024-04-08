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

// STB_IMAGE.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h";

using namespace std;

// Defini��es da janela.
const GLuint WIDTH = 1000, HEIGHT = 1000;
const char* WINDOW_TITLE = "M3_OBJ - Igor Bartmann";

// Vari�veis auxiliares.
bool rotateX = false, rotateY = false, rotateZ = false, zoomIn = false, zoomOut = false;
float zoomValue = 0;

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

	// Rota��o.
	if (key == GLFW_KEY_X)
	{
		rotateX = true;
		rotateY = false;
		rotateZ = false;
	}
	if (key == GLFW_KEY_Y)
	{
		rotateX = false;
		rotateY = true;
		rotateZ = false;
	}
	if (key == GLFW_KEY_Z)
	{
		rotateX = false;
		rotateY = false;
		rotateZ = true;
	}
	if (key == GLFW_KEY_SPACE)
	{
		rotateX = false;
		rotateY = false;
		rotateZ = false;
	}

	// Escala.
	if (key == GLFW_KEY_I)
	{
		zoomIn = true;
		zoomOut = false;
	}
	if (key == GLFW_KEY_O)
	{
		zoomIn = false;
		zoomOut = true;
	}
}

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

GLuint loadTexture(string filePath)
{
	GLuint texId;

	// Gera a textura em mem�ria.
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);

	// Configura os par�metros.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Carrega a imagem da textura.
	int width, height, nrChanels;
	unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChanels, 0);
	if (data)
	{
		if (nrChanels == 3) // jpg
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else //png
		{ 
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}

		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	// Limpa o espa�o armazenado.
	stbi_image_free(data);

	// Desvincula a textura.
	glBindTexture(GL_TEXTURE_2D, 0);

	return texId;
}

int main()
{
	// Inicializar GLFW.
	glfwInit();

	// Definir vers�o.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Criar janela GLFW.
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, WINDOW_TITLE, nullptr, nullptr);

	// Vincular janela ao contexto atual.
	glfwMakeContextCurrent(window);

	// Registrar fun��o de callback para a janela GLFW.
	glfwSetKeyCallback(window, key_callback);

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

	// Carregar a textura.
	GLuint texID = loadTexture("../cube_model/cube.png");

	// Carregar a geometrica armazenada.
	int nVerts;
	GLuint VAO = loadSimpleOBJ("../cube_model/cube.obj", nVerts, glm::vec3(0, 0, 0));

	// Vincula��o do program shader.
	glUseProgram(shader.ID);

	// Associando o buffer de textura ao shader (ser� usado no fragment shader).
	glUniform1i(glGetUniformLocation(shader.ID, "tex_buffer"), 0);

	// Habilita teste de profundidade.
	glEnable(GL_DEPTH_TEST);

	// La�o principal da execu��o.
	while (!glfwWindowShouldClose(window))
	{
		// Checar e tratar eventos de input.
		glfwPollEvents();

		// Limpar o buffer de cor.
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		// Cria��o da matriz model para aplica��o de rota��o.
		glm::mat4 model = glm::mat4(1);
		if (rotateX)
		{
			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1, 0, 0));
		}
		else if (rotateY)
		{
			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0, 1, 0));
		}
		else if (rotateZ)
		{
			// Embora deveria rotacional apenas sobre o Z, rotaciona tamb�m sobre X e Y para gerar uma representa��o visual melhor.
			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1, -1, 1));
		}
		shader.setMat4("model", glm::value_ptr(model));

		
		// Cria��o da matriz projection para aplica��o da escala.
		glm::mat4 projection = glm::mat4(1);
		if (zoomIn)
		{
			zoomValue = zoomValue >= 0.7f ? zoomValue : zoomValue + 0.2f;
			zoomIn = false;
		}
		else if (zoomOut)
		{
			zoomValue = zoomValue <= -0.7f ? zoomValue : zoomValue - 0.2f;
			zoomOut = false;
		}
		projection = glm::scale(projection, glm::vec3((projection[0][0] + zoomValue) / 2, (projection[1][1] + zoomValue) / 2, (projection[2][2] + zoomValue) / 2));
		shader.setMat4("projection", glm::value_ptr(projection));

		// Ativa��o da textura.
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texID);

		// Vincular VAO.
		glBindVertexArray(VAO);

		// Chama a fun��o de desenho.
		glDrawArrays(GL_TRIANGLES, 0, nVerts);

		 // Desvincular VAO.
		glBindVertexArray(0);

		// Desvincunlar a textura.
		glBindTexture(GL_TEXTURE_2D, 0);

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}

	// Deleta VAO para desalocar buffer.
	glDeleteVertexArrays(1, &VAO);

	// Finalizar execu��o da GLFW.
	glfwTerminate();

	return 0;
}
