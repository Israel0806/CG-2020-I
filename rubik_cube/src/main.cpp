#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include "shader.h"
#include "camera.h"

using namespace std;
using namespace glm;

string path = "D:/UCSP/Semestres/Semestre_VII/Computacion_Grafica/CG-2020-I/Proyecto_final/src/rubik_cube/";

int screenWidth = 800, screenHeight = 600;
unsigned int VBO, VAO, EBO, lightVAO;
Shader *shader, *lightShader;
mat4 model, view, projection;

// timing
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float lastX = screenWidth / 2, lastY = screenHeight / 2;
bool firstMouse = true;

// lighting
vec3 lightPos (3.2f, 0.0f, 3.0f);

Camera camera (vec3 (0.0f, 2.0f, 6.0f));

/// para cada cubo tiene que haber 6 colores

vec3 cubePositions[] = {
	/// de espacio = 0.1
	// CENTER  -0.5 a 0.5
	vec3 (0.0f,0.0f,0.0f),
	/// UP
	vec3 (0.f,   1.05f,  0.0f), // center
	vec3 (0.0f,  1.05f,  1.05f), // center up
	vec3 (0.0f,  1.05f, -1.05f), // center down
	vec3 (1.05f,  1.05f,  0.0f), // right up
	vec3 (1.05f,  1.05f,  1.05f), // right mid
	vec3 (1.05f,  1.05f, -1.05f), // right down
	vec3 (-1.05f, 1.05f,  0.0f), // left up
	vec3 (-1.05f, 1.05f,  1.05f), // left mid
	vec3 (-1.05f, 1.05f, -1.05f), // left down

	// MIDDLE
	vec3 (0.0f,  0.0f,  1.05f), // center up
	vec3 (0.0f,  0.0f, -1.05f), // center down
	vec3 (1.05f,  0.0f,  0.0f), // right up
	vec3 (1.05f,  0.0f,  1.05f), // right mid
	vec3 (1.05f,  0.0f, -1.05f), // right down
	vec3 (-1.05f, 0.0f,  0.0f), // left up
	vec3 (-1.05f, 0.0f,  1.05f), // left mid
	vec3 (-1.05f, 0.0f, -1.05f), // left down

	// BOTTOM
	vec3 (1.05f,-1.05f,0.f), // center up 
	vec3 (0.0f,-1.05f,0.f), // center mid 
	vec3 (-1.05f,-1.05f,0.f), // center left 
	vec3 (1.05f,-1.05f,-1.05f), // left up 
	vec3 (0.0f,-1.05f,-1.05f), // left mid 
	vec3 (-1.05f,-1.05f,-1.05f), // left down 
	vec3 (1.05f,-1.05f,1.05f), // right up 
	vec3 (0.0f,-1.05f,1.05f), // right mid 
	vec3 (-1.05f,-1.05f,1.05f) // right left 
};
int cubes = sizeof (cubePositions) / sizeof (cubePositions[0]);

void framebuffer_size_callback (GLFWwindow *window, int w, int h) {
	glViewport (0, 0, w, h);
}

static void cursor_position_callback (GLFWwindow *window, double xpos, double ypos) {
	if ( firstMouse ) // initially set to true
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement (xoffset, yoffset);
}

void processInput (GLFWwindow *window, int key, int scancode, int action, int mods) {
	float cameraSpeed = 10.f * deltaTime;
	if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
		glfwSetWindowShouldClose (window, true);
	if ( key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT) )
		camera.ProcessKeyboard (FORWARD, deltaTime);
	if ( key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT) )
		camera.ProcessKeyboard (BACKWARD, deltaTime);
	if ( key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT) )
		camera.ProcessKeyboard (LEFT, deltaTime);
	if ( key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT) )
		camera.ProcessKeyboard (RIGHT, deltaTime);
	if ( key == GLFW_KEY_SPACE && (action == GLFW_PRESS || action == GLFW_REPEAT) )
		camera.Position = vec3(camera.Position.x, camera.Position.y + cameraSpeed, camera.Position.z);
	if ( mods == GLFW_MOD_CONTROL && (action == GLFW_PRESS || action == GLFW_REPEAT) )
		camera.Position = vec3(camera.Position.x, camera.Position.y - cameraSpeed, camera.Position.z);

}

void mouseButtonCallback (GLFWwindow *window, int button, int action, int mods) {
	if ( button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS )
		cout << "Right button pressed at " << lastX << "," << lastY << endl;

	if ( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS )
		cout << "Left button pressed at " << lastX << "," << lastY << endl;

}

void scrollCallback (GLFWwindow *window, double xOffset, double yOffset) {
	camera.ProcessMouseScroll (yOffset);
}

void bufferConfig (float *vertices, int sizeVertex) {
	glEnable (GL_DEPTH_TEST);
	glGenBuffers (1, &VBO);
	glGenVertexArrays (1, &VAO);

	glBindBuffer (GL_ARRAY_BUFFER, VBO);
	glBufferData (GL_ARRAY_BUFFER, sizeVertex * sizeof (vertices[0]), vertices, GL_STATIC_DRAW);

	glBindVertexArray (VAO);

	// coordinates
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof (float), (void *)0);
	glEnableVertexAttribArray (0);
	// normal vector
	glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof (float), (void *)(3 * sizeof (float)));
	glEnableVertexAttribArray (1);
	// color 
	glVertexAttribPointer (2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof (float), (void *)(6 * sizeof (float)));
	glEnableVertexAttribArray (2);

	glGenVertexArrays (1, &lightVAO);
	glBindVertexArray (lightVAO);

	// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
	glBindBuffer (GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof (float), (void *)0);
	glEnableVertexAttribArray (0);
}


void displayWindow (GLFWwindow *window, int size) {
	glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float time = glfwGetTime ();
	deltaTime = time - lastFrame;
	lastFrame = time;

	model = mat4 (1.0f);
	model = rotate (model, time, vec3 (0.0f, 1.0f, 0.0f));
	model = translate (model, vec3 (lightPos.x, lightPos.y + sin (time), lightPos.z));

	view = camera.GetViewMatrix ();
	projection = perspective (radians (camera.Zoom), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);

	vec3 tempLightPos = model * vec4 (lightPos, 1.0f);

	model = mat4 (1.0f);
	shader->use ();
	shader->setVec3 ("lightPos", tempLightPos);
	shader->setVec3 ("lightColor", vec3(1.0f));
	shader->setVec3 ("viewPos", camera.Position);
	shader->setMat4 ("projection", projection);
	shader->setMat4 ("view", view);
	glBindVertexArray (VAO);
	int in = 0;
	int cont = 0;
	for ( int i = 0; i < cubes; ++i ) {
		model = mat4 (1.0f);
		model = translate (model, cubePositions[i]);
		shader->setMat4 ("model", model);

		glDrawArrays (GL_TRIANGLES, 0, size);
	}

	lightShader->use ();
	lightShader->setMat4 ("projection", projection);
	lightShader->setMat4 ("view", view);

	// para rotar con respecto a un punto primero rotar, luego trasladar

	model = rotate (model, time, vec3 (0.0f, 1.0f, 0.0f));
	model = translate (model, vec3 (lightPos.x, lightPos.y + sin (time), lightPos.z));
	model = scale (model, vec3 (0.2f)); // a smaller cube
	lightShader->setMat4 ("model", model);

	glBindVertexArray (lightVAO);
	glDrawArrays (GL_TRIANGLES, 0, size);

	glfwSwapBuffers (window); // swap buffers
	glfwPollEvents (); // checks if any events are triggered
}

int main () {
	if ( !glfwInit () )
		return -1;
	// para el manejo de la ventana
	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3); // mayor and minor version for openGL 3
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
	// to use core_profile for access to subsets of features 
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow *window = glfwCreateWindow (screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);
	if ( !window ) {
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate (); // destroys remaining windows
		return -1;
	}

	glfwSetMouseButtonCallback (window, mouseButtonCallback);
	glfwSetScrollCallback (window, scrollCallback);
	glfwSetKeyCallback (window, processInput);
	glfwSetCursorPosCallback (window, cursor_position_callback);

	glfwSetInputMode (window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent (window);
	glfwSetFramebufferSizeCallback (window, framebuffer_size_callback);

	if ( !gladLoadGLLoader ((GLADloadproc)glfwGetProcAddress) ) {
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

	shader = new Shader (path + "shaders/shader.vs", path + "shaders/shader.fs");
	lightShader = new Shader (path + "shaders/lightShader.vs", path + "shaders/lightShader.fs");

	float vertices[] = {
		/// Coordenadas       // Normales	       // Color
		/// Back: Amarillo
		-0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f, 0.0f,

		/// Front: Blanco
		-0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,    1.0f, 1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,    1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,    1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,    1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,    1.0f, 1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,    1.0f, 1.0f, 1.0f,

		/// Left: Naranja
		-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.5f, 0.0f,
		-0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.5f, 0.0f,
		-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.5f, 0.0f,
		-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.5f, 0.0f,
		-0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.5f, 0.0f,
		-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.5f, 0.0f,

		/// Right: Red
		 0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f, 0.0f,

		 /// Down: Blue
		-0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f, 1.0f,

		/// Up: Verde
		-0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f, 0.0f
	};
	int sizeVertex = sizeof (vertices) / sizeof (vertices[0]);

	bufferConfig (vertices, sizeVertex);

	while ( !glfwWindowShouldClose (window) )
		displayWindow (window, sizeVertex / 9);

	// ------------------------------------------------------------------------
	glDeleteVertexArrays (1, &VAO);
	glDeleteVertexArrays (1, &lightVAO);
	glDeleteBuffers (1, &VBO);
	glfwTerminate ();
	delete shader, lightShader;
	return 0;
}
