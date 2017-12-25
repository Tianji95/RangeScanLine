#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<fstream>
#include<cmath>
#include <omp.h>
#include"Mymath.h"
using namespace std;


vect::vect() {
	this->x = 0.0f;
	this->y = 0.0f;
	this->z = 0.0f;
	this->w = 0.0f;
}

vect::vect(double x, double y, double z) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = 1.0f;
}
vect::~vect() {

}

vect& vect::operator =(vect& v) {
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
	this->w = v.w;
	return *this;

}
void vect::setVect(double x, double y, double z) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = 1.0f;
}
//���������
vect* vect_add(vect *v1, vect *v2) {
	vect* out = new vect();
	out->x = v1->x + v2->x;
	out->y = v1->y + v2->y;
	out->z = v1->z + v2->z;
	out->w = 1.0f;
	return out;
}

void vect_sub(vect* out, vect* v1, vect* v2) {
	out->x = v1->x - v2->x;
	out->y = v1->y - v2->y;
	out->z = v1->z - v2->z;
	out->w = 1.0f;
}

//�������out = a * b;
double vect_dotmul(vect* v1, vect* v2) {
	return v1->x*v2->x + v1->y*v2->y + v1->z*v2->z;
}

//�������out = a x b;
void vect_crossmul(vect* out, vect* v1, vect* v2) {
	out->x = v1->y*v2->z - v1->z*v2->y;
	out->y = v1->z*v2->x - v1->x*v2->z;
	out->z = v1->x*v2->y - v1->y*v2->x;
	out->w = 1.0f;
}

float mySqrt(float number) {
	long i;
	float x2, y;
	const float threehalfs = 1.5F;
	x2 = number * 0.5F;
	y = number;
	i = *(long *)&y;                      
	i = 0x5f3759df - (i >> 1);            
	y = *(float *)&i;
	y = y * (threehalfs - (x2 * y * y)); 
	return y;
}

double vect_length(vect* v) {
	return mySqrt(v->x*v->x+v->y*v->y+v->z*v->z);
}




//������һ��
void vect_normalize(vect* out, vect* v){
	double inv = vect_length(v);
	if (inv != 0.0) {
		//double inv = 1.0f / invlength;
		out->x = v->x*inv;
		out->y = v->y*inv;
		out->z = v->z*inv;
		out->w = v->w;
	}
	else {
		cout << "��������Ϊ0������" << endl;
	}
}

void vect_interp(vect* out, vect* v1, vect* v2, double t) {
	out->x = v1->x + (v2->x - v1->x)*t;
	out->y = v1->y + (v2->y - v1->y)*t;
	out->z = v1->z + (v2->z - v1->z)*t;
}

void matr_identity(matr* m) {
	m->m[0][0] = m->m[1][1] = m->m[2][2] = m->m[3][3] = 1.0f; 
	m->m[0][1] = m->m[0][2] = m->m[0][3] = 0.0f;
	m->m[1][0] = m->m[1][2] = m->m[1][3] = 0.0f;
	m->m[2][0] = m->m[2][1] = m->m[2][3] = 0.0f;
	m->m[3][0] = m->m[3][1] = m->m[3][2] = 0.0f;
}

//����ӷ�
matr* matrix_add(matr* m1, matr* m2) {
	matr* out = new matr();
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			out->m[i][j] = m1->m[i][j] + m2->m[i][j];
		}
	}
	return out;
}

//�������
matr* matrix_sub(matr* m1, matr* m2) {
	matr* out = new matr();
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			out->m[i][j] = m1->m[i][j] - m2->m[i][j];
		}
	}
	return out;
}

//����˷�
void matrix_mul(matr* out,matr* m1, matr* m2) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			out->m[i][j] = m1->m[i][0] * m2->m[0][j] + m1->m[i][1] * m2->m[1][j] + m1->m[i][2] * m2->m[2][j] + m1->m[i][3] * m2->m[3][j];
		}
	}
}

// ƽ�Ʊ任
matr* translate_matrix(double x, double y, double z) {
	matr *m = new matr();
	matr_identity(m);
	m->m[3][0] = x;
	m->m[3][1] = y;
	m->m[3][2] = z;
	return m;
}

// ���ű任
matr* scale_matrix(double x, double y, double z) {
	matr *m = new matr();
	matr_identity(m);
	m->m[0][0] = x;
	m->m[1][1] = y;
	m->m[2][2] = z;
	return m;
}


//����һ����ת����thetaΪ��ת�Ƕȣ�vΪ��ת����
void rotate_matrix(matr* rotatematrix, double theta, vect* v) {
	double theSin = sin(theta);
	double theCos = cos(theta);
	vect_normalize(v, v);

	rotatematrix->m[0][0] = v->x*v->x + (1.0 - v->x*v->x)*theCos;
	rotatematrix->m[1][0] = v->x*v->y*(1.0 - theCos) - v->z*theSin;
	rotatematrix->m[2][0] = v->x*v->z*(1.0 - theCos) + v->y*theSin;
	rotatematrix->m[3][0] = 0;

	rotatematrix->m[0][1] = v->x*v->y*(1.0 - theCos) + v->z*theSin;
	rotatematrix->m[1][1] = v->y*v->y + (1.0 - v->y*v->y)*theCos;
	rotatematrix->m[2][1] = v->y*v->z*(1.0 - theCos) - v->x*theSin;
	rotatematrix->m[3][1] = 0;

	rotatematrix->m[0][2] = v->x*v->z*(1.0 - theCos) - v->y*theSin;
	rotatematrix->m[1][2] = v->y*v->z*(1.0 - theCos) + v->x*theSin;
	rotatematrix->m[2][2] = v->z*v->z + (1.0 - v->z*v->z)*theCos;
	rotatematrix->m[3][2] = 0;

	rotatematrix->m[0][3] = 0;
	rotatematrix->m[1][3] = 0;
	rotatematrix->m[2][3] = 0;
	rotatematrix->m[3][3] = 1;
}

//����������
void matrix_mul_vector(vect* out, matr* m, vect* v) {
	out->x = m->m[0][0] * v->x + m->m[0][1] * v->y + m->m[0][2] * v->z + m->m[0][3] * v->w;
	out->y = m->m[1][0] * v->x + m->m[1][1] * v->y + m->m[1][2] * v->z + m->m[1][3] * v->w;
	out->z = m->m[2][0] * v->x + m->m[2][1] * v->y + m->m[2][2] * v->z + m->m[2][3] * v->w;
	out->w = m->m[3][0] * v->x + m->m[3][1] * v->y + m->m[3][2] * v->z + m->m[3][3] * v->w;
}



//������˾���
void vector_mul_matrix(vect* out, matr* m, vect* v) {
	out->x = m->m[0][0] * v->x + m->m[1][0] * v->y + m->m[2][0] * v->z + m->m[3][0] * v->w;
	out->y = m->m[0][1] * v->x + m->m[1][1] * v->y + m->m[2][1] * v->z + m->m[3][1] * v->w;
	out->z = m->m[0][2] * v->x + m->m[1][2] * v->y + m->m[2][2] * v->z + m->m[3][2] * v->w;
	out->w = m->m[0][3] * v->x + m->m[1][3] * v->y + m->m[2][3] * v->z + m->m[3][3] * v->w;
}


void matr_inv(matr* out, matr* in) {

}

void matrix_display(matr* m) {
	cout << "���¾������ֵΪ��" << endl;
	for (int i = 0; i < 4; i++) {
		cout << "��" << i << "��" << endl;
		cout << m->m[i][0] << " " << m->m[i][1] << " " << m->m[i][2] << " " << m->m[i][3] << endl;
	}
}

bool checkCvv(vect* v_in) {
	double w = v_in->w;
	if (v_in->z < - w)return false;
	if (v_in->z > 0.0f) return false;
	if (v_in->x < -w)return false;
	if (v_in->x > w)return false;
	if (v_in->y < -w)return false;
	if (v_in->y > w)return false;
	return true;
}

point::point() {
	this->r = 0;
	this->g = 0;
	this->b = 0;
	this->u = 0;
	this->v = 0;
}

point::point(double positionX, double positionY, double positionZ, double r, double g, double b, double u, double v, double nx, double ny, double nz, double rate){
	this->pos.setVect(positionX, positionY, positionZ);
	this->nvect.setVect(nx, ny, nz);
	this->r = r;
	this->g = g;
	this->b = b;
	this->u = u;
	this->v = v;
	this->nvect.w = 0;
}
point::~point() {
}
void point::setPoint(double positionX, double positionY, double positionZ, double positionW, double r, double g, double b, double u, double v, double nx, double ny, double nz, double rate) {
	this->pos.x = positionX;
	this->pos.y = positionY;
	this->pos.z = positionZ;
	this->pos.w = positionW;
	this->r = r;
	this->g = g;
	this->b = b;
	this->u = u;
	this->v = v;
	this->nvect.x = nx;
	this->nvect.y = ny;
	this->nvect.z = nz;
	this->nvect.w = 0;
}

void point::setPoint(double positionX, double positionY, double positionZ) {
	this->pos.x = positionX;
	this->pos.y = positionY;
	this->pos.z = positionZ;
	this->pos.w = 1;
	this->r = 1;
	this->g = 1;
	this->b = 1;
	this->u = 0;
	this->v = 0;
	this->nvect.x = 0;
	this->nvect.y = 0;
	this->nvect.z = 0;
	this->nvect.w = 0;
}


point& point::operator =(point& p) {
	this->pos.x = p.pos.x;
	this->pos.y = p.pos.y;
	this->pos.z = p.pos.z;
	this->pos.w = p.pos.w;

	this->nvect.x = p.nvect.x;
	this->nvect.y = p.nvect.y;
	this->nvect.z = p.nvect.z;
	this->r = p.r;
	this->g = p.g;
	this->b = p.b;
	this->u = p.u;
	this->v = p.v;
	return *this;
}

void point_interp(point* out, point* p1, point* p2, double t) {
	out->pos.x = p1->pos.x + (p2->pos.x - p1->pos.x)*t;
	out->pos.y = p1->pos.y + (p2->pos.y - p1->pos.y)*t;
	out->r = p1->r + (p2->r - p1->r)*t;
	out->g = p1->g + (p2->g - p1->g)*t;
	out->b = p1->b + (p2->b - p1->b)*t;
	out->nvect.x = p1->nvect.x;
	out->nvect.y = p1->nvect.y;
	out->nvect.z = p1->nvect.z;
	if (p1->pos.z == p2->pos.z) {
		out->u = p1->u + (p2->u - p1->u)*t;
		out->v = p1->v + (p2->v - p1->v)*t;
		out->pos.z = p1->pos.z + (p2->pos.z - p1->pos.z)*t;
	}
	else {
		out->pos.z = (p1->pos.z*p2->pos.z)/ ((p1->pos.z - p2->pos.z)*t + p2->pos.z);
		double rate = out->pos.z / (p1->pos.z*p2->pos.z);
		out->u = (p1->u*p2->pos.z*(1.0f - t) + t*p2->u*p1->pos.z)*rate;
		out->v = (p1->v*p2->pos.z*(1.0f - t) + t*p2->v*p1->pos.z)*rate;
	}

}



Face::Face() {}

Face::~Face() {}

Model::Model(){}
Model::~Model(){}
void Model::initModel() {
	this->faceVector.clear();
	this->pointVector.clear();
	this->normalVector.clear();
}
void Model::ReadModelFromFile(const char* filename) {
	FILE*   file;
	unsigned int v, n, t;
	float x, y, z;
	char        buf[128];
	numVertex = 0;
	numFaces = 0;
	numNormals = 0;
	file = fopen(filename, "r");
	if (!file) {
		printf_s("can not open file !!!\n");
	}
	rewind(file);
	while (fscanf(file, "%s", buf) != EOF) {
		if(buf[0] == 'v'){
			if (buf[1] == '\0') {        /* vertex */
				point newPoint;
				fscanf(file, "%f %f %f", &x, &y, &z);
				newPoint.setPoint(x, y, z);
				pointVector.push_back(newPoint);
				numVertex++;
			}
			else if (buf[1] == 'n') {       /* normal */
				vect newVect;
				fscanf(file, "%f %f %f", &x, &y, &z);
				newVect.setVect(x, y, z);
				normalVector.push_back(newVect);
				numNormals++;
			}
		}
		else if (buf[0] == 'f') {
			fscanf(file, "%s", buf);
			if (strstr(buf, "//")) {
				sscanf(buf, "%u//%u", &v, &n);
				Face newFace;
				pointVector[v - 1].nvect = normalVector[n - 1];
				newFace.pointVector.push_back(pointVector[v - 1]);
				fscanf(file, "%u//%u", &v, &n);
				pointVector[v - 1].nvect = normalVector[n - 1];
				newFace.pointVector.push_back(pointVector[v - 1]);
				fscanf(file, "%u//%u", &v, &n);
				pointVector[v - 1].nvect = normalVector[n - 1];
				newFace.pointVector.push_back(pointVector[v - 1]);
				while (fscanf(file, "%u//%u", &v, &n) > 0) {
					pointVector[v - 1].nvect = normalVector[n - 1];
					newFace.pointVector.push_back(pointVector[v - 1]);
				}
				numFaces++;
				faceVector.push_back(newFace);
			}
			else if(sscanf(buf, "%u/%u/%u", &v, &t, &n) == 3){
				Face newFace;
				pointVector[v - 1].nvect = normalVector[n - 1];
				newFace.pointVector.push_back(pointVector[v - 1]);
				fscanf(file, "%u/%u/%u", &v, &t, &n);
				pointVector[v - 1].nvect = normalVector[n - 1];
				newFace.pointVector.push_back(pointVector[v - 1]);
				fscanf(file, "%u/%u/%u", &v, &t, &n);
				pointVector[v - 1].nvect = normalVector[n - 1];
				newFace.pointVector.push_back(pointVector[v - 1]);
				while (fscanf(file, "%u/%u/%u", &v, &t, &n) > 0) {
					pointVector[v - 1].nvect = normalVector[n - 1];
					newFace.pointVector.push_back(pointVector[v - 1]);
				}
				numFaces++;
				faceVector.push_back(newFace);

			}
		}
	}
}

