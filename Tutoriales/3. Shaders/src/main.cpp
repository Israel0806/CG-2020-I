#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include "shader.h"

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

    Shader myShader ("../src/shader.vs","../src/shader.fs");
    

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    float vertices[] = {
        //C = 6 triangles
            -0.8f, 0.4f, 0.0f, 1.0f, 0.0f, 0.0f, // top right
            -0.8f, -0.4f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right
            -0.9f, -0.4f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
            -0.9f, 0.4f, 0.0f, 0.0f, 0.0f, 1.0f, //top left

            -0.7f, 0.4f, 0.0f, 1.0f, 0.0f, 0.0f,
            -0.7f, 0.3f, 0.0f, 0.0f, 1.0f, 0.0f,
            -0.8f, 0.3f,0.0f, 1.0f, 0.0f, 0.0f,
            -0.8f, 0.4f,0.0f, 0.0f, 0.0f, 1.0f,

            -0.7f, -0.3f, 0.0f, 1.0f, 0.0f, 0.0f,
            -0.7f, -0.4f, 0.0f, 0.0f, 1.0f, 0.0f,
            -0.8f, -0.4f, 0.0f, 1.0f, 0.0f, 0.0f,
            -0.8f, -0.3f,0.0f, 0.0f, 0.0f, 1.0f,

            //H = 6 triangles
            -0.5f,0.4f, 0.0f, 1.0f, 0.0f, 0.0f,
            -0.5f,-0.4f, 0.0f, 0.0f, 1.0f, 0.0f,
            -0.6f,-0.4f, 0.0f, 1.0f, 0.0f, 0.0f,
            -0.6f,0.4f, 0.0f, 0.0f, 0.0f, 1.0f,

            -0.3f,0.1f, 0.0f, 1.0f, 0.0f, 0.0f,
            -0.3f,-0.1f, 0.0f, 0.0f, 1.0f, 0.0f,
            -0.5f,-0.1f, 0.0f, 1.0f, 0.0f, 0.0f,
            -0.5f,0.1f, 0.0f, 0.0f, 0.0f, 1.0f,

            -0.2f,0.4f, 0.0f, 1.0f, 0.0f, 0.0f,
            -0.2f,-0.4f, 0.0f, 0.0f, 1.0f, 0.0f,
            -0.3f,-0.4f, 0.0f, 1.0f, 0.0f, 0.0f,
            -0.3f,0.4f, 0.0f, 0.0f, 0.0f, 1.0f,

        //I = 6 triangles
            0.2f,0.4f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.2f,0.3f, 0.0f, 0.0f, 1.0f, 0.0f,
            -0.1f,0.3f, 0.0f, 1.0f, 0.0f, 0.0f,
            -0.1f,0.4f, 0.0f, 0.0f, 0.0f, 1.0f,

            0.1f,0.3f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.1f,-0.3f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.0f,-0.3f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.0f,0.3f, 0.0f, 0.0f, 0.0f, 1.0f,

            0.2f,-0.3f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.2f,-0.4f, 0.0f, 0.0f, 1.0f, 0.0f,
            -0.1f,-0.4f, 0.0f, 1.0f, 0.0f, 0.0f,
            -0.1f,-0.3f, 0.0f, 0.0f, 0.0f, 1.0f,
        //L = 4 triangles
            0.3f,0.4f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.3f,-0.4f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.4f,-0.4f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.4f,0.4f, 0.0f, 0.0f, 0.0f, 1.0f,

            0.6f,-0.3f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.6f,-0.4f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.4f,-0.4f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.4f,-0.3f, 0.0f, 0.0f, 0.0f, 1.0f,

        //L = 4 triangles
            0.8f,0.4f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.8f,-0.4f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.7f,-0.4f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.7f,0.4f, 0.0f, 0.0f, 0.0f, 1.0f,

            0.98f,-0.3f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.98f,-0.4f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.8f,-0.4f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.8f,-0.3f, 0.0f, 0.0f, 0.0f, 1.0f
    };
    unsigned int indices[sizeof(vertices)/sizeof(vertices[0]) / 2];
    int indice = 0;
    /// 0 1 3
    /// 1 2 3
    for ( int i = 0; i < sizeof (vertices) / sizeof (vertices[0]) / 2; i += 6 ) {
        indices[i] = indice; indices[i + 1] = indice + 1; indices[i + 2] = indice + 3;
        indices[i + 3] = indice + 1; indices[i + 4] = indice + 2; indices[i + 5] = indice + 3;
        indice += 4;
    }

    // Vertex Buffer Object, Vertex Array Object, Element Buffer Object
    unsigned int VBO, VAO, EBO;


    glGenBuffers (1, &VBO);
    glGenVertexArrays (1, &VAO);
    glGenBuffers (1, &EBO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray (VAO);

    glBindBuffer (GL_ARRAY_BUFFER, VBO);
    glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof (float), (void *)0);
    glEnableVertexAttribArray (0);
    // color attribute
    glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof (float), (void *)(3 * sizeof (float)));
    glEnableVertexAttribArray (1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer (GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray (0);
	while ( !glfwWindowShouldClose (window) ) {
		processInput (window);
		glClearColor (1.0f, 0.3f, 0.3f, 1.0f);
		glClear (GL_COLOR_BUFFER_BIT);

        /// change color values
        float timeValue = glfwGetTime (); /// obtiene el tiempo en segundos
        float greenValue = (cos (timeValue) / 2.0f) + 0.5f; /// valor de 0.0 a 1.0
        float redValue = (sin (timeValue) / 2.0f) + 0.5f;
        float blueValue = (cos (timeValue) / 2.0f) + 0.5f;
        float vec[] = { greenValue, redValue, blueValue, 1.0f };
        myShader.use ();
        myShader.setFloat ("changingColor",vec);
        // int vertexColorLocation = glGetUniformLocation (shaderProgram, "ourColor"); /// obtiene la location del uniform 
        ///glUseProgram (shaderProgram);
        ///glUniform4f (vertexColorLocation, 0.0f, greenValue, redValue, blueValue); /// set value of uniform in vertexColorLocation
        
        glBindVertexArray (VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawElements (GL_TRIANGLES, sizeof (vertices) / sizeof (vertices[0]), GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time 

		glfwSwapBuffers (window); // swap buffers
		glfwPollEvents (); // checks if any events are triggered
	}

    // ------------------------------------------------------------------------
    glDeleteVertexArrays (1, &VAO);
    glDeleteBuffers (1, &VBO);
    glDeleteBuffers (1, &EBO);
	glfwTerminate ();
	return 0;
}
