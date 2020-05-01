

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

using namespace std;

void framebuffer_size_callback (GLFWwindow *window, int width, int height);
void processInput (GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char *fragmentShader1Source = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 color1;\n"
"void main()\n"
"{\n"
"   FragColor = color1;\n"
"}\n\0";


const char *fragmentShader2Source = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 color2;\n"
"void main()\n"
"{\n"
"   FragColor = color2;\n"
"}\n\0";



float red1 = 1, red2 = 1, green1 = 1, green2 = 0, blue1 = 1, blue2 = 0;
bool active = 0;

void key_callback (GLFWwindow *window, int key, int scancode, int action, int mods) {
    if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
        glfwSetWindowShouldClose (window, true);
    if ( key == GLFW_KEY_C && action == GLFW_PRESS )
        active = !active;
    if ( key == GLFW_KEY_R && action == GLFW_PRESS )
        if ( active )
            red2 = !red2;
        else
            red1 = !red1;
    if ( key == GLFW_KEY_G && action == GLFW_PRESS )
        if ( active )
            green2 = !green2;
        else
            green1 = !green1;
    if ( key == GLFW_KEY_B && action == GLFW_PRESS )
        if ( active )
            blue2 = !blue2;
        else
            blue1 = !blue1;
}

void scrollCallback (GLFWwindow *window, double xOffset, double yOffset) {
    cout << xOffset << " " << yOffset << endl;
    if ( yOffset == -1 ) {
        if ( active ) {
            if ( red2 > 0 ) red2 -= 0.1f;
            if ( green2 > 0 ) green2 -= 0.1f;
            if ( blue2 > 0 ) blue2 -= 0.1f;
        } else {
            if ( red1 > 0 ) red1 -= 0.1f;
            if ( green1 > 0 ) green1 -= 0.1f;
            if ( blue1 > 0 ) blue1 -= 0.1f;
        }
    } else {
        if ( active ) {
            if ( red2 < 1 ) red2 += 0.1f;
            if ( green2 < 1 ) green2 += 0.1f;
            if ( blue2 < 1 ) blue2 += 0.1f;
        } else {
            if ( red1 < 1 ) red1 += 0.1f;
            if ( green1 < 1 ) green1 += 0.1f;
            if ( blue1 < 1 ) blue1 += 0.1f;
        }
    }
}


int main () {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit ();
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow (SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if ( window == NULL ) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate ();
        return -1;
    }
    glfwMakeContextCurrent (window);
    glfwSetFramebufferSizeCallback (window, framebuffer_size_callback);
    glfwSetScrollCallback (window, scrollCallback);
    glfwSetKeyCallback (window, key_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if ( !gladLoadGLLoader ((GLADloadproc)glfwGetProcAddress) ) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // build and compile our shader program
    // ------------------------------------
    // we skipped compile log checks this time for readability (if you do encounter issues, add the compile-checks! see previous code samples)
    unsigned int vertexShader = glCreateShader (GL_VERTEX_SHADER);
    unsigned int fragmentShader1 = glCreateShader (GL_FRAGMENT_SHADER); // the second fragment shader that outputs the color yellow
    unsigned int fragmentShader2 = glCreateShader (GL_FRAGMENT_SHADER); // the first fragment shader that outputs the color orange
    int success;
    char infoLog[512];

    glShaderSource (vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader (vertexShader);
    glShaderSource (fragmentShader1, 1, &fragmentShader1Source, NULL);
    glCompileShader (fragmentShader1);
    glShaderSource (fragmentShader2, 1, &fragmentShader2Source, NULL);
    glCompileShader (fragmentShader2);

    glGetShaderiv (vertexShader, GL_COMPILE_STATUS, &success);
    if ( !success ) {
        glGetShaderInfoLog (vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    glGetShaderiv (fragmentShader1, GL_COMPILE_STATUS, &success);
    if ( !success ) {
        glGetShaderInfoLog (fragmentShader1, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT1::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    glGetShaderiv (fragmentShader2, GL_COMPILE_STATUS, &success);
    if ( !success ) {
        glGetShaderInfoLog (fragmentShader2, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT2::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram1 = glCreateProgram (); // the second shader program
    unsigned int shaderProgram2 = glCreateProgram ();
    // link the first program object
    glAttachShader (shaderProgram2, vertexShader);
    glAttachShader (shaderProgram2, fragmentShader2);
    glLinkProgram (shaderProgram2);
    // then link the second program object using a different fragment shader (but same vertex shader)
    // this is perfectly allowed since the inputs and outputs of both the vertex and fragment shaders are equally matched.
    glAttachShader (shaderProgram1, vertexShader);
    glAttachShader (shaderProgram1, fragmentShader1);
    glLinkProgram (shaderProgram1);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float firstTriangle[] = {
        -0.9f, -0.5f, 0.0f,  // left 
        -0.0f, -0.5f, 0.0f,  // right
        -0.45f, 0.5f, 0.0f,  // top 
    };
    float secondTriangle[] = {
        0.0f, -0.5f, 0.0f,  // left
        0.9f, -0.5f, 0.0f,  // right
        0.45f, 0.5f, 0.0f   // top 
    };
    unsigned int VBOs[2], VAOs[2];
    glGenVertexArrays (2, VAOs); // we can also generate multiple VAOs or buffers at the same time
    glGenBuffers (2, VBOs);
    // first triangle setup
    // --------------------
    glBindVertexArray (VAOs[0]);
    glBindBuffer (GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData (GL_ARRAY_BUFFER, sizeof (firstTriangle), firstTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (void *)0);	// Vertex attributes stay the same
    glEnableVertexAttribArray (0);

    glBindVertexArray (VAOs[1]);	// note that we bind to a different VAO now
    glBindBuffer (GL_ARRAY_BUFFER, VBOs[1]);	// and a different VBO
    glBufferData (GL_ARRAY_BUFFER, sizeof (secondTriangle), secondTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
    glEnableVertexAttribArray (0);

    // render loop
    // -----------
    int vertexColorLocation1 = glGetUniformLocation (shaderProgram1, "color1");
    int vertexColorLocation2 = glGetUniformLocation (shaderProgram2, "color2");
    while ( !glfwWindowShouldClose (window) ) {
        // input
        // -----
        processInput (window);

        // render
        // ------
        glClearColor (0.2f, 0.3f, 0.3f, 1.0f);
        glClear (GL_COLOR_BUFFER_BIT);


        // now when we draw the triangle we first use the vertex and orange fragment shader from the first program
        glUseProgram (shaderProgram1);

        glUniform4f (vertexColorLocation2, red2, green2, blue2, 1.0f);
        // draw the first triangle using the data from our first VAO
        glBindVertexArray (VAOs[0]);
        glDrawArrays (GL_TRIANGLES, 0, 3);	// this call should output an orange triangle
        // then we draw the second triangle using the data from the second VAO
        // when we draw the second triangle we want to use a different shader program so we switch to the shader program with our yellow fragment shader.
        glUseProgram (shaderProgram2);
        glUniform4f (vertexColorLocation1, red1, green1, blue1, 1.0f);
        glBindVertexArray (VAOs[1]);
        glDrawArrays (GL_TRIANGLES, 0, 3);	// this call should output a yellow triangle

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers (window);
        glfwPollEvents ();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays (2, VAOs);
    glDeleteBuffers (2, VBOs);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate ();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput (GLFWwindow *window) {
    if ( glfwGetKey (window, GLFW_KEY_ESCAPE) == GLFW_PRESS )
        glfwSetWindowShouldClose (window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback (GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport (0, 0, width, height);
}

