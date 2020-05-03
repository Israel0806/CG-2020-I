#ifndef SIERPINSKY_H
#define SIERPINSKY_H

#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

using namespace std;

mutex mut;

struct Vertex;
struct Triangle;
class Sierpinski;


void createTriangle (int index, int newIndex, float red, float green, float blue, vector<Triangle *> triangles, vector<Triangle *> &newTriangles);

struct Vertex {
	float x, y, z;
	float red, green, blue;

	friend ostream &operator<<(ostream &os, const Vertex *v) {
		return os << "coordinates: (" << v->x << "," << v->y << "," << v->z << "), color:(" << v->red << "," << v->green << "," << v->blue << ")" << endl;
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
	float getNumVertex () {
		int low = (level <= 0) ? 0 : levels[level - 1];
		int high = (level + 1 > maxLevel) ? triangles.size () : (level <= 0) ? 1 : levels[level];
		return ((float)high - (float)low) * 24.0f;
	}

	void addLevel () {
		if ( ++level == maxLevel )
			newLevel ();
		cout << "Current level: " << level << endl;
	}

	void removeLevel () {
		if ( level == 0 )
			return;
		--level;
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
		int _size = triangles.size () * 24;
		int _index = 0;
		Triangle *triangle;

		int low = (level <= 0) ? 0 : levels[level - 1];
		int high = (level + 1 > maxLevel) ? triangles.size () : (level <= 0) ? 1 : levels[level];
		cout << "Number of triangles retrieved: " << high - low << endl;
		float *_vertices = new float[(high - low) * 24];
		for ( int i = low; i < high; ++i ) {
			triangle = triangles[i];
			triangle->v1->copyInto (_vertices, _index);
			_vertices[_index++] = 1.0f;
			_vertices[_index++] = 2.0f;
			triangle->v2->copyInto (_vertices, _index);
			_vertices[_index++] = 2.0f;
			_vertices[_index++] = 0.0f;
			triangle->v3->copyInto (_vertices, _index);
			_vertices[_index++] = 0.0f;
			_vertices[_index++] = 0.0f;
		}
		return _vertices;
	}

	~Sierpinski () {
		while ( !triangles.empty () ) {
			Triangle *triangle = triangles.back ();
			triangles.pop_back ();
			delete triangle;
		}
	}
};

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

#endif // !SIERPINSKY_H