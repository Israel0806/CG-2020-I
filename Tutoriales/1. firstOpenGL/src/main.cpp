#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <iostream>

using namespace std;

int width = 800, height = 600;

void framebuffer_size_callback (GLFWwindow *window, int w, int h) {
	glViewport (500, 500, w, h);
}

void processInput (GLFWwindow *window) {
	if ( glfwGetKey (window, GLFW_KEY_ESCAPE) == GLFW_PRESS )
		glfwSetWindowShouldClose (window, true);
}

int main () {
	if ( !glfwInit () )
		return -1;
	// para el manejo de la ventana
	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3); // mayor and minor version for openGL 3
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
	// to use core_profile for access to subsets of features 
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow *window = glfwCreateWindow (width, height, "LearnOpenGL", NULL, NULL);
	if ( !window ) {
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate (); // destroys remaining windows
		return -1;
	}
	glfwMakeContextCurrent (window);
	glfwSetFramebufferSizeCallback (window, framebuffer_size_callback);
	
	// defines the correct function based on which OS we're compiling for
	if ( !gladLoadGLLoader ((GLADloadproc)glfwGetProcAddress) ) {
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}
	
	while ( !glfwWindowShouldClose (window) ) {
		glClearColor (0.2f, 0.3f, 0.3f, 1.0f);
		glClear (GL_COLOR_BUFFER_BIT);
		processInput (window);
		glfwSwapBuffers (window); // swap buffers
		glfwPollEvents (); // checks if any events are triggered
	}
	glfwTerminate ();
	return 0;
}
