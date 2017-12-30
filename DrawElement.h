#pragma once
#ifndef _DRAWELEMENT_H_
#define _DRAWELEMENT_H_
#include<Windows.h>
#include<list>
#include"Transform.h"
#include"MyMath.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define TEXTURE_MODE 4
#define COLOR_MODE 2
#define WINDOWS_CLASS_NAME "DrawElement"
#define PI 3.1415926536


class ActivateEdge{
public:
	double x, dx;
	int id, ymax;// 属于哪一个面
};


class Scanline {
public:
	Scanline();
	~Scanline();
	list<ActivateEdge> edgeList;
};




class DrawElement {
public:

	DrawElement();
	~DrawElement();
	HRESULT initDevice();
	void clearBuffer();
	void destoryDevice();
	static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void ModelToEdge(Model *in_model);
	void translateModel(Model *out_model, Model *in_model);
	int translateFace(Face *in_face, Face *out_face);
	void drawScanLine(Model *in_model);
	void updateScene();

	Transform transform;//矩阵变换
	vect lightDiffuse;//
	vect lightSpecular;
	vect lightAmbient;
	vect lightPos;
	vect cameraPosition;
	vect cameraTarget;
	vect cameraUp;
	HDC hMemDC;//一个兼容设备的设备描述句柄
	HBITMAP hBITMAP;//一个位图句柄
	HDC hdc;//windows设备描述句柄

	LARGE_INTEGER startTick = {};
	LARGE_INTEGER endTick = {};
	LARGE_INTEGER frequency_ = {};
	FLOAT fps_ = 60.0f;

private:
	float ks = 1;//高光系数
	float kd = 1;//漫反射光系数
	float ka = 1;//环境光系数
	float ke = 0.1f; //自发射光系数
	int shininess = 3;
	HINSTANCE hinstSelf;
	HWND hwnd;//windows  窗体句柄
	BITMAPINFO bmpInfo;//需要画的位图信息
	Scanline scanlineArray[WINDOW_HEIGHT];
	int y_min = WINDOW_HEIGHT;
	int y_max = 0;

	UINT32 *bmpBuffer_;
};

#endif // !_DRAWELEMENT_H_
