#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <iostream>

using namespace std;

int width = 800, height = 600;

const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(.2f, 0.0f, .5f, .5f);\n"

"}\n\0";


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





    // vertex shader
    unsigned int vertexShader = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader (vertexShader);

    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv (vertexShader, GL_COMPILE_STATUS, &success);
    if ( !success ) {
        glGetShaderInfoLog (vertexShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
    }


    // fragment shader
    unsigned int fragmentShader = glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader (fragmentShader);
    // check for shader compile errors
    glGetShaderiv (fragmentShader, GL_COMPILE_STATUS, &success);
    if ( !success ) {
        glGetShaderInfoLog (fragmentShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
    }

    
    // link shaders
    unsigned int shaderProgram = glCreateProgram ();
    glAttachShader (shaderProgram, vertexShader);
    glAttachShader (shaderProgram, fragmentShader);
    glLinkProgram (shaderProgram);

    // check for linking errors
    glGetProgramiv (shaderProgram, GL_LINK_STATUS, &success);
    if ( !success ) {
        glGetProgramInfoLog (shaderProgram, 512, NULL, infoLog);
        cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
    }
    glDeleteShader (vertexShader);
    glDeleteShader (fragmentShader);


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    float vertices[] = {
        //C = 6 triangles
            -0.8f, 0.4f, 0.0f, // top right
            -0.8f, -0.4f, 0.0f, // bottom right
            -0.9f, -0.4f, 0.0f, // bottom left
            -0.9f, 0.4f, 0.0f, //top left

            -0.7f, 0.4f, 0.0f,
            -0.7f, 0.3f, 0.0f,
            -0.8f, 0.3f,0.0f,
            -0.8f, 0.4f,0.0f,

            -0.7f, -0.3f, 0.0f,
            -0.7f, -0.4f, 0.0f,
            -0.8f, -0.4f, 0.0f,
            -0.8f, -0.3f,0.0f,

            //H = 6 triangles
            -0.5f,0.4f, 0.0f,
            -0.5f,-0.4f, 0.0f,
            -0.6f,-0.4f, 0.0f,
            -0.6f,0.4f, 0.0f,

            -0.3f,0.1f, 0.0f,
            -0.3f,-0.1f, 0.0f,
            -0.5f,-0.1f, 0.0f,
            -0.5f,0.1f, 0.0f,

            -0.2f,0.4f, 0.0f,
            -0.2f,-0.4f, 0.0f,
            -0.3f,-0.4f, 0.0f,
            -0.3f,0.4f, 0.0f,
        //I = 6 triangles
            0.2f,0.4f, 0.0f,
            0.2f,0.3f, 0.0f,
            -0.1f,0.3f, 0.0f,
            -0.1f,0.4f, 0.0f,

            0.1f,0.3f, 0.0f,
            0.1f,-0.3f, 0.0f,
            0.0f,-0.3f, 0.0f,
            0.0f,0.3f, 0.0f,

            0.2f,-0.3f, 0.0f,
            0.2f,-0.4f, 0.0f,
            -0.1f,-0.4f, 0.0f,
            -0.1f,-0.3f, 0.0f,
        //I = 6 triangles
            0.6f,0.4f, 0.0f,
            0.6f,0.3f, 0.0f,
            0.3f,0.3f, 0.0f,
            0.3f,0.4f, 0.0f,

            0.5f,0.3f, 0.0f,
            0.5f,-0.3f, 0.0f,
            0.4f,-0.3f, 0.0f,
            0.4f,0.3f, 0.0f,

            0.6f,-0.3f, 0.0f,
            0.6f,-0.4f, 0.0f,
            0.3f,-0.4f, 0.0f,
            0.3f,-0.3f, 0.0f,
        //L = 4 triangles
            0.8f,0.4f, 0.0f,
            0.8f,-0.4f, 0.0f,
            0.7f,-0.4f, 0.0f,
            0.7f,0.4f, 0.0f,

            0.98f,-0.3f, 0.0f,
            0.98f,-0.4f, 0.0f,
            0.8f,-0.4f, 0.0f,
            0.8f,-0.3f, 0.0f,

    };
    unsigned int indices[sizeof(vertices)/sizeof(vertices[0])];
    int indice = 0;
    for ( int i = 0; i < 168; i += 6 ) {
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

    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (void *)0);
    glEnableVertexAttribArray (0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer (GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray (0);

	while ( !glfwWindowShouldClose (window) ) {
		processInput (window);
		glClearColor (1.0f, 0.3f, 0.3f, 1.0f);
		glClear (GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram (shaderProgram);
        glBindVertexArray (VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawElements (GL_TRIANGLES, 168, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time 

		glfwSwapBuffers (window); // swap buffers
		glfwPollEvents (); // checks if any events are triggered
	}

    // ------------------------------------------------------------------------
    glDeleteVertexArrays (1, &VAO);
    glDeleteBuffers (1, &VBO);
	glfwTerminate ();
	return 0;
}
