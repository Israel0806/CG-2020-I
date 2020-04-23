#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include "shader.h"

#include <iostream>
#include <time.h>
#include <vector>

using namespace std;

int width = 800, height = 600;
unsigned int VBO, VAO;
float *vertices = nullptr;

struct Vertex {
	float x, y, z;
	float red, green, blue;
	Vertex (float x, float y, float z, float red, float green, float blue) : x (x), y (y), z (z), red (red), green (green), blue (blue) {};

	void copyInto (float *vertices, int &index) {
		vertices[index++] = x;
		vertices[index++] = y;
		vertices[index++] = z;
		vertices[index++] = red;
		vertices[index++] = green;
		vertices[index++] = blue;
	}
};

struct Triangle {
	Vertex *v1, *v2, *v3;

	Triangle (Vertex *v1, Vertex *v2, Vertex *v3){
		this->v1 = v1;
		this->v2 = v2;
		this->v3 = v3;
	}

	~Triangle () {
		//delete v1;
		//delete v2;
		//delete v3;
	}
};

class Sierpinski {
	vector<Triangle *> triangles;
public:
	void addTriangle (Vertex *v1, Vertex *v2, Vertex *v3) {
		triangles.push_back (new Triangle (v1, v2, v3));
	}

	Vertex *midPoint (Vertex *v1, Vertex *v2, float red, float green, float blue) {
		return new Vertex ((v1->x + v2->x) / 2.0f, (v1->y + v2->y) / 2.0f, (v1->z + v2->z) / 2.0f, red, green, blue);
	}

	float getNumVertex () { return (float)triangles.size () * 18.0f; }

	void addLevel () {
		float red = rand () % 2;
		float green = rand () % 2;
		float blue = rand () % 2;
		vector<Triangle *> newTriangles;
		for ( auto &triangle : triangles ) {
			newTriangles.push_back (triangle);
			Vertex *_v1 = midPoint (triangle->v1, triangle->v2, red, green, blue);
			Vertex *_v2 = midPoint (triangle->v2, triangle->v3, red, green, blue);
			Vertex *_v3 = midPoint (triangle->v3, triangle->v1, red, green, blue);

			newTriangles.push_back (new Triangle (triangle->v1, _v1, _v3));
			newTriangles.push_back (new Triangle (_v1, triangle->v2, _v2));
			newTriangles.push_back (new Triangle (_v3, _v2, triangle->v3));
		}
		triangles.clear ();
		triangles = newTriangles;
		reDraw ();
	}

	float *getVertex () {
		int _size = triangles.size () * 18;
		float *_vertices = new float[_size];
		int index = 0;
		for ( auto &triangle : triangles ) {
			triangle->v1->copyInto (_vertices, index);
			triangle->v2->copyInto (_vertices, index);
			triangle->v3->copyInto (_vertices, index);
		}
		return _vertices;
	}

	friend void reDraw ();

	~Sierpinski () {
		while ( !triangles.empty () ) {
			Triangle *triangle = triangles.back ();
			triangles.pop_back ();
			delete triangle;
		}
	}
};

Sierpinski *fractal = new Sierpinski ();

void reDraw () {
	delete[] vertices;
	vertices = fractal->getVertex ();
	float sizeVertices = fractal->getNumVertex () * sizeof (float);
	glBindBuffer (GL_ARRAY_BUFFER, VBO);
	glBufferData (GL_ARRAY_BUFFER, sizeVertices, vertices, GL_DYNAMIC_DRAW);
}

//void addLevel () {
//    float timeValue = glfwGetTime (); /// obtiene el tiempo en segundos
//    float green = sin (timeValue) ; /// valor de 0.0 a 1.0
//    float red = sin (timeValue);
//    float blue= sin (timeValue);
//    float *ver = new float[(int)numTriangles + 54];
//    for ( int i = 0; i < numTriangles; ++i )
//		ver[i] = vertices[i];
//    delete[] vertices;
//    float v[3][3];
//    //0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // top 
//    //0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom right
//    //-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f // bottom left 
//    for ( int i = 0; i < numTriangles + 54; i+=6 ) {
//        v[i][0] = (ver[i] + ver[(i + 6) % numTriangles]) / 2;
//        v[i][1] = (ver[i + 1] + ver[(i + 7) % numTriangles]) / 2;
//		v[i][2] = (ver[i + 2] + ver[(i + 8) % numTriangles]) / 2;
//    }
//    for ( int i = 0; i < numTriangles + 54; i += 6 ) {
//        float v1;
//		float v2;
//    }
//    numTriangles += 54;
//    vertices = ver;
//    reDraw ();
//}

void printVertex () {
	for ( int i = 0; i < fractal->getNumVertex () ; i+=6 ) {
		if ( i % 16 == 0 ) cout << endl;
		cout << "pos: (" << vertices[i] << " ," << vertices[i + 1] << " ," << vertices[i + 2] << ") color: (" << vertices[i + 3] << " ," << vertices[i + 4] << " ," << vertices[i + 5] << ")";
		cout << endl;
	}
	delete[] vertices;
}

void framebuffer_size_callback (GLFWwindow *window, int w, int h) {
	glViewport (500, 500, w, h);
}

void key_callback (GLFWwindow *window, int key, int scancode, int action, int mods) {
	if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
		glfwSetWindowShouldClose (window, true);
	if ( key == GLFW_KEY_UP && action == GLFW_PRESS )
		fractal->addLevel ();
	if ( key == GLFW_KEY_P && action == GLFW_PRESS ) {
		printVertex ();
	}
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
	glfwSetKeyCallback (window, key_callback);

	// defines the correct function based on which OS we're compiling for
	if ( !gladLoadGLLoader ((GLADloadproc)glfwGetProcAddress) ) {
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

	Shader myShader ("../src/shader.vs", "../src/shader.fs");


	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------

	Vertex *v1 = new Vertex (0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f);
	Vertex *v2 = new Vertex (0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f);
	Vertex *v3 = new Vertex (-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f);
	fractal->addTriangle (v1, v2, v3);
	// Vertex Buffer Object, Vertex Array Object, Element Buffer Object


	glGenBuffers (1, &VBO);
	glGenVertexArrays (1, &VAO);

	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray (VAO);

	vertices = fractal->getVertex ();
	float sizeVertices = fractal->getNumVertex () * sizeof (float);
	glBindBuffer (GL_ARRAY_BUFFER, VBO);
	glBufferData (GL_ARRAY_BUFFER, sizeVertices, vertices, GL_DYNAMIC_DRAW);

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
		glClearColor (1.0f, 0.3f, 0.3f, 1.0f);
		glClear (GL_COLOR_BUFFER_BIT);

		myShader.use ();
		glBindVertexArray (VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
		glDrawArrays (GL_TRIANGLES, 0, fractal->getNumVertex ());

		glfwSwapBuffers (window); // swap buffers
		glfwPollEvents (); // checks if any events are triggered
	}

	// ------------------------------------------------------------------------
	glDeleteVertexArrays (1, &VAO);
	glDeleteBuffers (1, &VBO);
	glfwTerminate ();
	delete[] vertices;
	delete fractal;
	return 0;
}
