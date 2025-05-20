/* 
 *
 * Adaptado por Felipe Guimaraens de Lucena
 * para a disciplina de Processamento Gráfico - Unisinos
 * Versão inicial: 20/5/2025
 * Última atualização em 20/5/2025
 *
 */

#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

#include <cmath>

struct Triangulo {
    vec2 position;  // Posição (x, y) no mundo
    vec2 scale;     // Escala (largura, altura)
    float rotation; // Rotação em graus
    vec4 color;     // Cor (R, G, B, A)
};

// Exemplo: Lista de triângulos
vector<Triangulo> triangulos = {
    {vec2(400.0, 300.0), vec2(300.0, 300.0), 0.0f, vec4(1.0, 0.0, 0.0, 1.0)},
    {vec2(200.0, 100.0), vec2(100.0, 100.0), 30.0f, vec4(0.0, 1.0, 0.0, 1.0)}, 
    {vec2(600.0, 400.0), vec2(150.0, 150.0), -45.0f, vec4(0.0, 0.0, 1.0, 1.0)} 
};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

int setupShader();
GLuint createTriangle(float x0, float y0, float x1, float y1, float x2, float y2);


const GLuint WIDTH = 800, HEIGHT = 600;


const GLchar* vertexShaderSource = "#version 400\n"
"layout (location = 0) in vec3 position;\n"
"uniform mat4 projection;\n"
"uniform mat4 model;\n"
"void main()\n"
"{\n"
"gl_Position = projection * model * vec4(position.x, position.y, position.z, 1.0);\n"
"}\0";


const GLchar* fragmentShaderSource = "#version 400\n"
"uniform vec4 inputColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = inputColor;\n"
"}\n\0";

// Função MAIN
int main()
{

	glfwInit();

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Transformação", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	GLuint shaderID = setupShader();

	GLuint VAO = createTriangle(-0.1, -0.1, 0.1, -0.1, 0.0,0.1);
	
	glUseProgram(shaderID);

	GLint colorLoc = glGetUniformLocation(shaderID, "inputColor");

	mat4 projection = ortho(0.0, 800.0, 0.0, 600.0, -1.0, 1.0);  
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));

	mat4 model = mat4(1); //matriz identidade
	model = translate(model,vec3(400.0,300.0,0.0));
	
	//Escala
	model = scale(model,vec3(300.0,300.0,1.0));
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));


	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    // Configurações comuns (shader, projeção)
    glUseProgram(shaderID);
    glBindVertexArray(VAO); // Usa o mesmo VAO para todos os triângulos

    // Desenha cada triângulo
    for (const auto& tri : triangulos) {
        // Matriz de modelo para este triângulo
        mat4 model = mat4(1.0);
        model = translate(model, vec3(tri.position, 0.0)); // Posição
        model = rotate(model, radians(tri.rotation), vec3(0.0, 0.0, 1.0)); // Rotação
        model = scale(model, vec3(tri.scale, 1.0)); // Escala

        // Envia a matriz model para o shader
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));

        // Envia a cor para o shader
        glUniform4f(colorLoc, tri.color.r, tri.color.g, tri.color.b, tri.color.a);

        // Desenha o triângulo
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

int setupShader()
{
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Checando erros de compilação (exibição via log no terminal)
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}


GLuint createTriangle(float x0, float y0, float x1, float y1, float x2, float y2)
{

	GLfloat vertices[] = {
		//x    y    z
		//T0
		 x0 , y0 , 0.0, //v0
		 x1 , y1 , 0.0, //v1
		 x2 , y2 , 0.0, //v2				  
	};

	GLuint VBO, VAO;

	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0); 

	return VAO;
}



