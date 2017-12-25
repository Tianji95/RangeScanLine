#pragma once
#ifndef _MYMATH_H_
#define _MYMATH_H_
#include<vector>
#define MAX_NUM_TRIANGLE 100000
#define MAX_NUM_VERTEX 100000
using namespace std;
class vect{
public:
	vect();
	vect(double x, double y, double z);
	~vect();
	vect& operator =(vect& v);
	void setVect(double x, double y, double z);
	double x = 0, y = 0, z = 0, w = 0;
};

class matr {
public:
	double m[4][4];
};

class point {
public:
	point();
	point& operator =(point& a);
	point(double positionX, double positionY, double positionZ, double r,double g, double b, double u ,double v, double nx, double ny, double nz, double rate);
	void setPoint(double positionX, double positionY, double positionZ, double positionW, double r, double g, double b, double u, double v, double nx, double ny, double nz, double rate);
	void setPoint(double positionX, double positionY, double positionZ);
	~point();
	vect pos;
	double r;
	double g;
	double b;
	double u;
	double v;
	vect nvect;
};

class Face {
public:
	Face();
	~Face();
	vector<point> pointVector;
	double A = 0, B = 0, C = 0, D = 0;
	double r, g, b;
	bool in_out_flag;
	double nowZ;
	int id;

};

class Model {
public:
	void ReadModelFromFile(const char* filename);
	Model();
	~Model();
	void initModel();
	vector<Face> faceVector;
	vector<point> pointVector;
	vector<vect> normalVector;
	int numNormals;
	int numFaces;
	int numVertex;
};

vect* vect_add(vect* v1, vect* v2);
void vect_sub(vect* out,vect* v1, vect* v2);
double vect_dotmul(vect* v1, vect* v2);
void vect_crossmul(vect* out, vect* v1, vect* v2);
double vect_length(vect* v);
void vect_normalize(vect* out, vect* v);
void vect_interp(vect* out, vect* v1, vect* v2, double t);

void matr_identity(matr* m);
matr* matrix_add(matr* m1, matr* m2);
matr* matrix_sub(matr* m1, matr* m2);
void matrix_mul(matr* out, matr* m1, matr* m2);
matr* translate_matrix(double x, double y, double z);
matr* scale_matrix(double x, double y, double z);

void rotate_matrix(matr* rotatematrix, double theta, vect* v);
void vector_mul_matrix(vect* out, matr* m, vect* v);
void matrix_mul_vector(vect* out, matr* m, vect* v);
bool checkCvv(vect* v_in);

void point_interp(point* out, point* p1, point* p2, double t);

void matr_inv(matr* out, matr* in);


void matrix_display(matr* m);
#endif // !_MYMATH_H_
