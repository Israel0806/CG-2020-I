#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include "shader.h"

#include <iostream>
#include <time.h>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;

int width = 800, height = 600;
unsigned int VBO, VAO;
float *vertices = nullptr;
mutex mut;

struct Vertex;
struct Triangle;
class Sierpinski;

void createTriangle (int index, int newIndex, float red, float green, float blue, vector<Triangle *> triangles, vector<Triangle *> &newTriangles);

struct Vertex {
	float x, y, z;
	float red, green, blue;

	friend ostream &operator<<(ostream &os, const Vertex *v) {
		return os <<"coordinates: (" << v->x << "," << v->y << "," << v->z << "), color:(" << v->red << "," << v->green << "," << v->blue << ")" << endl;
	}

	Vertex (float x, float y, float z, float red, float green, float blue) : x (x), y (y), z (z), red (red), green (green), blue (blue) {};

	Vertex (Vertex *v) {
		x = v->x;
		y = v->y;
		z = v->z;
		red = v->red;
		green = v->green;
		blue = v->blue;
	}

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

	Triangle (Vertex *v1, Vertex *v2, Vertex *v3) {
		this->v1 = v1;
		this->v2 = v2;
		this->v3 = v3;
	}

	void print () {
		cout << v1 << v2 << v3 << endl;
	}

	Vertex *midPoint (Vertex *v1, Vertex *v2, float red, float green, float blue) {
		return new Vertex ((v1->x + v2->x) / 2.0f, (v1->y + v2->y) / 2.0f, (v1->z + v2->z) / 2.0f, red, green, blue);
	}

	~Triangle () {
		delete v1, v2, v3;
	}
};

class Sierpinski {
	vector<Triangle *> triangles;
	int level; /// index del nivel actual
	int maxLevel;
	int levels[20]; /// indices de cuantos triangulos dibujar por nivel
	int index; /// el index por el cual debe empezar a dividir los triangulos
public:
	Sierpinski () {
		index = 0;
		level = 0;
		maxLevel = 0;
		levels[0] = 1;
	}
	void addTriangle (Vertex *v1, Vertex *v2, Vertex *v3) {
		triangles.push_back (new Triangle (v1, v2, v3));
	}

	//float getNumVertex () { return (float)triangles.size () * 18.0f; }
	float getNumVertex () { return (float)levels[level] * 18.0f; }

	void addLevel () {
		if ( ++level == maxLevel )
			newLevel ();
		reDraw ();
		cout << "Current level: " << level << endl;
	}

	void removeLevel () {
		if ( level == 0 )
			return;
		--level;
		reDraw ();
		cout << "Current level: " << level << endl;
	}

	void createLevels (int maxLevel) {
		for ( int i = 0; i < 10; ++i )
			newLevel ();
	}

	void newLevel () {
		int newIndex = triangles.size ();
		levels[maxLevel++] = newIndex;
		float red = rand () % 2;
		float green = rand () % 2;
		float blue = rand () % 2;
		vector<Triangle *> newTriangles = triangles;

		int cores = thread::hardware_concurrency ();
		if ( newIndex - index > cores ) {
			thread *threads = new thread[cores];
			int start = index;
			int jump = (newIndex - index) / cores;
			int end;
			for ( int i = 0; i < cores; ++i ) {
				end = (i + 1 != cores) ? (start + jump) : newIndex;
				threads[i] = thread (createTriangle, start, end, red, green, blue, triangles, ref (newTriangles));
				start = end;
			}
			for ( int i = 0; i < cores; ++i )
				threads[i].join ();
			delete[] threads;
		} else {
			createTriangle (index, newIndex, red, green, blue, triangles, newTriangles);
		}
		index = newIndex;
		triangles.clear ();
		triangles = newTriangles;
		cout << "End doing level, number of triangles: " << triangles.size () << endl;
	}

	void print () {
		int low = (level <= 0) ? 0 : levels[level - 1];
		int high = (level + 1 > maxLevel) ? triangles.size () : (level <= 0) ? 1 : levels[level];
		cout << "Number of triangles: " << high - low << endl;
		Triangle *triangle;
		for ( int i = low; i < high; ++i ) {
			triangle = triangles[i];
			triangle->print ();
		}
	}

	float *getVertex () {
		int _size = triangles.size () * 18;
		float *_vertices = new float[_size];
		int _index = 0;
		Triangle *triangle;

		int low = (level <= 0) ? 0 : levels[level - 1];
		int high = (level + 1 > maxLevel) ? triangles.size () : (level <= 0) ? 1 : levels[level];
		for ( int i = low; i < high; ++i ) {
			triangle = triangles[i];
			triangle->v1->copyInto (_vertices, _index);
			triangle->v2->copyInto (_vertices, _index);
			triangle->v3->copyInto (_vertices, _index);
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

void createTriangle (int index, int newIndex, float red, float green, float blue, vector<Triangle *> triangles, vector<Triangle *> &newTriangles) {
	Triangle *triangle;
	for ( int i = index; i < newIndex; ++i ) {
		triangle = triangles[i];

		Vertex *_v1 = triangle->midPoint (triangle->v1, triangle->v2, red, green, blue);
		Vertex *_v2 = triangle->midPoint (triangle->v2, triangle->v3, red, green, blue);
		Vertex *_v3 = triangle->midPoint (triangle->v3, triangle->v1, red, green, blue);

		mut.lock ();
		newTriangles.push_back (new Triangle (new Vertex (triangle->v1), _v1, _v3));
		newTriangles.push_back (new Triangle (_v1, new Vertex (triangle->v2), _v2));
		newTriangles.push_back (new Triangle (_v3, _v2, new Vertex (triangle->v3)));
		mut.unlock ();
	}
}

void reDraw () {
	delete[] vertices;
	vertices = fractal->getVertex ();
	float sizeVertices = fractal->getNumVertex () * sizeof (float);
	glBindBuffer (GL_ARRAY_BUFFER, VBO);
	glBufferData (GL_ARRAY_BUFFER, sizeVertices, vertices, GL_DYNAMIC_DRAW);
}

void printVertex () {
}

void framebuffer_size_callback (GLFWwindow *window, int w, int h) {
	glViewport (500, 500, w, h);
}

Shader *shader1 = nullptr;
Shader *shader2 = nullptr;
Shader *shader3 = nullptr;
Shader *shaderActual = nullptr;

void key_callback (GLFWwindow *window, int key, int scancode, int action, int mods) {
	if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
		glfwSetWindowShouldClose (window, true);
	if ( key == GLFW_KEY_UP && action == GLFW_PRESS )
		fractal->addLevel ();
	if ( key == GLFW_KEY_DOWN && action == GLFW_PRESS )
		fractal->removeLevel ();
	if ( key == GLFW_KEY_P && action == GLFW_PRESS )
		fractal->print ();
	if ( key == GLFW_KEY_1 && action == GLFW_PRESS )
		shaderActual = shader1;
	if ( key == GLFW_KEY_2 && action == GLFW_PRESS )
		shaderActual = shader2;
	if ( key == GLFW_KEY_3 && action == GLFW_PRESS )
		shaderActual = shader3;
}

void scrollCallback (GLFWwindow *window, double xOffset, double yOffset) {
	if ( yOffset == -1 )
		fractal->removeLevel ();
	else
		fractal->addLevel ();
}

int main () {
	srand (time (NULL));
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
	glfwSetScrollCallback (window, scrollCallback);

	// defines the correct function based on which OS we're compiling for
	if ( !gladLoadGLLoader ((GLADloadproc)glfwGetProcAddress) ) {
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	shader1 = new Shader ("../src/shader1.vs", "../src/shader1.fs"); /// color esta en los valores del vertice como rgb
	shader2 = new Shader ("../src/shader2.vs", "../src/shader1.fs"); /// los colores son segun la posicion de la interfaz
	shader3 = new Shader ("../src/shader1.vs", "../src/shader2.fs"); /// los colores cambian segun el tiempo
	shaderActual = shader1;

	Vertex *v1 = new Vertex (0.0f, 0.9f, 0.0f, 1.0f, 0.0f, 0.0f);
	Vertex *v2 = new Vertex (0.5f, -0.1f, 0.0f, 1.0f, 0.0f, 0.0f);
	Vertex *v3 = new Vertex (-0.5f, -0.1f, 0.0f, 1.0f, 0.0f, 0.0f);
	fractal->addTriangle (v1, v2, v3);
	// Vertex Buffer Object, Vertex Array Object, Element Buffer Object
	fractal->createLevels (10);

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
		float time = glfwGetTime ();
		float red = cos (time) / 2 + 0.5f;
		float green = sin (time) / 2 + 0.5f;
		float blue = cos (time) / 2 + 0.5f;
		shaderActual->use ();
		float color[4] = { red,green,blue, 1.0f };
		shader3->setFloat ("changingColor", color);
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
