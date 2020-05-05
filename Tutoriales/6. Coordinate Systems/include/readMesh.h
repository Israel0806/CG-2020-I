#ifndef READMESH_H
#define READMESH_H

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

struct Vertex {
	float coord[3];
	float texCoord[2];
public:
	Vertex () {
		for ( int i = 0; i < 3; ++i )
			coord[i] = 0.0f;
			texCoord[0] = 0.0f;
			texCoord[1] = 1.0f;
	}

	void addCoor (float value, int pos) {
		coord[pos] = value;
	}
	void addTexCoor (float value, int pos) {
		texCoord[pos] = value;
	}
};

struct Face {
	int index[3];
public:
	Face () {
		for ( int i = 0; i < 3; ++i )
			index[i] = 0.0f;
	}

	void addIndex (int value, int pos) {
		index[pos] = value;
	}
};

class Mesh {
	vector<Vertex *> LVertex;
	vector<Face *> LFace;
public:
	Mesh (string name) {
		ifstream file;
		file.open (name.c_str ());
		string line;
		int pos = 0;
		while ( getline (file, line) ) {
			string idk = line.substr (0, line.find (' '));
			string temp = line;
			line = line.substr (line.find (' ') + 1);
			if ( idk == "v" )
				addVertex (line);
			else if ( idk == "f" )
				addFace (line);
			//else if ( idk == "vt" )
			//	addTCoor (line, pos);
		}

		file.close ();
	}

	void addTCoor (string line, int &pos) {
		int position = line.find (' ');
		float coor = stof (line.substr (0, position));
		LVertex[pos]->addTexCoor (coor, 0);
		line = line.substr (position + 1);

		coor = stof (line);
		LVertex[pos]->addTexCoor (coor, 1);
		++pos;
	}
	void addFace (string line) {
		Face *face = new Face ();
		int pos = 0;
		while ( line.find (' ') != -1 ) {
			int position = line.find (' ');
			string faceIndex = line.substr (0, position);
			int faceI = stoi (faceIndex.substr (0, faceIndex.find ('/'))) - 1;
			face->addIndex (faceI, pos);
			++pos;
			line = line.substr (position + 1);
		}
		int faceI = stoi (line.substr (0, line.find ('/'))) - 1;
		face->addIndex (faceI, pos);
		LFace.push_back (face);
	}

	void addVertex (string line) {
		Vertex *vertex = new Vertex ();
		int pos = 0;
		while ( line.find (' ') != -1 ) {
			int position = line.find (' ');
			float coor = stof (line.substr (0, position));
			vertex->addCoor (coor, pos);
			++pos;
			line = line.substr (position + 1);
		}
		float coor = stof (line);
		vertex->addCoor (coor, pos);
		LVertex.push_back (vertex);
	}

	float *getVertexs (int &sizeVertex) {
		sizeVertex = LVertex.size () * 5;
		float *ver = new float[sizeVertex];
		int index = 0;
		for ( auto &v : LVertex ) {
			ver[index++] = v->coord[0];
			ver[index++] = v->coord[1];
			ver[index++] = v->coord[2];
			ver[index++] = v->texCoord[0];
			ver[index++] = v->texCoord[1];
		}
		return ver;
	}

	int *getFaces (int &size) {
		size = LFace.size () * 3;
		int *ind = new int[size];
		int index = 0;
		for ( auto &f : LFace ) {
			ind[index++] = f->index[0];
			ind[index++] = f->index[1];
			ind[index++] = f->index[2];
		}
		return ind;
	}

};


#endif 