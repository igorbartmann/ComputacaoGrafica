// Default libs C++.
#include <assert.h>

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

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

// Libconfig.
#include <libconfig.h++>

// STB_IMAGE.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// USING.
using namespace std;
using namespace libconfig;

// Variáveis para câmera.
Camera camera;
float fov = 1.0f;

// Variáveis para movimentação dos objetos.
int selected_object_id = 0;
float zoom_object1 = 0, zoom_object2 = 0, zoom_object3 = 0;

const int CAMERA_ID = 0;

enum RotationState {
	ROTATE_NONE,
	ROTATE_TOP,
	ROTATE_DOWN,
	ROTATE_LEFT,
	ROTATE_RIGHT,
	ROTATE_RIGHT_TOP,
	ROTATE_LEFT_DOWN,
	ZOOM_IN,
	ZOOM_OUT
};

RotationState currentRotationState = ROTATE_NONE;

// Função para resetar os valores de movimentação utilizados para os objetos.
void reset_moviment_values() {
	zoom_object1 = 0;
	zoom_object2 = 0;
	zoom_object3 = 0;
	currentRotationState = ROTATE_NONE;
}

// Função para ler um arquivo de configuração.
void read_config(Config& cfg, const string& filename) {
	try {
		cfg.readFile(filename.c_str());
	} catch (const FileIOException& fioex) {
		cerr << "I/O error while reading file." << endl;
		exit(EXIT_FAILURE);
	} catch (const ParseException& pex) {
		cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine() << " - " << pex.getError() << endl;
		exit(EXIT_FAILURE);
	}
}

// Função para alterar o objeto selecionado.
void change_selectable_object() {
	// Lê o valor de selectable_objects_number a partir do arquivo de configuração.
	Config cfg;
	read_config(cfg, "config.txt");
	int selectable_objects_number = cfg.lookup("selectable_objects_number");
	selected_object_id = (selected_object_id + 1) % selectable_objects_number;
}

// Função para movimentação da câmera.
void handle_camera_movement(int key) {
	switch (key) {
		case GLFW_KEY_W:
			camera.moveFront();
			break;
		case GLFW_KEY_S:
			camera.moveBack();
			break;
		case GLFW_KEY_A:
			camera.moveLeft();
			break;
		case GLFW_KEY_D:
			camera.moveRight();
			break;
		case GLFW_KEY_SPACE:
			camera.moveUp();
			break;
		case GLFW_KEY_LEFT_CONTROL:
			camera.moveDown();
			break;
		default:
			break;
	}
}

// Função para movimentação de outros objetos
void handle_object_movement(int key) {
	currentRotationState = ROTATE_NONE;	 // Reset the state
	switch (key) {
		case GLFW_KEY_W:
			currentRotationState = ROTATE_TOP;
			break;
		case GLFW_KEY_S:
			currentRotationState = ROTATE_DOWN;
			break;
		case GLFW_KEY_A:
			currentRotationState = ROTATE_LEFT;
			break;
		case GLFW_KEY_D:
			currentRotationState = ROTATE_RIGHT;
			break;
		case GLFW_KEY_R:
			currentRotationState = ROTATE_RIGHT_TOP;
			break;
		case GLFW_KEY_E:
			currentRotationState = ROTATE_LEFT_DOWN;
			break;
		case GLFW_KEY_I:
			currentRotationState = ZOOM_IN;
			break;
		case GLFW_KEY_O:
			currentRotationState = ZOOM_OUT;
			break;
		default:
			break;
	}
}
// Função para configurar callback de entrada via teclado.
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (key == GLFW_KEY_TAB) {
			change_selectable_object();
			reset_moviment_values();
			return;
		}

		if (key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(window, GL_TRUE);
			return;
		}

		if (selected_object_id == CAMERA_ID) {
			handle_camera_movement(key);
		} else {
			handle_object_movement(key);
		}
	}
}

// Função para configurar o callback de entrada via mouse.
void mouse_callback(GLFWwindow* window, double xpos, double ypos) { camera.updateMatrixByMousePosition(xpos, ypos); }

// Função para configurar o callback de entrada via scroll.
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (fov >= 1.0f && fov <= 45.0f) {
		fov -= yoffset;
	} else if (fov <= 1.0f) {
		fov = 1.0f;
	} else if (fov >= 45.0f) {
		fov = 45.0f;
	}
}

// Função para carregar um arquivo obj.
int load_simple_obj(string filepath, int& nVerts, glm::vec3 color = glm::vec3(1.0, 0.0, 1.0)) {
	vector<glm::vec3> vertices;
	vector<GLuint> indices;
	vector<glm::vec2> texCoords;
	vector<glm::vec3> normals;
	vector<GLfloat> vbuffer;

	ifstream inputFile;
	inputFile.open(filepath.c_str());
	if (inputFile.is_open()) {
		char line[100];
		string sline;

		while (!inputFile.eof()) {
			inputFile.getline(line, 100);
			sline = line;

			string word;
			istringstream ssline(line);
			ssline >> word;

			if (word == "v") {
				glm::vec3 v;
				ssline >> v.x >> v.y >> v.z;
				vertices.push_back(v);
			}
			if (word == "vt") {
				glm::vec2 vt;
				ssline >> vt.s >> vt.t;
				texCoords.push_back(vt);
			}
			if (word == "vn") {
				glm::vec3 vn;
				ssline >> vn.x >> vn.y >> vn.z;
				normals.push_back(vn);
			}
			if (word == "f") {
				string tokens[3];
				ssline >> tokens[0] >> tokens[1] >> tokens[2];

				for (int i = 0; i < 3; i++) {
					// Recuperando os indices de v
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

					// Recuperando os indices de vts
					tokens[i] = tokens[i].substr(pos + 1);
					pos = tokens[i].find("/");
					token = tokens[i].substr(0, pos);
					index = atoi(token.c_str()) - 1;

					vbuffer.push_back(texCoords[index].s);
					vbuffer.push_back(texCoords[index].t);

					// Recuperando os indices de vns
					tokens[i] = tokens[i].substr(pos + 1);
					index = atoi(tokens[i].c_str()) - 1;

					vbuffer.push_back(normals[index].x);
					vbuffer.push_back(normals[index].y);
					vbuffer.push_back(normals[index].z);
				}
			}
		}
	} else {
		cout << "Problema ao encontrar o arquivo " << filepath << endl;
	}

	inputFile.close();

	GLuint VBO, VAO;

	nVerts = vbuffer.size() / 11;

	// Geração do identificador do VBO
	glGenBuffers(1, &VBO);

	// Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Envia os dados do array de floats para o buffer da OpenGL
	glBufferData(GL_ARRAY_BUFFER, vbuffer.size() * sizeof(GLfloat), vbuffer.data(), GL_STATIC_DRAW);

	// Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);

	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos
	glBindVertexArray(VAO);

	// Atributo posição (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Atributo coordenada de textura (s, t)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// Atributo normal do vértice (x, y, z)
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de
	// vértice atualmente vinculado - para que depois possamos desvincular com segurança.
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}

// Função para carregar uma textura.
GLuint load_texture(string filePath) {
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
	if (data) {
		if (nrChannels == 3)  // jpg
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		} else	// png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}

		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		cout << "Failed to load texture" << endl;
	}

	// Limpa o espaço armazenado.
	stbi_image_free(data);

	// Desvincula a textura.
	glBindTexture(GL_TEXTURE_2D, 0);

	return texId;
}

// Função utilitária para analisar o arquivo OBJ e obter o caminho completo do arquivo MTL
std::string getMTLFilePath(const std::string& objFilePath) {
	std::ifstream objFile(objFilePath);
	std::string line;
	std::string mtlFilePath;

	if (!objFile.is_open()) {
		std::cerr << "Falha ao abrir o arquivo OBJ: " << objFilePath << std::endl;
		return "";
	}

	while (std::getline(objFile, line)) {
		std::istringstream iss(line);
		std::string word;
		iss >> word;
		if (word == "mtllib") {
			iss >> mtlFilePath;
			break;
		}
	}

	objFile.close();

	// Extrai o diretório do caminho relativo do arquivo OBJ
	std::filesystem::path objPath(objFilePath);
	std::filesystem::path objDirectory = objPath.parent_path();

	// Constrói o caminho completo para o arquivo MTL
	std::filesystem::path fullMtlPath = objDirectory / mtlFilePath;

	return fullMtlPath.string();
}

struct Material {
	glm::vec3 Ka;  // Ambient color
	glm::vec3 Kd;  // Diffuse color
	glm::vec3 Ks;  // Specular color
	glm::vec3 Ke;  // Emissive color
	float Ns;	   // Specular exponent
	float Ni;	   // Optical density
	float d;	   // Dissolve
	int illum;	   // Illumination model
};

Material parseMTL(const std::string& mtlFilePath) {
	std::ifstream mtlFile(mtlFilePath);
	std::string line;
	Material material;

	if (!mtlFile.is_open()) {
		std::cerr << "Failed to open MTL file: " << mtlFilePath << std::endl;
		return material;
	}

	while (std::getline(mtlFile, line)) {
		std::istringstream iss(line);
		std::string word;
		iss >> word;
		if (word == "Ka") {
			iss >> material.Ka.r >> material.Ka.g >> material.Ka.b;
		} else if (word == "Kd") {
			iss >> material.Kd.r >> material.Kd.g >> material.Kd.b;
		} else if (word == "Ks") {
			iss >> material.Ks.r >> material.Ks.g >> material.Ks.b;
		} else if (word == "Ke") {
			iss >> material.Ke.r >> material.Ke.g >> material.Ke.b;
		} else if (word == "Ni") {
			iss >> material.Ni;
		} else if (word == "d") {
			iss >> material.d;
		} else if (word == "illum") {
			iss >> material.illum;
		}
	}

	mtlFile.close();
	return material;
}

void setMaterialProperties(Shader& shader, const Material& material) {
	shader.setVec3("ka", material.Ka.r, material.Ka.g, material.Ka.b);
	shader.setVec3("kd", material.Kd.r, material.Kd.g, material.Kd.b);
	shader.setVec3("ks", material.Ks.r, material.Ks.g, material.Ks.b);
	shader.setVec3("ke", material.Ke.r, material.Ke.g, material.Ke.b);
	shader.setFloat("ns", material.Ns);
	shader.setFloat("ni", material.Ni);
	shader.setFloat("d", material.d);
	shader.setInt("illum", material.illum);
}

// Função para atualizar os valores das matrizes modelo e projeção do objeto para movimentação.
void update_object_matrix_to_move(int object_id, glm::mat4& model, glm::mat4& projection, float& zoom) {
	if (object_id != selected_object_id) {
		return;
	}

	switch (currentRotationState) {
		case ROTATE_TOP:
			model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1, 0, 0));
			break;
		case ROTATE_DOWN:
			model = glm::rotate(model, glm::radians(45.0f), glm::vec3(-1, 0, 0));
			break;
		case ROTATE_LEFT:
			model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0, 1, 0));
			break;
		case ROTATE_RIGHT:
			model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0, -1, 0));
			break;
		case ROTATE_RIGHT_TOP:
			model = glm::rotate(model, glm::radians(45.0f), glm::vec3(-1, -1, 0));
			break;
		case ROTATE_LEFT_DOWN:
			model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1, 1, 0));
			break;
		case ZOOM_IN:
			zoom += 0.5f;
			break;
		case ZOOM_OUT:
			zoom -= 0.5f;
			break;
		default:
			break;
	}

	// Apply zoom to the projection matrix
	if (currentRotationState == ZOOM_IN || currentRotationState == ZOOM_OUT) {
		projection = glm::perspective(glm::radians(45.0f + zoom), 800.0f / 600.0f, 0.1f, 100.0f);
	}

	// Reset the rotation state after applying the transformation
	currentRotationState = ROTATE_NONE;
}

// Função para renderizar o objeto.
void handle_object_render(Shader& shader, Mesh object, glm::mat4& model, glm::mat4& projection, float& zoom, GLuint texture_id, Material material) {
	// Atualização das matrizes de modelo e projeção.
	update_object_matrix_to_move(object.getId(), model, projection, zoom);
	shader.setMat4("model", glm::value_ptr(model));
	shader.setMat4("projection", glm::value_ptr(projection));

	// Definição do material da superficie (textura).
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glUniform1i(glGetUniformLocation(shader.ID, "diffuseMap"), texture_id);

	// Setando valores de iluminação para o shader.
	setMaterialProperties(shader, material);

	// Associando o buffer de textura ao shader (será usado no fragment shader).
	shader.setInt("tex_buffer", 0);

	// Chamada de desenho - drawcall.
	object.update(model);
	object.draw();

	// Desvincular a textura.
	glBindTexture(GL_TEXTURE_2D, 0);
}

// Função principal do programa.
int main() {
	Config cfg;
	read_config(cfg, "config.txt");

	// Window
	GLint window_width = cfg.lookup("window_width");
	GLint window_height = cfg.lookup("window_height");
	const char* window_title = cfg.lookup("window_title");

	// Camera
	fov = cfg.lookup("fov");
	glm::vec3 camera_position((float)cfg.lookup("position")[0], (float)cfg.lookup("position")[1],
							  (float)cfg.lookup("position")[2]);
	glm::vec3 camera_orientation((float)cfg.lookup("orientation")[0], (float)cfg.lookup("orientation")[1],
								 (float)cfg.lookup("orientation")[2]);

	glm::vec3 camera_view_x((float)cfg.lookup("view_x")[0], (float)cfg.lookup("view_x")[1],
							(float)cfg.lookup("view_x")[2]);
	glm::vec3 camera_view_y((float)cfg.lookup("view_y")[0], (float)cfg.lookup("view_y")[1],
							(float)cfg.lookup("view_y")[2]);
	glm::vec3 camera_view_z((float)cfg.lookup("view_z")[0], (float)cfg.lookup("view_z")[1],
							(float)cfg.lookup("view_z")[2]);

	// Shaders
	const char* vertex_shader_path = cfg.lookup("vertex_shader_path");
	const char* fragment_shader_path = cfg.lookup("fragment_shader_path");

	// Object 1
	const Setting& cfg_object1 = cfg.lookup("object1");
	glm::vec3 obj1_position((float)cfg_object1.lookup("position")[0], (float)cfg_object1.lookup("position")[1],
							(float)cfg_object1.lookup("position")[2]);
	glm::vec3 obj1_scale((float)cfg_object1.lookup("scale")[0], (float)cfg_object1.lookup("scale")[1],
						 (float)cfg_object1.lookup("scale")[2]);

	// Object 2
	const Setting& cfg_object2 = cfg.lookup("object2");
	glm::vec3 obj2_position((float)cfg_object2.lookup("position")[0], (float)cfg_object2.lookup("position")[1],
							(float)cfg_object2.lookup("position")[2]);
	glm::vec3 obj2_scale((float)cfg_object2.lookup("scale")[0], (float)cfg_object2.lookup("scale")[1],
						 (float)cfg_object2.lookup("scale")[2]);

	// Object 3
	const Setting& cfg_object3 = cfg.lookup("object3");
	glm::vec3 obj3_position((float)cfg_object3.lookup("position")[0], (float)cfg_object3.lookup("position")[1],
							(float)cfg_object3.lookup("position")[2]);
	glm::vec3 obj3_scale((float)cfg_object3.lookup("scale")[0], (float)cfg_object3.lookup("scale")[1],
						 (float)cfg_object3.lookup("scale")[2]);

	// Object 4
	const Setting& cfg_object4 = cfg.lookup("object4");
	glm::vec3 obj4_position((float)cfg_object4.lookup("position")[0], (float)cfg_object4.lookup("position")[1],
							(float)cfg_object4.lookup("position")[2]);
	glm::vec3 obj4_scale((float)cfg_object4.lookup("scale")[0], (float)cfg_object4.lookup("scale")[1],
						 (float)cfg_object4.lookup("scale")[2]);

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
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
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
	GLuint object1_texID = load_texture(cfg_object1.lookup("texture_path"));
	GLuint object2_texID = load_texture(cfg_object2.lookup("texture_path"));
	GLuint object3_texID = load_texture(cfg_object3.lookup("texture_path"));
	GLuint object4_texID = load_texture(cfg_object4.lookup("texture_path"));

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
	int nVertsObj1, nVertsObj2, nVertsObj3, nVertsObj4;
	GLuint VAO1 = load_simple_obj(cfg_object1.lookup("obj_path"), nVertsObj1, glm::vec3(1.0, 0.0, 0.0));
	GLuint VAO2 = load_simple_obj(cfg_object2.lookup("obj_path"), nVertsObj2, glm::vec3(0.0, 1.0, 0.0));
	GLuint VAO3 = load_simple_obj(cfg_object3.lookup("obj_path"), nVertsObj3, glm::vec3(1.0, 1.0, 0.0));
	GLuint VAO4 = load_simple_obj(cfg_object4.lookup("obj_path"), nVertsObj4, glm::vec3(1.0, 1.0, 0.0));

	// Definir a malha dos objetos.
	Mesh object1, object2, object3, object4;
	object1.initialize(1, VAO1, nVertsObj1, &shader, obj1_position, obj1_scale, cfg_object1.lookup("rotation"));
	object2.initialize(2, VAO2, nVertsObj2, &shader, obj2_position, obj2_scale, cfg_object2.lookup("rotation"));
	object3.initialize(3, VAO3, nVertsObj3, &shader, obj3_position, obj3_scale, cfg_object3.lookup("rotation"));
	object4.initialize(4, VAO4, nVertsObj4, &shader, obj4_position, obj4_scale, cfg_object4.lookup("rotation"));

	// Definiar material dos objetos
	Material obj1_material = parseMTL(getMTLFilePath(cfg_object1.lookup("obj_path")));
	Material obj2_material = parseMTL(getMTLFilePath(cfg_object2.lookup("obj_path")));
	Material obj3_material = parseMTL(getMTLFilePath(cfg_object3.lookup("obj_path")));
	Material obj4_material = parseMTL(getMTLFilePath(cfg_object4.lookup("obj_path")));

	// Definindo a fonte de luz pontual
	shader.setVec3("lightPos", cfg.lookup("light_pos")[0], cfg.lookup("light_pos")[1], cfg.lookup("light_pos")[2]);
	shader.setVec3("lightColor", cfg.lookup("light_color")[0], cfg.lookup("light_color")[1],
				   cfg.lookup("light_color")[2]);

	float planetRotationAngle = 0.0f;
	glm::vec3 planetTranslation(0.0f, 0.0f, 0.0f);

	// Declaração das matrizes de cada objeto.
	// Para que não percam a posição do último movimento quando não estiverem selecionados para movimentação.
	glm::mat4 model_object1 = glm::mat4(1);
	glm::mat4 model_object2 = glm::mat4(1);
	glm::mat4 model_object3 = glm::mat4(1);

	glm::mat4 projection_object1 = camera.getCameraProjection();
	glm::mat4 projection_object2 = camera.getCameraProjection();
	glm::mat4 projection_object3 = camera.getCameraProjection();
	glm::mat4 projection_object4 = camera.getCameraProjection();

	// glm::mat4 projection =
	// Laço principal da execução.
	while (!glfwWindowShouldClose(window)) {
		// Checar e tratar eventos de input.
		glfwPollEvents();

		// Limpar o buffer de cor.
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Definir a largura da linha e do ponto.
		glLineWidth(10);
		glPointSize(20);

		// Atualizar a posição e orientação da câmera.
		camera.recalculateCameraView();
		glm::mat4 cameraView = camera.getCameraView();
		shader.setMat4("view", glm::value_ptr(cameraView));

		// Atualizar o shader com a posição da câmera.
		glm::vec3 cameraPosition = camera.getCameraPosition();
		shader.setVec3("cameraPos", cameraPosition.x, cameraPosition.y, cameraPosition.z);

		// Renderização do Objeto 1.
		handle_object_render(shader, object1, model_object1, projection_object1, zoom_object1, object1_texID,
							 obj1_material);
		
		// Renderização do Objeto 2.
		handle_object_render(shader, object2, model_object2, projection_object2, zoom_object2, object2_texID,
							 obj2_material);	

		// Renderização do Objeto 3.
		handle_object_render(shader, object3, model_object3, projection_object3, zoom_object3, object3_texID,
							 obj3_material);

		// Renderização do Objeto 4.
		// Definição do material da superfície (textura).
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, object4_texID);
		glUniform1i(glGetUniformLocation(shader.ID, "diffuseMap"), object4_texID);

		// Setando os valores de iluminação para o shader.
		setMaterialProperties(shader, obj4_material);

		// Associando o buffer de textura ao shader (será usado no fragment shader).
		shader.setInt("tex_buffer", 0);

		// Calculando ângulo de rotação do objeto.
		planetRotationAngle += 0.01f;
		if (planetRotationAngle > 360.0f) {
			planetRotationAngle -= 360.0f;
		}

		// Criação de uma órbita circular.
		float orbitRadius = 10.0f;
		planetTranslation.x = orbitRadius * cos(planetRotationAngle);
		planetTranslation.z = orbitRadius * sin(planetRotationAngle);
		planetTranslation.y = 3.0f;

		// Criando matrizes de Rotação e Transalação.
		glm::mat4 rotation =
			glm::rotate(glm::mat4(1.0f), glm::radians(planetRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), planetTranslation);

		// Aplicando a transformação a partir da translação e rotação.
		glm::mat4 planetTransform = translation * rotation;
		float modelArray[16];
		memcpy(modelArray, glm::value_ptr(planetTransform * glm::scale(glm::mat4(1.0f), obj4_scale)),
			   sizeof(float) * 16);
		shader.setMat4("model", modelArray);
		shader.setMat4("projection", glm::value_ptr(projection_object4));

		// Chamada de desenho - drawcall.
		object4.draw();

		// Desvincunlar a textura.
		glBindTexture(GL_TEXTURE_2D, 0);

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}

	// Deleta VAO para desalocar buffer.
	glDeleteVertexArrays(1, &VAO1);
	glDeleteVertexArrays(1, &VAO2);
	glDeleteVertexArrays(1, &VAO3);
	glDeleteVertexArrays(1, &VAO4);

	// Finalizar execução da GLFW.
	glfwTerminate();

	return 0;
}
