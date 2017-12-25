#include"DrawElement.h"
#include<stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
Scanline::Scanline() {

}

Scanline::~Scanline() {
}


DrawElement::DrawElement() {
	zBuffer_ = nullptr;
	bmpBuffer_ = nullptr;
	zBuffer_ = nullptr;
}

DrawElement::~DrawElement() {
	this->destoryDevice();
}




HRESULT DrawElement::initDevice()
{
	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = wndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = hinstSelf;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = WINDOWS_CLASS_NAME;
	wcex.hIconSm = LoadIcon(hinstSelf, IDI_APPLICATION);
	RegisterClassEx(&wcex);
	hwnd = CreateWindowEx(
		0,
		WINDOWS_CLASS_NAME,
		"SoftRendering",
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		NULL,
		NULL,
		GetModuleHandle(nullptr),
		this
	);

	HRESULT hr = S_OK;
	hdc = GetDC(hwnd);//根据窗体句柄得到窗体中的设备描述句柄
	hMemDC = CreateCompatibleDC(hdc);//根据设备描述句柄得到一个兼容的设备描述句柄
	hBITMAP = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT); //根据设备描述句柄得到一个位图句柄
	SelectObject(hMemDC, hBITMAP);
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = WINDOW_WIDTH;
	bmpInfo.bmiHeader.biHeight = -WINDOW_HEIGHT;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32; 
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	bmpInfo.bmiHeader.biSizeImage = 0;
	bmpInfo.bmiHeader.biXPelsPerMeter = 0;
	bmpInfo.bmiHeader.biYPelsPerMeter = 0;
	bmpInfo.bmiHeader.biClrUsed = 0;
	bmpInfo.bmiHeader.biClrImportant = 0;
	hBITMAP = CreateDIBSection(hMemDC, &bmpInfo, DIB_RGB_COLORS, (void**)&bmpBuffer_, NULL, 0);//创建一个可以直接写的buffer
	zBuffer_ = new double[WINDOW_HEIGHT * WINDOW_WIDTH];
	int i;
	for (i = 0; i < WINDOW_HEIGHT * WINDOW_WIDTH; i++) {
		zBuffer_[i]= -100;
	}


	/*Z Buffer and Texture*/

	/* set Projection parameters */
	transform.set_projection_matrix(0.33f*PI, (double)WINDOW_WIDTH / (double)WINDOW_HEIGHT, 1.0f, 100.0f);
	transform.set_transform_matrix();
	/* Light */
	/* FPS counter */
	ShowWindow(hwnd, SW_SHOWNORMAL);
	UpdateWindow(hwnd);
	return hr;
}


void DrawElement::clearBuffer() {
	int i;
	for (i = 0; i < WINDOW_HEIGHT * WINDOW_WIDTH; i++) {
		this->bmpBuffer_[i] = 0x00000000;
		zBuffer_[i] = -100;
	}
	for (i = 0; i < WINDOW_HEIGHT; i++) {
		scanlineArray[i].edgeList.clear();
	}
	
}


void DrawElement::destoryDevice() {
	ReleaseDC(hwnd, hMemDC);
	delete[] zBuffer_;
	delete[] bmpBuffer_;
}

LRESULT DrawElement::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
		case 104:
			break;
		case 105:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int DrawElement::translateFace(Face *in_face, Face *out_face) {
	vector<vect> transV;
	vector<vect> worldV;
	vect worldnormalV;
	double totalNormalX = 0.0f, totalNormalY = 0.0f, totalNormalZ = 0.0f, totalOriginX = 0.0f, totalOriginY = 0.0f, totalOriginZ = 0.0f, totalWorldX = 0.0f, totalWorldY = 0.0f,totalWorldZ = 0.0f;
	vect avgNormalVect, avgOriginPos, avgWorldPos;
	vect pointToCamera, pointToLight;
	vect tempH, H;
	vector<point> *in = &in_face->pointVector;
	vector<point> *out = &out_face->pointVector;

	for (auto point_iter = in->begin(); point_iter != in->end(); ++point_iter) {
		vect newVect1, newVect2;
		vector_mul_matrix(&newVect1, &this->transform.transform_matrix, &point_iter->pos);
		if (!checkCvv(&newVect1)) {
			return 0;//判断是否在裁剪空间里面
		}
		transV.push_back(newVect1);
		vector_mul_matrix(&newVect2, &this->transform.world_matrix, &point_iter->pos);
		worldV.push_back(newVect2);
		totalNormalX += point_iter->nvect.x;
		totalNormalY += point_iter->nvect.y;
		totalNormalZ += point_iter->nvect.z;
		totalOriginX += point_iter->pos.x;
		totalOriginY += point_iter->pos.y;
		totalOriginZ += point_iter->pos.z;
		totalWorldX += newVect2.x;
		totalWorldY += newVect2.y;
		totalWorldZ += newVect2.z;

	}

	avgNormalVect.setVect(totalNormalX / in->size(), totalNormalY / in->size(), totalNormalZ / in->size());
	avgOriginPos.setVect(totalOriginX / in->size(), totalOriginY / in->size(), totalOriginZ / in->size());
	avgWorldPos.setVect(totalWorldX / in->size(), totalWorldY / in->size(), totalWorldZ / in->size());
	vector_mul_matrix(&worldnormalV, &this->transform.world_matrix, &avgNormalVect);
	if (vect_dotmul(&worldnormalV, &this->cameraPosition) < 0) {
		return 0;
	}
	out_face->in_out_flag = false;



	//获取光照的颜色
	tempH.setVect(this->cameraPosition.x - avgOriginPos.x, this->cameraPosition.y - avgOriginPos.y, this->cameraPosition.z - avgOriginPos.z);
	vect_normalize(&pointToCamera, &tempH);
	tempH.setVect(this->lightPos.x - avgOriginPos.x, this->lightPos.y - avgOriginPos.y, this->lightPos.z - avgOriginPos.z);
	vect_normalize(&pointToLight, &tempH);
	tempH.setVect((pointToLight.x + pointToCamera.x)*0.5f, (pointToLight.y + pointToCamera.y)*0.5f, (pointToLight.z + pointToCamera.z)*0.5f);
	vect_normalize(&H, &tempH);
	double maxdot = max(vect_dotmul(&worldnormalV, &H), 0);

	out_face->r = maxdot;//rand() % (155 + 1) + 100;
	out_face->g = maxdot;//rand() % (155 + 1) + 100;
	out_face->b = maxdot;//rand() % (155 + 1) + 100;
	for (unsigned int i = 0; i < in->size(); ++i) {
		vect newscreenV;
		point outP;
		getScreenPos(&newscreenV, &transV[i], WINDOW_WIDTH, WINDOW_HEIGHT);//获取屏幕坐标
		outP.setPoint(newscreenV.x, newscreenV.y, newscreenV.z, 1.0f, maxdot, maxdot, maxdot, (*in)[i].u, (*in)[i].v, worldnormalV.x, worldnormalV.y, worldnormalV.z, transV[i].w);
		out->push_back(outP);
	}
	out_face->nowZ = (*out)[0].pos.z;
	//out_face->A = worldnormalV.x;
	//out_face->B = worldnormalV.y;
	//out_face->C = worldnormalV.z;
	//out_face->D = -(out_face->A*(*out)[0].pos.x + out_face->B*(*out)[0].pos.y + out_face->C*(*out)[0].pos.z);

	return 1;
}


void DrawElement::translateModel(Model *out_model, Model *in_model) {
	int i;
	out_model->faceVector.clear();

	for (i = 0; i < in_model->faceVector.size(); ++i) {
		Face transFace;
		if (translateFace(&in_model->faceVector[i], &transFace) == 1) {
			out_model->faceVector.push_back(transFace);
		}
	}
}



void DrawElement::ModelToEdge(Model *in_model) {
	
	int point_iter = 0;
	point maxyP, minyP;

	for (int face_iter = 0; face_iter < in_model->faceVector.size(); face_iter++) {
		point_iter = 0;
		for (; point_iter < in_model->faceVector[face_iter].pointVector.size();point_iter++) {
			if (point_iter != in_model->faceVector[face_iter].pointVector.size() - 1) {
				if (in_model->faceVector[face_iter].pointVector[point_iter].pos.y < in_model->faceVector[face_iter].pointVector[point_iter + 1].pos.y) {
					maxyP = in_model->faceVector[face_iter].pointVector[point_iter + 1];
					minyP = in_model->faceVector[face_iter].pointVector[point_iter];
				}
				else {
					maxyP = in_model->faceVector[face_iter].pointVector[point_iter];
					minyP = in_model->faceVector[face_iter].pointVector[point_iter + 1];
				}
			}
			else {
				if (in_model->faceVector[face_iter].pointVector[point_iter].pos.y < in_model->faceVector[face_iter].pointVector[0].pos.y) {
					maxyP = in_model->faceVector[face_iter].pointVector[0];
					minyP = in_model->faceVector[face_iter].pointVector[point_iter];
				}
				else {
					maxyP = in_model->faceVector[face_iter].pointVector[point_iter];
					minyP = in_model->faceVector[face_iter].pointVector[0];
				}
			}

			ActivateEdge newActiEdge;
			newActiEdge.id = face_iter;
			newActiEdge.x = minyP.pos.x;
			newActiEdge.ymax = maxyP.pos.y;
			if ((maxyP.pos.y - minyP.pos.y) != 0) {
				newActiEdge.dx = (maxyP.pos.x - minyP.pos.x) / (maxyP.pos.y - minyP.pos.y);
				scanlineArray[(int)minyP.pos.y].edgeList.push_back(newActiEdge);
			}
		}
	}
}


bool EdgeXcompareRule(const ActivateEdge & m1, const ActivateEdge & m2) {
	return m1.x < m2.x;
}

bool FaceZcompareRule(const Face & f1, const Face & f2) {
	return f1.nowZ > f2.nowZ;
}

void DrawElement::drawScanLine(Model *in_model) {
	int scanlinePosy = 0;
	int scanlinePosx = 0;
	int pointPos;
	int R, G, B;
	bool isDraw = true;
	list<ActivateEdge> nowEdgeList;
	list<Face> nowFaceList;

	list<Face>::iterator nowFaceIter;
	list<ActivateEdge>::iterator nowEdgeIter, tempEdgeIter;
	for (; scanlinePosy < WINDOW_HEIGHT; scanlinePosy++) {
		if (nowEdgeList.size() == 0 && scanlineArray[scanlinePosy].edgeList.size() == 0) {
			continue;
		}
		if (nowEdgeList.size() == 0) {
			nowEdgeList = scanlineArray[scanlinePosy].edgeList;
		}
		else {
			nowEdgeList.splice(++nowEdgeList.begin(), scanlineArray[scanlinePosy].edgeList);
		}


		for (int i = 0; i < in_model->faceVector.size(); i++) {
			in_model->faceVector[i].in_out_flag = false;
		}
		nowFaceList.clear();
		nowEdgeList.sort(EdgeXcompareRule);
		nowEdgeIter = nowEdgeList.begin();

		for (scanlinePosx = nowEdgeList.front().x; scanlinePosx <= nowEdgeList.back().x && scanlinePosx < WINDOW_WIDTH && nowEdgeIter!= nowEdgeList.end();) {
			pointPos = scanlinePosy*WINDOW_WIDTH + scanlinePosx;
			if ((int)scanlinePosx == (int)(*nowEdgeIter).x) {
				isDraw = true;
				for (tempEdgeIter = nowEdgeList.begin(); tempEdgeIter != nowEdgeList.end(); tempEdgeIter++) {
					if ((*nowEdgeIter).ymax == scanlinePosy && ((*nowEdgeIter).x - (*tempEdgeIter).x)<0.02f && (*tempEdgeIter).ymax!=scanlinePosy&&(*nowEdgeIter).id == (*tempEdgeIter).id) {
						isDraw = false;
					}
				}
				if (isDraw == true) {
					in_model->faceVector[(*nowEdgeIter).id].in_out_flag = !in_model->faceVector[(*nowEdgeIter).id].in_out_flag;
				}
				if (in_model->faceVector[(*nowEdgeIter).id].in_out_flag == true) {
					//in_model->faceVector[(*nowEdgeIter).id].nowZ = (-in_model->faceVector[(*nowEdgeIter).id].A*scanlinePosx - in_model->faceVector[(*nowEdgeIter).id].B*scanlinePosy - in_model->faceVector[(*nowEdgeIter).id].D) / in_model->faceVector[(*nowEdgeIter).id].C;
					in_model->faceVector[(*nowEdgeIter).id].id = (*nowEdgeIter).id;
					nowFaceList.push_back(in_model->faceVector[(*nowEdgeIter).id]);	
					nowFaceList.sort(FaceZcompareRule);
					R = (int)(nowFaceList.front().r * 255.0f);
					G = (int)(nowFaceList.front().g * 255.0f);
					B = (int)(nowFaceList.front().b * 255.0f);
				}
				else {
					for (nowFaceIter = nowFaceList.begin(); nowFaceIter != nowFaceList.end(); ) {
						if ((*nowFaceIter).id == (*nowEdgeIter).id) {
							nowFaceIter = nowFaceList.erase(nowFaceIter);
						}
						else {
							nowFaceIter++;
						}
					}
					if (nowFaceList.size() == 0) {
						R = 0;
						G = 0;
						B = 0;
					}
					else {
						nowFaceList.sort(FaceZcompareRule);
						R = (int)(nowFaceList.front().r * 255.0f);
						G = (int)(nowFaceList.front().g * 255.0f);
						B = (int)(nowFaceList.front().b * 255.0f);
					}

				}
				nowEdgeIter++;
			}
			if (nowEdgeIter != nowEdgeList.end()&&(int)scanlinePosx < (int)(*nowEdgeIter).x) {
				scanlinePosx++;
			}
			this->bmpBuffer_[pointPos] = (R << 16) | (G << 8) | B;
		}

		for (nowEdgeIter = nowEdgeList.begin(); nowEdgeIter != nowEdgeList.end();) {
			(*nowEdgeIter).x += (*nowEdgeIter).dx;
			if ((*nowEdgeIter).ymax < scanlinePosy + 1) {
				nowEdgeList.erase(nowEdgeIter++);
			}
			else {
				nowEdgeIter++;
			}
		}

	}

}

void DrawElement::updateScene() {
	HDC hDC = GetDC(hwnd);
	SelectObject(hMemDC, hBITMAP);
	BitBlt(hDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hMemDC, 0, 0, SRCCOPY);
	//ReleaseDC(hwnd, hDC);
	//ReleaseDC(hwnd, hMemDC);
}
