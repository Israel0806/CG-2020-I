#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include "shader.h"
#define STB_IMAGE_IMPLEMENTATION

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include<windows.h>
#include "readMesh.h"
#include "stb_image.h"

using namespace std;
using namespace glm;

int screenWidth = 800, screenHeight = 600, time = 0;
Shader *myShader;
double mouseXPos = 0, mouseYPos = 0;
/// 0 = apple, 1 = log, 2 = leaves
unsigned int VBO[3], VAO[3], EBO[3];
unsigned int texture[3];
mat4 model, view, projection;
bool createApple = true;
bool firstMouse = false;
float lastX = 0;
float posX = 0.0f, posY = 0.0f, posZ = -15.0f;
bool _rotate = true;

vector<vec3 *> apples;
vector<vec3> applePositions;

void framebuffer_size_callback (GLFWwindow *window, int w, int h) {
	glViewport (500, 500, w, h);
}

void processInput (GLFWwindow *window, int key, int scancode, int action, int mods) {
	if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
		glfwSetWindowShouldClose (window, true);
	if ( key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT) )
		posY -= 0.5;
	if ( key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT) )
		posY += 0.5;
	if ( key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT) )
		posX += 0.5;
	if ( key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT) )
		posX -= 0.5;
}

void cursorEnterCallback (GLFWwindow *window, int entered) {
	if ( entered ) {
		cout << "Entered window" << endl;
	} else {
		cout << "Left window" << endl;
	}
}

void mouseButtonCallback (GLFWwindow *window, int button, int action, int mods) {
	if ( button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS ) {
		cout << "Right button pressed at " << mouseXPos << "," << mouseYPos << endl;
	}
	if ( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS ) {
		cout << "Left button pressed at " << mouseXPos << "," << mouseYPos << endl;
	}
}

void printApples () {
	for ( int i = 0; i < apples.size (); ++i ) {
		cout << "apple " << i << ": " << apples[i]->x << " " << apples[i]->y << " " << apples[i]->z << endl;
	}
}

void scrollCallback (GLFWwindow *window, double xOffset, double yOffset) {
	if ( !_rotate ) return;
	posZ += yOffset;
	if ( posZ > -10.0f )
		posZ = -10.0f;

}

void mouse_callback (GLFWwindow *window, double xpos, double ypos) {
	if ( firstMouse ) {
		lastX = xpos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	lastX = xoffset * 0.05;

}

void bufferConfig (float *vertices, int sizeVertex, int *indices, int sizeIndex, unsigned int VBO, unsigned int VAO, unsigned int EBO) {
	glEnable (GL_DEPTH_TEST);
	glBindVertexArray (VAO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray (VAO);
	glBindBuffer (GL_ARRAY_BUFFER, VBO);
	glBufferData (GL_ARRAY_BUFFER, sizeVertex * sizeof (vertices[0]), vertices, GL_STATIC_DRAW);

	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeIndex * sizeof (indices[0]), indices, GL_STATIC_DRAW);

	// coordinates
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof (float), (void *)0);
	glEnableVertexAttribArray (0);

	glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof (float), (void *)3);
	glEnableVertexAttribArray (0);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	//glBindBuffer (GL_ARRAY_BUFFER, 0);
	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray (0);
}

void textureConfig () {
	int imageWidth, imageHeight, nrChannels;

	/// APPLE
	glGenTextures (1, &texture[0]);
	glBindTexture (GL_TEXTURE_2D, texture[0]);

	// set the texture wrapping parameters
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	// load and generate the texture
	unsigned char *imageData = stbi_load ("../textures/apple.png", &imageWidth, &imageHeight, &nrChannels, 0);
	if ( imageData ) {
		glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap (GL_TEXTURE_2D);
	} else
		cout << "Failed to load texture 1" << endl;
	stbi_image_free (imageData);

	/// LOG
	glGenTextures (1, &texture[1]);
	glBindTexture (GL_TEXTURE_2D, texture[1]);

	// set the texture wrapping parameters
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	// load and generate the texture
	imageData = stbi_load ("../textures/log.jpeg", &imageWidth, &imageHeight, &nrChannels, 0);
	if ( imageData ) {
		glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap (GL_TEXTURE_2D);
	} else
		cout << "Failed to load texture 2" << endl;
	stbi_image_free (imageData);


	/// LEAVES
	glGenTextures (1, &texture[2]);
	glBindTexture (GL_TEXTURE_2D, texture[2]);

	// set the texture wrapping parameters
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	// load and generate the texture
	imageData = stbi_load ("../textures/leaves.jpg", &imageWidth, &imageHeight, &nrChannels, 0);
	if ( imageData ) {
		glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap (GL_TEXTURE_2D);
	} else
		cout << "Failed to load texture 3" << endl;
	stbi_image_free (imageData);

	myShader->use ();
	myShader->setInt ("texture1", 0);
	myShader->setInt ("texture2", 1);
	myShader->setInt ("texture3", 2);

}

void displayWindow (GLFWwindow *window, int sizeApple, int sizeLog, int sizeLeaves) {
	glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glfwGetCursorPos (window, &mouseXPos, &mouseYPos);

	view = mat4 (1.0f);
	projection = mat4 (1.0f);
	view = translate (view, vec3 (posX, posY, posZ));
	if ( _rotate )
		lastX += 0.05;
		view = rotate (view, lastX * (radians (20.0f)), vec3 (0.0f, 1.0f, 0.0f));
	projection = perspective (radians (45.f), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
	//model = rotate (model, time * (radians (20.0f)), vec3 (1.0f, 0.3f, 0.5f));

	++time;
	if ( time == 30 ) {
		apples.push_back (new vec3 (rand () % 8 - 3, 0.0f, rand () % 8 - 4));
		time = 0;
	}


	/// apple
	glActiveTexture (GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, texture[0]);
	vec3 color = vec3 (0.5f, 0.f, 0.0f);
	myShader->use ();
	myShader->setInt ("selected", 0);
	myShader->setMat4 ("view", view);
	myShader->setMat4 ("projection", projection);

	glBindVertexArray (VAO[0]);
	myShader->use ();
	myShader->setVec3 ("color", color);
	vec3 *apple;
	int eraseIndex = -1;
	for ( int i = 0; i < apples.size (); ++i ) {
		apple = apples[i];
		apple->y -= 0.05;
		if ( apple->y < -10.f )
			eraseIndex = i;
		model = mat4 (1.0f);
		model = translate (model, vec3 (apple->x, apple->y + 5.0, apple->z));
		model = scale (model, vec3 (0.3f, 0.3f, 0.3f));
		myShader->setMat4 ("model", model);
		glDrawElements (GL_TRIANGLES, sizeApple, GL_UNSIGNED_INT, 0);
	}
	if ( eraseIndex != -1 )
		apples.erase (apples.begin () + eraseIndex);



	/// log
	glActiveTexture (GL_TEXTURE1);
	glBindTexture (GL_TEXTURE_2D, texture[1]);

	color = vec3 (0.2f, 0.9f, 0.1f);
	model = mat4 (1.0f);
	model = translate (model, vec3 (0.0f, -5.0f, 0.0f));
	model = scale (model, vec3 (2.0f, 2.0f, 2.0f));

	glBindVertexArray (VAO[1]);
	myShader->use ();
	myShader->setInt ("selected", 1);
	myShader->setMat4 ("model", model);
	myShader->setVec3 ("color", color);
	glDrawElements (GL_TRIANGLES, sizeLog, GL_UNSIGNED_INT, 0);


	/// leaves
	glActiveTexture (GL_TEXTURE2);
	glBindTexture (GL_TEXTURE_2D, texture[2]);

	color = vec3 (0.7f, 0.0f, 0.0f);
	model = mat4 (1.0f);
	model = translate (model, vec3 (0.0f, -5.0f, 0.0f));
	model = scale (model, vec3 (2.0f, 2.0f, 2.0f));

	glBindVertexArray (VAO[2]);
	myShader->use ();	
	myShader->setInt ("selected", 2);
	myShader->setMat4 ("model", model);
	myShader->setVec3 ("color", color);
	glDrawElements (GL_TRIANGLES, sizeLeaves, GL_UNSIGNED_INT, 0);

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

	// glfwSetCursorPosCallback (window, cursorPositionCallback);
	glfwSetCursorEnterCallback (window, cursorEnterCallback);
	glfwSetMouseButtonCallback (window, mouseButtonCallback);
	glfwSetScrollCallback (window, scrollCallback);
	glfwSetCursorPosCallback (window, mouse_callback);
	glfwSetKeyCallback (window, processInput);

	glfwSetInputMode (window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent (window);
	glfwSetFramebufferSizeCallback (window, framebuffer_size_callback);

	/// gkad vaida que version esta permitida para el opengl actual
	// defines the correct function based on which OS we're compiling for
	if ( !gladLoadGLLoader ((GLADloadproc)glfwGetProcAddress) ) {
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

	myShader = new Shader ("../src/shader.vs", "../src/shader.fs");

	Mesh *meshApple = new Mesh ("../apple.obj");
	Mesh *meshLeaves = new Mesh ("../leaves.obj");
	Mesh *meshLog = new Mesh ("../log.obj");

	glGenBuffers (3, VBO);
	glGenVertexArrays (3, VAO);
	glGenBuffers (3, EBO);

	float *vertices = nullptr;
	int *indices = nullptr;
	int sizeApple, sizeLog, sizeLeaves;
	int  sizeVertex;

	/// apples
	vertices = meshApple->getVertexs (sizeVertex);
	indices = meshApple->getFaces (sizeApple);
	bufferConfig (vertices, sizeVertex, indices, sizeApple, VBO[0], VAO[0], EBO[0]);
	delete[] vertices, indices;

	/// log
	vertices = meshLog->getVertexs (sizeVertex);
	indices = meshLog->getFaces (sizeLog);
	bufferConfig (vertices, sizeVertex, indices, sizeLog, VBO[1], VAO[1], EBO[1]);
	delete[] vertices, indices;

	/// leaves
	vertices = meshLeaves->getVertexs (sizeVertex);
	indices = meshLeaves->getFaces (sizeLeaves);
	bufferConfig (vertices, sizeVertex, indices, sizeLeaves, VBO[2], VAO[2], EBO[2]);
	delete[] vertices, indices;


	textureConfig ();
	while ( !glfwWindowShouldClose (window) )
		displayWindow (window, sizeApple, sizeLog, sizeLeaves);

	// ------------------------------------------------------------------------
	glDeleteVertexArrays (3, VAO);
	glDeleteBuffers (3, VBO);
	glDeleteBuffers (3, EBO);
	delete myShader;
	glfwTerminate ();
	return 0;
}
