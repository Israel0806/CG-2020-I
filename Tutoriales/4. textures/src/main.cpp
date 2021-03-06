#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include "shader.h"
#include "sierpinsky.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

using namespace std;

int screenWidth = 800, screenHeight = 600;
double mouseXPos = 0, mouseYPos = 0;
unsigned int VBO, VAO, EBO;
unsigned int texture1, texture2;
Shader *myShader;
float mixer = 0.2;
Sierpinski *fractal;
float *vertices;

void reDraw () {
	delete[] vertices;
	vertices = fractal->getVertex ();
	float sizeVertices = fractal->getNumVertex () * sizeof (vertices[0]);
	glBindBuffer (GL_ARRAY_BUFFER, VBO);
	glBufferData (GL_ARRAY_BUFFER, sizeVertices, vertices, GL_DYNAMIC_DRAW);
}

void framebuffer_size_callback (GLFWwindow *window, int w, int h) {
	glViewport (500, 500, w, h);
}

void processInput (GLFWwindow *window, int key, int scancode, int action, int mods) {
	if ( key == GLFW_KEY_ESCAPE && (action == GLFW_PRESS || action == GLFW_REPEAT) )
		glfwSetWindowShouldClose (window, true);
	if ( key == GLFW_KEY_P && action == GLFW_PRESS )
		fractal->print ();
	if ( key == GLFW_KEY_UP && action == GLFW_PRESS ) {
		fractal->addLevel ();
		reDraw ();
	}
	if ( key == GLFW_KEY_DOWN && action == GLFW_PRESS ) {
		fractal->removeLevel ();
		reDraw ();
	}
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

void scrollCallback (GLFWwindow *window, double xOffset, double yOffset) {
	if ( yOffset == -1 )
		fractal->removeLevel ();
	else
		fractal->addLevel ();
	reDraw ();
}


void bufferConfig (float *vertices, int sizeVertex) {
	glGenBuffers (1, &VBO);
	glGenVertexArrays (1, &VAO);


	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray (VAO);
	glBindBuffer (GL_ARRAY_BUFFER, VBO);
	glBufferData (GL_ARRAY_BUFFER, sizeVertex * sizeof (vertices[0]), vertices, GL_STATIC_DRAW);

	// coordinates
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof (float), (void *)0);
	glEnableVertexAttribArray (0);

	/// color values
	glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof (float), (void *)(3 * sizeof (float)));
	glEnableVertexAttribArray (1);

	// texture position
	glVertexAttribPointer (2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof (float), (void *)(6 * sizeof (float)));
	glEnableVertexAttribArray (2);
	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer (GL_ARRAY_BUFFER, 0);
	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray (0);
}

void textureConfig () {
	int imageWidth, imageHeight, nrChannels;

	glGenTextures (1, &texture1);
	glBindTexture (GL_TEXTURE_2D, texture1);


	// set the texture wrapping parameters
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	// load and generate the texture
	unsigned char *imageData = stbi_load ("../textures/container.jpg", &imageWidth, &imageHeight, &nrChannels, 0);
	if ( imageData ) {
		glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap (GL_TEXTURE_2D);
	} else {
		cout << "Failed to load texture 1" << endl;
	}

	stbi_image_free (imageData);

	stbi_set_flip_vertically_on_load (true);
	glGenTextures (1, &texture2);
	glBindTexture (GL_TEXTURE_2D, texture2);

	// set the texture wrapping parameters
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// we want to repeat the awesomeface pattern so we kept it at GL_REPEAT
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	imageData = stbi_load ("../textures/awesomeface.png", &imageWidth, &imageHeight, &nrChannels, 0);
	if ( imageData ) {
		glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap (GL_TEXTURE_2D);
	} else {
		cout << "Failed to load texture 2" << endl;
	}
	stbi_image_free (imageData);

	myShader->use (); // don't forget to activate the shader before setting uniforms!  
	glUniform1i (glGetUniformLocation (myShader->ID, "texture1"), 0); // set it manually
	myShader->setInt ("texture2", 1); // or with shader class
}

void displayWindow (GLFWwindow *window, int size) {
	glClearColor (1.0f, 0.3f, 0.3f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT);
	glfwGetCursorPos (window, &mouseXPos, &mouseYPos);
	glActiveTexture (GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, texture1);
	glActiveTexture (GL_TEXTURE1);
	glBindTexture (GL_TEXTURE_2D, texture2);

	glBindVertexArray (VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	glDrawArrays (GL_TRIANGLES, 0, fractal->getNumVertex());
	// glBindVertexArray(0); // no need to unbind it every time 

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
	glfwSetKeyCallback (window, processInput);

	glfwMakeContextCurrent (window);
	glfwSetFramebufferSizeCallback (window, framebuffer_size_callback);

	/// gkad vaida que version esta permitida para el opengl actual
	// defines the correct function based on which OS we're compiling for
	if ( !gladLoadGLLoader ((GLADloadproc)glfwGetProcAddress) ) {
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}
	fractal = new Sierpinski ();
	myShader = new Shader ("../src/shader.vs", "../src/shader.fs");
	myShader->use ();
	myShader->setFloat ("mixer", mixer);
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------

	Vertex *v1 = new Vertex (0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f);
	Vertex *v2 = new Vertex (0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f);
	Vertex *v3 = new Vertex (-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f);
	fractal->addTriangle (v1, v2, v3);
	// Vertex Buffer Object, Vertex Array Object, Element Buffer Object
	fractal->createLevels (10);


	//float vertices[] = {
	//	// positions          // colors           // texture coords
	//	 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   2.0f, 2.0f,   // top right
	//	 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   2.0f, 0.0f,   // bottom right
	//	-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
	//	-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 2.0f    // top left 
	//};
	vertices = fractal->getVertex ();
	int sizeVertex = fractal->getNumVertex ();

	// Vertex Buffer Object, Vertex Array Object, Element Buffer Object

	bufferConfig (vertices, sizeVertex);

	textureConfig ();

	while ( !glfwWindowShouldClose (window) )
		displayWindow (window, sizeVertex);

	// ------------------------------------------------------------------------
	glDeleteVertexArrays (1, &VAO);
	glDeleteBuffers (1, &VBO);
	glfwTerminate ();
	delete myShader;
	delete fractal;
	delete[] vertices;
	return 0;
}
