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
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// SHADER.
#include "Shader.h"

// MESH.
#include "Mesh.h"

// Camera.
#include "Camera.h"

#include <libconfig.h++>

// STB_IMAGE.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// USING.
using namespace std;
using namespace libconfig;

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
int load_simple_obj(string filepath, int& nVerts, glm::vec3 color = glm::vec3(1.0, 0.0, 1.0))
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

	//Envia os dados do array de floats para o buffer da OpenGL
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

GLuint load_texture(string filePath)
{
	GLuint texId;

	// Gera a textura em memória.
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);

	// Configura os parâmetros.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Carrega a imagem da textura.
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		if (nrChannels == 3) // jpg
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
		cout << "Failed to load texture" << endl;
	}

	// Limpa o espaço armazenado.
	stbi_image_free(data);

	// Desvincula a textura.
	glBindTexture(GL_TEXTURE_2D, 0);

	return texId;
}

void read_config(Config &cfg, const string &filename) {
    try {
        cfg.readFile(filename.c_str());
    } catch(const FileIOException &fioex) {
        cerr << "I/O error while reading file." << endl;
        exit(EXIT_FAILURE);
    } catch(const ParseException &pex) {
        cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                  << " - " << pex.getError() << endl;
        exit(EXIT_FAILURE);
    }
}

// Função principal do programa.
int main()
{
	Config cfg;
	read_config(cfg, "config.txt");

	// Window
	GLint window_width = cfg.lookup("window_width");
	GLint window_height = cfg.lookup("window_height");
	const char* window_title = cfg.lookup("window_title");

	// Camera
	fov = cfg.lookup("fov");
	glm::vec3 camera_position((float)cfg.lookup("position")[0], (float)cfg.lookup("position")[1], (float)cfg.lookup("position")[2]);
	glm::vec3 camera_orientation((float)cfg.lookup("orientation")[0], (float)cfg.lookup("orientation")[1], (float)cfg.lookup("orientation")[2]);

	glm::vec3 camera_view_x((float)cfg.lookup("view_x")[0], (float)cfg.lookup("view_x")[1], (float)cfg.lookup("view_x")[2]);
	glm::vec3 camera_view_y((float)cfg.lookup("view_y")[0], (float)cfg.lookup("view_y")[1], (float)cfg.lookup("view_y")[2]);
	glm::vec3 camera_view_z((float)cfg.lookup("view_z")[0], (float)cfg.lookup("view_z")[1], (float)cfg.lookup("view_z")[2]);

	// Shaders
	const char* vertex_shader_path = cfg.lookup("vertex_shader_path");
	const char* fragment_shader_path = cfg.lookup("fragment_shader_path");

	// Object 1
	const Setting& cfg_object1 = cfg.lookup("object1");
	const char* obj1_obj_path = cfg_object1.lookup("obj_path");
	const char* obj1_texture = cfg_object1.lookup("texture_path");
	glm::vec3 obj1_position((float)cfg_object1.lookup("position")[0], (float)cfg_object1.lookup("position")[1], (float)cfg_object1.lookup("position")[2]);
	const float obj1_rotation = cfg_object1.lookup("rotation");
	glm::vec3 obj1_scale((float)cfg_object1.lookup("scale")[0], (float)cfg_object1.lookup("scale")[1], (float)cfg_object1.lookup("scale")[2]);

	// Object 2
	const Setting& cfg_object2 = cfg.lookup("object2");
	const char* obj2_obj_path = cfg_object2.lookup("obj_path");
	const char* obj2_texture = cfg_object2.lookup("texture_path");
	glm::vec3 obj2_position((float)cfg_object2.lookup("position")[0], (float)cfg_object2.lookup("position")[1], (float)cfg_object2.lookup("position")[2]);
	const float obj2_rotation = cfg_object2.lookup("rotation");
	glm::vec3 obj2_scale((float)cfg_object2.lookup("scale")[0], (float)cfg_object2.lookup("scale")[1], (float)cfg_object2.lookup("scale")[2]);

	// Object 3
	const Setting& cfg_object3 = cfg.lookup("object3");
	const char* obj3_obj_path = cfg_object3.lookup("obj_path");
	const char* obj3_texture = cfg_object3.lookup("texture_path");
	glm::vec3 obj3_position((float)cfg_object3.lookup("position")[0], (float)cfg_object3.lookup("position")[1], (float)cfg_object3.lookup("position")[2]);
	const float obj3_rotation = cfg_object3.lookup("rotation");
	glm::vec3 obj3_scale((float)cfg_object3.lookup("scale")[0], (float)cfg_object3.lookup("scale")[1], (float)cfg_object3.lookup("scale")[2]);

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
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, window_title, nullptr, nullptr);

	// Vincular janela ao contexto atual.
	glfwMakeContextCurrent(window);

	// Registrar função de callback via teclado para a janela GLFW.
	glfwSetKeyCallback(window, key_callback);

	// Registrar função de callback via mouse para a janela GLFW.
	glfwSetCursorPosCallback(window, mouse_callback);

	// Registrar função de callback via scroll para a janela GLFW.
	glfwSetScrollCallback(window, scroll_callback);

	// Definir a posição do cursor.
	glfwSetCursorPos(window, ((double)window_width / 2), ((double)window_height / 2));

	// Desabilitar o desenho do cursor.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// GLAD: carregar todos os ponteiros das funções da OpenGL.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
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
	Shader shader(vertex_shader_path, fragment_shader_path);

	// Vincular o program shader.
	glUseProgram(shader.ID);

	// Carregar a texturas.
	GLuint object1_texID = load_texture(obj1_texture);
	GLuint object2_texID = load_texture(obj2_texture);
	GLuint object3_texID = load_texture(obj3_texture);

	// Câmera.
	camera.initialize((float)current_width, (float)current_height);

	camera.setCameraPosition(camera_position);

	camera.setCameraProjection(camera_view_x, camera_view_y, camera_view_z);

	// Matriz de visualização (posição e orientação da câmera).
	glm::mat4 cameraView = camera.getCameraView();
	shader.setMat4("view", glm::value_ptr(cameraView));

	// Matriz de perspectiva (definindo o volume de visualização - frustum).
	glm::mat4 cameraProjection = camera.getCameraProjection();
	shader.setMat4("projection", glm::value_ptr(cameraProjection));

	// Habilita teste de profundidade.
	glEnable(GL_DEPTH_TEST);

	// Carregar a geometria armazenada.
	int nVertsObj1, nVertsObj2, nVertsObj3;
	GLuint VAO1 = load_simple_obj(obj1_obj_path, nVertsObj1, glm::vec3(1.0, 0.0, 0.0));
	GLuint VAO2 = load_simple_obj(obj2_obj_path, nVertsObj2, glm::vec3(0.0, 1.0, 0.0));
	GLuint VAO3 = load_simple_obj(obj3_obj_path, nVertsObj3, glm::vec3(1.0, 1.0, 0.0));

	// Definir o objeto (malha) do cubo.
	Mesh object1, object2, object3;
	object1.initialize(VAO1, nVertsObj1, &shader, obj1_position, obj1_scale, obj1_rotation);
	object2.initialize(VAO2, nVertsObj2, &shader, obj2_position, obj2_scale, obj2_rotation);
	object3.initialize(VAO3, nVertsObj3, &shader, obj3_position, obj3_scale, obj3_rotation);

	//Definindo as propriedades do material da superfície
	shader.setFloat("ka", 0.2);
	shader.setFloat("kd", 0.5);
	shader.setFloat("ks", 0.5);
	shader.setFloat("q", 10.0);

	//Definindo a fonte de luz pontual
	shader.setVec3("lightPos", cfg.lookup("light_pos")[0], cfg.lookup("light_pos")[1], cfg.lookup("light_pos")[2]);
	shader.setVec3("lightColor", cfg.lookup("light_color")[0], cfg.lookup("light_color")[1], cfg.lookup("light_color")[2]);

	// Laço principal da execução.
	while (!glfwWindowShouldClose(window))
	{
		// Checar e tratar eventos de input.
		glfwPollEvents();

		// Limpar o buffer de cor.
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Definir a largura da linha e do ponto.
		glLineWidth(10);
		glPointSize(20);

		// Atualizar a posição e orientação da câmera.
		camera.recalculateCameraView();
		glm::mat4 cameraView = camera.getCameraView();
		shader.setMat4("view", glm::value_ptr(cameraView));

		// Atualizar o shader com a posição da câmera
		glm::vec3 cameraPosition = camera.getCameraPosition();
		shader.setVec3("cameraPos", cameraPosition.x, cameraPosition.y, cameraPosition.z);

		// Object 1
		// Ativação da textura.
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, object1_texID);

		// Associando o buffer de textura ao shader (será usado no fragment shader).
		shader.setInt("tex_buffer", 0);

		// Chamada de desenho - drawcall
		shader.setFloat("q", 10.0);
		object1.update();
		object1.draw();

		// Desvincular a textura.
		glBindTexture(GL_TEXTURE_2D, 0);

		// Object 2
		// Ativação da textura.
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, object2_texID);

		// Associando o buffer de textura ao shader (será usado no fragment shader).
		shader.setInt("tex_buffer", 0);

		shader.setFloat("q", 250.0);
		object2.update();
		object2.draw();

		// Desvincular a textura.
		glBindTexture(GL_TEXTURE_2D, 0);

		// Object 3
		// Ativação da textura.
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, object3_texID);

		// Associando o buffer de textura ao shader (será usado no fragment shader).
		shader.setInt("tex_buffer", 1);

		shader.setFloat("q", 1.0);
		object3.update();
		object3.draw();

		// Desvincunlar a textura.
		glBindTexture(GL_TEXTURE_2D, 0);

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
