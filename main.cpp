//´úÂë²Î¿¼×Ô£ºhttps://www.zhihu.com/question/24786878
//https://www.zhihu.com/question/33712299

#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include"MyMath.h"
#include"Transform.h"
#include"DrawElement.h"

using namespace std;
CHAR fpsBuffer[256];
LARGE_INTEGER delta;


int main() {
	DrawElement *dw = new DrawElement();
	Model ori_model, final_model;
	dw->initDevice();
	/* World-View matrix */
	dw->cameraPosition.setVect(0.0, 0.0, 10.0);
	dw->cameraTarget.setVect(0.0, 0.0, 0.0);
	dw->cameraUp.setVect(0.0, 1.0, 0.0);
	dw->transform.set_view_matrix(&dw->cameraPosition, &dw->cameraTarget, &dw->cameraUp);
	matr_identity(&dw->transform.world_matrix);
	dw->transform.set_transform_matrix();
	dw->lightPos.setVect(0.0f, 0.0f, 10.0f);
	dw->lightDiffuse.setVect(0.0f, 0.0f, 1.0f);
	dw->lightSpecular.setVect(1.0f, 1.0f, 1.0f);
	dw->lightAmbient.setVect(0.0f, 0.0f, 1.0f);

	dw->clearBuffer();
	ori_model.ReadModelFromFile("cow.obj");
	dw->translateModel(&final_model, &ori_model);
	cout << final_model.faceVector.size() << endl;
	dw->ModelToEdge(&final_model);
	dw->drawScanLine(&final_model);
	dw->updateScene();

	vect rotateRow;
	rotateRow.setVect(0, 1, 0);
	matr tempMatrix;
	double theta = 0.001f*PI;

	QueryPerformanceFrequency(&dw->frequency_);
	matr rotateMatrix;
	while (true) {
		theta = 0.1*PI / dw->fps_;
		rotate_matrix(&rotateMatrix, theta, &rotateRow);

		tempMatrix = dw->transform.world_matrix;
		matrix_mul(&dw->transform.world_matrix, &rotateMatrix, &tempMatrix);
		dw->transform.set_transform_matrix();
		QueryPerformanceCounter(&dw->startTick);
		dw->clearBuffer();
		final_model.initModel();
		
		dw->translateModel(&final_model, &ori_model);
		cout << ori_model.faceVector.size() << endl;
		dw->ModelToEdge(&final_model);
		dw->drawScanLine(&final_model);



		
		/* Count FPS */
		QueryPerformanceCounter(&dw->endTick);
		delta.QuadPart = dw->endTick.QuadPart - dw->startTick.QuadPart;
		dw->fps_ = 1.0f * dw->frequency_.QuadPart / delta.QuadPart;
		sprintf_s(fpsBuffer, 256, "FPS: %.3f", dw->fps_);
		SetBkColor(dw->hMemDC, RGB(0xD7, 0xC4, 0xBB));
		SetTextColor(dw->hMemDC, RGB(0x00, 0x00, 0xFF));
		TextOut(dw->hMemDC, 0, 0, fpsBuffer, lstrlen(fpsBuffer));
		SelectObject(dw->hMemDC, dw->hBITMAP);
		BitBlt(dw->hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, dw->hMemDC, 0, 0, SRCCOPY);
		dw->updateScene();
	}
	dw->destoryDevice();
	delete dw;
	return 0;

}